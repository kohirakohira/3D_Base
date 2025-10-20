#include "CComPlayer.h"

//-----ライブラリ-----
#include <cmath>

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h"       // 戦車：車体クラス
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h"   // 戦車：砲塔クラス

#undef min

// ---- 小ヘルパ ----
static inline float PI() { return D3DX_PI; }
static inline float TWO_PI() { return D3DX_PI * 2.0f; }

//静的レジストリ.複数をいっきに扱う
std::vector<CComPlayer*>& CComPlayer::Instances() {
    static std::vector<CComPlayer*> registry;
    return registry;
}

CComPlayer::CComPlayer()
    : m_KeepDistance        ( 9.0f )   //0ならベタ詰め
    , m_pTarget             ( nullptr )
    , m_AvoidRadius         ( 10.0f )
    , m_AvoidWeight         ( 0.8f )
    , m_Registered          ( false )
    , m_StateFrames         ( 0 )
    , m_SeekRadius          ( 5.0f ) //現状は確実に追尾してほしいので、大きい値に設定
    , m_AttacRadius         ( 12.0f )
    , m_FireConeDeg         ( 10.0f )
    , m_ClosenessRadius     ( 5.f )     //近くにしすぎない
    , m_EvadeDuration       ( 60 )
    , m_ComEnabled          ( true )
    , m_EvadeFrames         ( 60 )
#if 0
    , m_ShotCD()
    , MuzzleOffsetZ()
    , ShotCooldownFrames()
    , FireAngleEpsDeg()
#endif
{
}

//そのCOM自身を削除する処理
CComPlayer::~CComPlayer()
{
    if (m_Registered) {
        auto& comList = Instances();
        //comListのなかからthisを探してremoveで後ろに回す、無効かされたのをまとめて削除
        comList.erase(std::remove(comList.begin(), comList.end(), this), comList.end());
        m_Registered = false;
    }
}

void CComPlayer::Initialize(int id)
{
    CPlayer::Initialize(id);
    SanitizeParams();

    //自分がまだ登録されていなければ、全体リストに登録する
    if (!m_Registered) {
        Instances().push_back(this);
        m_Registered = true;
    }
}

void CComPlayer::SetTarget(std::shared_ptr<CPlayer> player)
{
    m_pTarget = player;
}
void CComPlayer::ClearTarget()
{
    m_pTarget = nullptr;
}

D3DXVECTOR3 CComPlayer::GetPosition() const
{
    if (m_pBody)return m_pBody->GetPosition();
    return CCharacter::GetPosition();
}

D3DXVECTOR3 CComPlayer::GetRotation() const
{
    if (m_pBody) return m_pBody->GetRotation();
    return CCharacter::GetRotation();
}

//不正値を防ぐ
void CComPlayer::SanitizeParams()
{
    auto& tuning = GetTuning();
    if (tuning.moveSpeed <= 0.0f) tuning.moveSpeed;
    if (tuning.bodyTurnSpeed <= 0.0f) tuning.bodyTurnSpeed;
    if (tuning.cannonHeight <= 0.0f) tuning.cannonHeight;
    if (tuning.turretTurnSpeed <= 0.0f) tuning.turretTurnSpeed;
    if (m_AvoidRadius < 0.0f)       m_AvoidRadius = 0.0f;
    if (m_AvoidWeight < 0.0f)       m_AvoidWeight = 0.0f;

}

//[-π,π]に正規化
float CComPlayer::Wrap(float a)
{
    while (a > PI())     a -= TWO_PI();
    while (a < -PI())     a += TWO_PI();
    return a;
}

//一方向にstepだけ近づける
float CComPlayer::Approach(float cur, float goal, float step)
{
    const float d = goal - cur;
    if (d > step)  return cur + step;
    if (d < -step) return cur - step;
    return goal;
}

D3DXVECTOR3 CComPlayer::ForwardFromYaw(float yaw)
{
    return D3DXVECTOR3(std::sinf(yaw), 0.0f, std::cosf(yaw));
}

