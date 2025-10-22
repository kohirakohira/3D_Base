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
    , m_SeekRadius          ( 5.0f ) 
    , m_AttacRadius         ( 3.0f )
    , m_FireConeDeg         ( 10.0f )
    , m_ClosenessRadius     ( 1.f )     //近くにしすぎない
    , m_EvadeDuration       ( 60 )
    , m_ComEnabled          ( true )
    , m_EvadeFrames         ( 60 )
    , m_TargetRadius        ( 20.0f)
    , m_IsTarget            ( false )   //最初はターゲットではない
    , m_LostSightFrames     ()
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
    D3DXVECTOR3 target = targetPos - pos;
    target.y = 0.0f;
    const float d2 = target.x * target.x + target.z * target.z;

    // 目標方位へ最短差で回頭
    if (d2 > 1e-6f) {
        const float desiredYaw = std::atan2f(target.x, target.z);               // +Z前 左手座標
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), tuning.turretTurnSpeed);
    }

    D3DXVECTOR3 chaseDir(0, 0, 0);
    if (d2 > 1e-6f) {
        const float inv = 1.0f / std::sqrtf(d2);
        chaseDir.x = target.x * inv;    //正規化
        chaseDir.z = target.z * inv;
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
    const D3DXVECTOR3 target = targetPos - base;
    const float desiredYaw = std::atan2f(target.x, target.z);

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

    //プレイヤー操作の場合は無視
    if (!m_ComEnabled)
    {
        CPlayer::Update();
        return;
    }

    //ターゲット・自分の姿勢
    std::shared_ptr<CBody> body = Body();
    std::shared_ptr<CCannon> cannon = Cannon();

    if (!body)
    {
        if (cannon) cannon->CCharacter::Update();
        return;
    }

    //ターゲット情報の更新
    float dist = 1e9f;

#if 1
    if (m_pTarget)
    {
        m_LastSeenPos = m_pTarget->GetPosition();
        const D3DXVECTOR3 d = m_LastSeenPos - body->GetPosition();
        dist = std::sqrtf(d.x * d.x + d.z * d.z);
        m_LostSightFrames = 0;
    }
    else
    {
        ++m_LostSightFrames;
    }
#endif

    EvaluateTransitions(dist);

    //状態の実行
    switch (m_State)
    {
    case CComPlayer::State::Idle:
        StepIdle();
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
    case CComPlayer::State::ItemSeek:
        StepItemSeek();
        break;
    }
    ++m_StateFrames;
}

//待機処理
void CComPlayer::StepIdle()
{
    //BodyとCannonを取得.見た目だけ同期
    if (auto body = Body())
    {
        body->CCharacter::Update();
    }
    SyncCannonToBody();
    if (auto cannon = Cannon())
    {
        cannon->CCharacter::Update();
    }
}

//追尾    
void CComPlayer::StepChase()
{
    auto body = Body();
    const D3DXVECTOR3 target = m_pTarget->GetPosition();;
    TickChaseTo(target);    //本体は追尾
    TickAimTo(target);      //砲塔は常に狙う
    TryAutoFire();          //一旦ここでも発射処理
}

//攻撃、基本的には弾発射処理
void CComPlayer::StepAttack()
{
    //近距離は動きを停止して照準.中距離はちょっとだけ前進する
    const D3DXVECTOR3 target = m_pTarget->GetPosition();
    const float dist = DistXZ(Body() ->GetPosition(), target);
    if (dist > m_KeepDistance * 0.9f)
    {
        //少しだけ詰める
        TickChaseTo(target);
    }
    else
    {
        //位置更新だけ
        if (auto body = Body())
        {
            SyncCannonToBody();
            body->CCharacter::Update();
        }
    }
    TickAimTo(target);

    //発射判定
    TryAutoFire();
}