//COM同士の分離ベクトルを計算
void CComPlayer::ComputeSeparation(const D3DXVECTOR3& selfPos,
        D3DXVECTOR3& outSep, float& outNearest)const
{
    outSep = D3DXVECTOR3(0, 0, 0);
    outNearest = 1e9f;  //大きい値.fをつけてるのはfloat型にするから
    
    if (m_AvoidRadius <= 0.0f)return;   //回避半径が0以下なら何もしない

    const float avoidRadius = m_AvoidRadius;
    const float avoidRadiusSq = avoidRadius * avoidRadius;

    for (CComPlayer* other : Instances()) {
        if (other == this) continue;
        std::shared_ptr<CBody> ob = other ? other->Body() : nullptr;
        if (!ob)continue;   //位置が取れない相手は無視する

        D3DXVECTOR3 offset = selfPos - ob->GetPosition();
        offset.y = 0.0f; //高さは無視

        const float distSq = offset.x * offset.x + offset.z * offset.z;
        if (distSq <= 1e-6f) {
            //ほぼ同一点のため少し押す
            outSep.x += 0.1f;
            continue;
        }
        //一番近い相手までの距離を更新
        outNearest = std::min(outNearest, std::sqrtf(distSq));

        if (distSq < avoidRadiusSq) {
            //近いほど強い反発
            const float invDistSq = 1.0f / distSq;
            outSep.x += offset.x * invDistSq;
            outSep.z += offset.z * invDistSq;
        }
    }
    //正規化は呼び出し側でブレンド時にやる
}

// 本体を常にターゲットへ回頭＋前進
void CComPlayer::TickChaseTo(const D3DXVECTOR3& targetPos)
{
    //パラメータ取得
    auto& tuning = GetTuning();

    std::shared_ptr<CBody> body = Body();
    if (!body) return;

    // 現在姿勢
    D3DXVECTOR3 pos = body->GetPosition();
    float yaw = body->GetRotation().y;

    // 水平面の差分
    D3DXVECTOR3 to = targetPos - pos; to.y = 0.0f;
    const float d2 = to.x * to.x + to.z * to.z;

    // 目標方位へ最短差で回頭
    if (d2 > 1e-6f) {
        const float desiredYaw = std::atan2f(to.x, to.z);               // +Z前 左手座標
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), tuning.turretTurnSpeed);
    }

    D3DXVECTOR3 chaseDir(0, 0, 0);
    if (d2 > 1e-6f) {
        const float inv = 1.0f / std::sqrtf(d2);
        chaseDir.x = to.x * inv;    //正規化
        chaseDir.z = to.z * inv;
    }

    //分離ベクトル
    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    ComputeSeparation(pos, sep, nearest);

    //ブレンド
    D3DXVECTOR3 desire = chaseDir;  //目標優先
    if (m_AvoidWeight > 0.0f && (sep.x != 0.0f || sep.z != 0.0f)) {
        //sepを正規化して重み付け
        const float sepLen = std::sqrt(sep.x * sep.x + sep.z * sep.z);
        if (sepLen > 1e-6f) {
            sep.x /= sepLen; sep.z /= sepLen;
            desire.x += sep.x * m_AvoidWeight;
            desire.z += sep.z * m_AvoidWeight;
        }
    }

    //合成方向が有効ならその方位にいく
    float desiredYaw = yaw;
    const float desLen2 = desire.x * desire.x + desire.z * desire.z;
    if (desLen2 > 1e-8f) {
        desiredYaw = std::atan2f(desire.x, desire.z);
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), tuning.turretTurnSpeed);
    }
 
    //前進量の決定
    float step = tuning.moveSpeed;
    if (d2 > 0.0f) {
        const float dist = std::sqrtf(d2);
        if (m_KeepDistance > 0.0f) {
            const float remain = dist - m_KeepDistance;
            if (remain <= 0.0f) {
                step = 0.0f;    //これ以上は詰めない
            }
            else if (step > remain) {
                step = remain;
            }
            else
            {
                if (step > dist)step = dist;
            }
        }
    }

    //COMが近すぎるときは減速.完全停止もする
    if (nearest < 1e9f && m_AvoidRadius > 0.0f) {
        float scale = nearest / m_AvoidRadius;
        if (scale < 0.0f) scale = 0.0f;
        if (scale > 1.0f) scale = 1.0f;
        step *= scale;
    }

    // 前進（ヨーに沿って +Z 基準で）
    if (step > 0.0f) {
        const D3DXVECTOR3 fwd = ForwardFromYaw(yaw);
        pos += fwd * step;
    }

    // 反映
    body->SetRotation(D3DXVECTOR3(0.0f, yaw, 0.0f));
    body->SetPosition(pos);
    body->CCharacter::Update();
}

// 砲塔があれば常にターゲットを向く
void CComPlayer::TickAimTo(const D3DXVECTOR3& targetPos)
{
    auto& tuning = GetTuning();
    std::shared_ptr<CCannon> cannon = Cannon();
    const std::shared_ptr<CBody> body = Body();
    if (!cannon) return;

    // 砲塔の基準位置
    D3DXVECTOR3 base = body ? body->GetPosition() : cannon->GetPosition();
    base.y += tuning.cannonHeight;

    // 目標方位
    const D3DXVECTOR3 to = targetPos - base;
    const float desiredYaw = std::atan2f(to.x, to.z);

    float cyaw = cannon->GetRotation().y;
    cyaw = Approach(cyaw, cyaw + Wrap(desiredYaw - cyaw),tuning.turretTurnSpeed);

    cannon->SetPosition(base);
    cannon->SetRotation(D3DXVECTOR3(0.0f, cyaw, 0.0f));
    cannon->CCharacter::Update();
}

//砲口のワールド座標とヨー角を計算
void CComPlayer::ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const
{
    auto& tunign = GetTuning();
    auto body = Body();
    auto cannon = Cannon();

    D3DXVECTOR3 base(0, 0, 0);
    float yaw = 0.0f;

    if (body) {
        base = body->GetPosition();
        yaw = body->GetRotation().y;
    }

    if (cannon) {
        //砲塔があれば向きを優先
        if (!body) base = cannon->GetPosition();
        yaw = cannon->GetRotation().y;
    }

    base.y += tunign.cannonHeight; //砲塔の高さオフセット
    const D3DXVECTOR3 forwared = ForwardFromYaw(yaw);

    outpos = base + forwared * m_ShotState.MuzzleOffsetZ;   //砲身先端オフセット
    outYaw = yaw;
}