//退避
void CComPlayer::StepEvade()
{
    auto tuning = GetTuning();

   //ターゲットと反対方向に少し下がる
    std::shared_ptr<CBody> body = Body();
    if (!body) return;

    const D3DXVECTOR3 selfPos = body->GetPosition();
   
    D3DXVECTOR3 targetPos = selfPos;
    if (m_pTarget)
    {
        targetPos = m_pTarget->GetPosition();
    }

    //水平面でターゲットの反対方向に移動
    D3DXVECTOR3 away = selfPos - targetPos; //相手から離れる向きのベクトル
    away.y = 0.0f;

    const float len2 = away.x * away.x + away.z * away.z;
    if (len2 > 1e-6f)
    {
        //正規化
        const float invLen = 1.0f / std::sqrtf(len2);
        away.x *= invLen;
        away.z *= invLen;
        
        //少し後退
        const float step = tuning.moveSpeed * 0.6f;
        const D3DXVECTOR3 pos = selfPos + away * step;

        //逃げ方向へ向きを寄せる
        float yaw = body->GetRotation().y;
        const float desired = std::atan2f(away.x, away.z);
        const float delta = Wrap(desired - yaw);
        yaw = Approach(yaw, yaw + delta, tuning.turretTurnSpeed);

        body->SetPosition(pos);
        body->SetRotation(D3DXVECTOR3(0.0f,yaw, 0.0f));

        //位置とか回転反映の直後に同期
        SyncCannonToBody();
        TryAutoFire();  //逃げながら発射

        body->CCharacter::Update();

        //砲塔も見た目だけ更新
        if (auto cannon = Cannon())
        {
            cannon->CCharacter::Update();
        }
    }
}

//アイテム取得.アイテム認識
void CComPlayer::StepItemSeek()
{
    auto tuning = GetTuning();
    std::shared_ptr<CBody> body = Body();
    std::shared_ptr<CItemBox> ItemBox;

    if (!body) return;

    if (ItemBox)
    {
        //アイテム情報を取得する
        ItemBox->GetItemInfo();
        D3DXVECTOR3 item = ItemBox->GetPosition();

        TickAimTo(item);
        TickChaseTo(item);
    }
}

//動作切り替え
void CComPlayer::EvaluateTransitions(float dist)
{
    const float attackEnter = m_KeepDistance * 1.05f;   //既存値から派生
    const float attackExit = m_KeepDistance * 1.25f;    //ヒステリシス
    const float evadeDist = m_KeepDistance * 0.60f;     //近すぎの目安
    const int   loseFrames = 120;                       //2秒

    switch (m_State) {
    case State::Idle:
        if (m_pTarget) ChangeState(State::Chase);
        break;

    case State::Chase:
        if (!m_pTarget) { ChangeState(State::Idle); break; }
        if (dist <= evadeDist) { ChangeState(State::Evade);  break; }
        if (dist <= attackEnter) { ChangeState(State::Attack); break; }
        break;

    case State::Attack:
        if (!m_pTarget) { ChangeState(State::Idle);   break; }
        if (dist < evadeDist) { ChangeState(State::Evade);  break; }
        if (dist > attackExit) { ChangeState(State::Chase);  break; }
//        else if (dist >= attackEnter) { ChangeState(State::ItemSeek); break; }
        break;

    case State::Evade:
        if (!m_pTarget) { ChangeState(State::Idle);   break; }
        if (dist >= attackEnter) { ChangeState(State::Chase);  break; }
        else if (dist >= evadeDist) { ChangeState(State::Attack); break; }
        //見失い長すぎなら待機に戻す
        if (m_LostSightFrames > loseFrames) { ChangeState(State::Idle); }
        break;

    case State::ItemSeek:
        if (!m_pTarget) { ChangeState(State::Idle); break; }
        break;
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

    D3DXVECTOR3 muzzle; float yaw = 0.f;
    ComputeMuzzle(muzzle, yaw);

    D3DXVECTOR3 to = m_pTarget->GetPosition() - muzzle;
    to.y = 0.0f;
    const float d2 = to.x * to.x + to.z * to.z;
    if (d2 <= 1e-6f) return;

    const float desired = std::atan2f(to.x, to.z);
    const float err = std::fabs(Wrap(desired - yaw));

    //まずは広めに
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

}

//砲塔と車体の同期
void CComPlayer::SyncCannonToBody()
{
    auto tuning = GetTuning();
    auto body = Body();
    auto cannon = Cannon();
    if (!body || !cannon) return;

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y += tuning.cannonHeight;   //砲塔の高さオフセット
    cannon->SetPosition(pos);       //位置を同期
}

//ステータスを変更する
void CComPlayer::TransitionTo(State state)
{
    //ステータスが同じであればスキップ
    if (m_State == state) return;

    //ステータス更新
    m_State = state;
    m_StateFrames = 0;

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