void CComPlayer::Update()
{
    SanitizeParams();

    //cannon.bodyをPlayerから取得する
    auto body = Body();
    auto cannon = Cannon();

    //プレイヤー操作の場合は無視
    if (!m_ComEnabled)
    {
        CPlayer::Update();
        return;
    }

    //COM無効ならプレイヤー操作
    if (m_ComEnabled)
    {
        //ターゲット不在でも見た目は更新
        //存在するのかを確認
        if (!body) { if (cannon = Cannon()) cannon->CCharacter::Update(); return; }

        // 追尾対象がなければ回頭も移動もせず、そのまま更新.自己ターゲットは無視
        if (!m_pTarget || m_pTarget.get() == this) {
            body->CCharacter::Update();
            if (auto cannon = Cannon()) cannon->CCharacter::Update();
            TransitionTo(State::Idle);  //ターゲット未設定時は待機
            return;
        }
        const D3DXVECTOR3 target = m_pTarget->GetPosition();
        TickChaseTo(target);
        TickAimTo(target);
        return;
    }

    //それぞれをフレームごとに実行
    switch (m_State)
    {
    case CComPlayer::State::Idle:
        StepIdle();
        break;
    //case CComPlayer::State::Seek:
    //    StepSeek();
        break;
    case CComPlayer::State::Chase:
        StepChase();
        break;
    case CComPlayer::State::Attack:
        StepAttack();
        break;
    case CComPlayer::State::Evade:
        StepEvade();
        break;
    }
    ++m_StateFrames;

    //状態遷移
    const D3DXVECTOR3 selfPos = body->GetPosition();        //自分のPosition
    const float selfYaw = body->GetRotation().y;            //自分ヨー角
    const D3DXVECTOR3 targetPos = m_pTarget->GetPosition();  //現状はプレイヤーのみターゲット 
    const float dist = DistXZ(selfPos, targetPos);
    const float aimError = AngleError(selfYaw, selfPos, targetPos); //角度制限

    switch (m_State)
    {
    case CComPlayer::State::Idle:
        if (dist < m_SeekRadius)
        {
            //ターゲットがいる場合は追跡する
            TransitionTo(State::Chase);
        }
        break;
    case CComPlayer::State::Chase:
        if (dist < m_ClosenessRadius)
        {
            //近くしすぎたらEvadeにして退避を選ぶ.Item追加したらそっちでもあり.
            TransitionTo(State::Evade);
        }
        else if (dist <= m_AttacRadius)
        {   
            //近づいたら攻撃する
            TransitionTo(State::Attack);
        }
        break;
    case CComPlayer::State::Attack:
        //追跡したら戻る
        if (dist > m_AttacRadius * 1.25f)
        {
            TransitionTo(State::Chase);
        }
        else if (dist < m_ClosenessRadius)
        {
            TransitionTo(State::Evade);
        }
        break;
    case CComPlayer::State::Evade:
        if (--m_EvadeFrames <= 0)
        {
            //一定時間ごとで追尾
            TransitionTo(State::Chase);
        }
        break;
    //Seekは一旦なし
    //case CComPlayer::State::Seek:
    //    break;
    }

}

//COM弾発射処理
void CComPlayer::TryAutoFire()
{
    auto manager = m_pShotManager.lock();
    if (!manager || !m_pTarget) return;

    if (m_ShotState.m_ShotCD > 0)
    {
        --m_ShotState.m_ShotCD;
        return;
    }

    D3DXVECTOR3 muzzle;
    float yaw = 0.f;
    ComputeMuzzle(muzzle, yaw);

    D3DXVECTOR3 to = m_pTarget->GetPosition() - muzzle;
    to.y = 0.0f;
    const float d2 = to.x * to.x + to.z * to.z;
    if (d2 <= 1e-6f) return;

    const float desired = std::atan2f(to.x, to.z);
    const float err = std::fabs(Wrap(desired - yaw));
    if (err <= ToRad(m_ShotState.FireAngleEpsDeg)) {
        manager->SetReload(m_PlayerID, muzzle, yaw);
        m_ShotState.m_ShotCD = m_ShotState.ShotCooldownFrames;
    }
}


inline float CComPlayer::DistXZ(const D3DXVECTOR3& targetPos, const D3DXVECTOR3& selfPos)
{
    const float dx = targetPos.x - selfPos.x;
    const float dz = targetPos.z - selfPos.z;
    return std::sqrtf(dx * dx + dz * dz);

}

inline float CComPlayer::AngleError(float fromYaw, const D3DXVECTOR3& fromPos, const D3DXVECTOR3& toPos)
{
    D3DXVECTOR3 vec = toPos - fromPos;
    vec.y = 0.f;
    
    if (vec.x == 0 && vec.z == 0)
    {
        return 0.f;
    }

    const float desired = std::atan2f(vec.x, vec.z);
    const float error = Wrap(desired - fromYaw);
    return std::fabs(error);

#if 0
    D3DXVECTOR3 v = toPos - fromPos; v.y = 0.f;
    if (v.x == 0 && v.z == 0) return 0.f;
    const float desired = std::atan2f(v.x, v.z);
    const float err = Wrap(desired - fromYaw);
    return std::fabs(err);
#endif
}


//ステータスを変更する
void CComPlayer::TransitionTo(State state)
{
    //ステータスが同じであればスキップ
    if (m_State == state) return;

    //ステータス更新
    m_State = state;
    m_StateFrames = 0;

    //ステータスが離脱の際の秒数
    if (state == State::Evade)
    {
        m_EvadeFrames = m_EvadeDuration;
    }
}

//探索処理
void CComPlayer::StepSeek()
{
    //今は範囲内に入ったら追尾するシンプル仕様なので書かない
}

//待機処理
void CComPlayer::StepIdle()
{
    //BodyとCannonを取得.見た目だけ同期
    if (auto body = Body())
    {
        body->CCharacter::Update();
    }

    if (auto cannon = Cannon())
    {
        cannon->CCharacter::Update();
    }
}

void CComPlayer::StepChase()
{
    auto body = Body();
    auto cannon = Cannon();
    //プレイヤー位置取得
    D3DXVECTOR3 target = m_pTarget->GetPosition();

    //ターゲット不在または自分自身の場合
    if (!m_pTarget || m_pTarget.get() == this)
    {
        //ターゲット不在でも見た目だけ同期させておく
        body->CCharacter::Update();
        cannon->CCharacter::Update();
        TransitionTo(State::Idle);  //ターゲットの場合はIdle
    }
    else

    {   //ターゲット不在でも自分自身でもない場合
        TickChaseTo(target);
        TickAimTo(target);
    }
}

//攻撃、基本的には弾発射処理
void CComPlayer::StepAttack()
{
    auto body = Body();
    auto cannon = Cannon();

    //近距離では移動停止
    D3DXVECTOR3 selfPos = body->GetPosition();
    D3DXVECTOR3 targetPos = m_pTarget->GetPosition();
    float dist = DistXZ(selfPos, targetPos);

    if (dist > m_KeepDistance * 0.9f)
    {
        //ちょっとだけ動く
        TickChaseTo(targetPos);
    }
    else
    {
        //動かなくても見た目だけ同期
        if (auto body = Body())
        {
            body->CCharacter::Update();
        }
        TickAimTo(targetPos);   //ターゲットの方向にむく
        TryAutoFire();          //弾発射処理呼び出し
    }
}

//退避処理
void CComPlayer::StepEvade()
{
    //bodyがないスキップ
    std::shared_ptr<CBody> body = Body();
    if (!body) return;


    D3DXVECTOR3 selfPos = m_pTarget->GetPosition();
    D3DXVECTOR3 targetPos = body->GetPosition();
    D3DXVECTOR3 vec = selfPos - targetPos;
    vec.y = 0.f;
    const float len = vec.x* vec.x + vec.z * vec.z;
}

#if 0
}
    // ターゲットと反対方向に少し下がる（簡易版）
    std::shared_ptr<CBody> body = Body();
    if (!body) return;

    const D3DXVECTOR3 selfPos = body->GetPosition();
    const D3DXVECTOR3 tp = m_pTarget ? m_pTarget->GetPosition() : selfPos;
    D3DXVECTOR3 away = selfPos - tp; away.y = 0.f;

    const float len2 = away.x * away.x + away.z * away.z;
    if (len2 > 1e-6f) {
        const float inv = 1.0f / std::sqrtf(len2);
        away.x *= inv; away.z *= inv;

        // 反対方向へ少し移動
        const float step = MoveSpeed * 0.6f; // 逃げ速度は好みで
        D3DXVECTOR3 pos = selfPos + away * step;

        // 逃げ方向を向く
        float yaw = body->GetRotation().y;
        const float desired = std::atan2f(away.x, away.z);
        yaw = Approach(yaw, yaw + Wrap(desired - yaw), TurnStep);

        body->SetPosition(pos);
        body->SetRotation(D3DXVECTOR3(0, yaw, 0));
        body->CCharacter::Update();
    }
    if (auto c = Cannon()) c->CCharacter::Update();
}

#endif

