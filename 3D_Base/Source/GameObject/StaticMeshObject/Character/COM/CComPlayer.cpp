#include "CComPlayer.h"

//-----ライブラリ-----
#include <cmath>

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h"       // 戦車：車体クラス
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h"   // 戦車：砲塔クラス

#undef min
#undef max

//静的レジストリ.複数をいっきに扱う
std::vector<CComPlayer*>& CComPlayer::Instances() {
    static std::vector<CComPlayer*> registry;
    return registry;
}

CComPlayer::CComPlayer()
    : m_KeepDistance        ( 9.0f )   //0ならベタ詰め
    , m_pTarget             ( nullptr )
    , m_AvoidRadius         ( 10.0f )
    , m_AvoidWeight         ( 2.0f )
    , m_Registered          ( false )
    , m_StateFrames         ( 0 )
    , m_SeekRadius          ( 5.0f ) 
    , m_AttacRadius         ( 10.0f )
    , m_FireConeDeg         ( 10.0f )
    , m_ClosenessRadius     ( 1.f )     //近くにしすぎない
    , m_EvadeDuration       ( 60 )
    , m_ComEnabled          ( true )
    , m_EvadeFrames         ( 60 )
    , m_IsTarget            ( false )   //最初はターゲットではない
    , m_LostSightFrames     ()
    , m_pAllPlayer          ( nullptr )
    , m_RetargetInterval    ( 120 )
    , m_RetargetTimer       ( 0 )
    , m_ForgetDistance      ( 60.0f )
    , m_StickinessRatio     ( 0.8f)
    , m_CurTargetDist       ( 1e9f )
    , m_LastSeenPos         ( D3DXVECTOR3(0, 0, 0) )
    , m_State               ( State::Seek )
    , m_WanderAngle         ( 0.f )
    , m_BlackListTime       ( 120 )     //3秒くらいだけ
    , m_CurTargetDist2      ( std::numeric_limits<float>::infinity() )
    , m_pItemBox            ( nullptr )
    , m_pItemTarget         ()
    , m_RetargetItemTimer   ( 0 )
    , m_RetargetItemInterval( 30 )
    , m_ItemGetRadius       ( 20.f )
    , m_ItemPickUpRaius     ( 1.f )
    , m_pBoxCollider        ( nullptr )
    , m_ProdeAngleRad       ( 0.f ) 
    , m_ProdeDist           ( 10.f )
    , m_AvoidHolde          ( 0.f )
    , m_AvoidSide           ( 0 )
    , m_AvoidMax            ( 0.f )
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

//不正値を防ぐ
void CComPlayer::SanitizeParams()
{
    auto& tuning = GetTuning();
#if 0
    if (tuning.moveSpeed <= 0.0f)           tuning.moveSpeed;
    if (tuning.bodyTurnSpeed <= 0.0f)       tuning.bodyTurnSpeed;
    if (tuning.cannonHeight <= 0.0f)        tuning.cannonHeight;
    if (tuning.turretTurnSpeed <= 0.0f)     tuning.turretTurnSpeed;
#endif
    if (m_AvoidRadius < 0.0f)               m_AvoidRadius = 0.0f;
    if (m_AvoidWeight < 0.0f)               m_AvoidWeight = 0.0f;
    if (m_AttacRadius < 0.0f)               m_AttacRadius = 10.0f;
    if (m_SeekRadius < 0.0f)                m_SeekRadius = 5.0f;
    if (m_FireConeDeg < 0.0f)               m_FireConeDeg = 10.0f;
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

//COMの状態変更
void CComPlayer::ChangeState(State state)
{
    m_State = state;
    m_StateFrames = 0;
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

void CComPlayer::Update()
{
    SanitizeParams();

    if (!m_ComEnabled) { 
        CPlayer::Update(); 
        return;
    }
    
    TickBlacklist();

    auto body = Body();
    auto cannon = Cannon();
    if (!body) { 
        if (cannon) cannon->CCharacter::Update(); 
        return;
    }

    //定期リターゲット
    if (--m_RetargetTimer <= 0 || !m_pTarget) {
        MakeFixedTimeTarget();
        m_RetargetTimer = m_RetargetInterval;
    }

    //距離を計算
    float dist2 = 1e18f;
    if (m_pTarget) {
        const D3DXVECTOR3 d = m_pTarget->GetPosition() - body->GetPosition();
        dist2 = d.x * d.x + d.z * d.z;
        m_LostSightFrames = 0;
    }
    else {
        ++m_LostSightFrames;
    }
    
    float itemD2;
    NearestItemDist2(itemD2);

    //状態遷移はここだけで行う
    EvaluateTransitions(dist2);

    //実行
    switch (m_State) {
    case State::Seek:     StepSeek();     break;
    case State::Chase:    StepChase();    break;
    case State::Attack:   StepAttack();   break;
    case State::Evade:    StepEvade();    break;
    case State::ItemSeek: StepItemSeek(); break;
    }
    ++m_StateFrames;
}

//前方に当たり判定を設置する
bool CComPlayer::HasObstacleAheadWithBox(const CBoxCollider& selfBox,
    const D3DXVECTOR3& forward,
    float  prodeDist,
    float  step,
    float& outHitDist)const
{
    if (!m_pBoxCollider || m_pBoxCollider->empty()) return false;
    CBoxCollider ghost = selfBox;
    const D3DXVECTOR3 base = selfBox.GetPosition();

    for (float d = step; d <= prodeDist; d += step)
    {
        ghost.SetPosition(base + forward * d);
        for (const auto& object : *m_pBoxCollider)
        {
            if (object && ghost.CheckCollisionBox(*object))
            {
                outHitDist = d;
                return true;
            }
        }
    }
    return false;
}



//探索処理
void CComPlayer::StepSeek()
{
    //パラメータ取得
    const auto tuning = GetTuning();
    TickWander(tuning.bodyTurnSpeed, tuning.moveSpeed);     //動作

    if (m_pTarget)
    {
        //回頭して狙ってうつ
        TickAimTo(m_pTarget->GetPosition());
        TryAutoFire();
    }
}


void CComPlayer::StepChase()
{

    //パラメータ
    const auto tuning = GetTuning();
    TickWander(tuning.bodyTurnSpeed, tuning.moveSpeed);

    if (m_pTarget)
    {
        TickAimTo(m_pTarget->GetPosition());
        TryAutoFire();
    }
#if 0
    auto body = Body(); if (!body || !m_pTarget) { StepSeek(); return; }
    const auto t = GetTuning();

    const D3DXVECTOR3 pos = body->GetPosition();
    const float cur = body->GetRotation().y;

    const D3DXVECTOR3 to = m_pTarget->GetPosition() - pos;
    const float desired = std::atan2f(to.x, to.z);

    const float nextYaw = SteerWithAvoidAABB(cur, desired, t.bodyTurnSpeed);

//  SafeAdvance(*body, nextYaw, t.moveSpeed);

    //砲塔・発砲
    body->CCharacter::Update();
    SyncCannonToBody();
    TickAimTo(m_pTarget->GetPosition());
    TryAutoFire();
#endif
}



//攻撃、基本的には弾発射処理
void CComPlayer::StepAttack()
{
    auto tuning = GetTuning();
    TickWander(tuning.bodyTurnSpeed, tuning.moveSpeed);
    if (m_pTarget)
    {
        TickAimTo(m_pTarget->GetPosition());
        TryAutoFire();
    }
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

        //砲塔の見た目を更新
        if (auto cannon = Cannon())
        {
            cannon->CCharacter::Update();
        }
    }
}

//アイテム取得.アイテム認識
void CComPlayer::StepItemSeek()
{
    auto item = std::shared_ptr<CItemBox>();
    item->SpeedUpEffect();
    item->PowerUpEffect();
}

void CComPlayer::EvaluateTransitions(float dist2)
{
    //2乗
    const float attackEnter2 = Sqr(std::max(m_KeepDistance * 1.05f, 3.f));
    const float attackExit2 = Sqr(std::max(m_KeepDistance * 1.25f, 5.f));
    const float evadeDist2 = Sqr(m_KeepDistance * 0.60f);
    const int   loseFrames = 120;

    switch (m_State) {
    case State::Seek:
        if (m_pTarget) ChangeState(State::Chase);
        break;
    case State::Chase:
        if (!m_pTarget) { ChangeState(State::Seek);  break; }
        if (dist2 <= evadeDist2) { ChangeState(State::Evade); break; }
        if (dist2 <= attackEnter2) { ChangeState(State::Attack); break; }
        break;
    case State::Attack:
        if (!m_pTarget) { ChangeState(State::Seek);  break; }
        if (dist2 < evadeDist2) { ChangeState(State::Evade); break; }
        if (dist2 > attackExit2) { ChangeState(State::Chase); break; }
        break;
    case State::Evade:
        if (!m_pTarget) { ChangeState(State::Seek);  break; }
        if (dist2 >= attackEnter2) { ChangeState(State::Chase); break; }
        else if (dist2 >= evadeDist2) { ChangeState(State::Attack); break; }
        if (m_LostSightFrames > loseFrames) { ChangeState(State::Seek); }
        break;
    case State::ItemSeek:
        if (!m_pTarget) ChangeState(State::Seek);
        break;
    }
}

//一番近いターゲットを狙う
void CComPlayer::MakeFixedTimeTarget()
{ 
    if (!m_pAllPlayer) return;
    auto body = Body();
    if (!body) return;

    const D3DXVECTOR3 self = body->GetPosition();

    std::shared_ptr<CPlayer> best;
    float bestD2 = 1e9f;

    for (auto& p : *m_pAllPlayer) {
        if (!p) continue;
        if (p->GetPlayerID() == m_PlayerID) continue; // 自分は除外
        if (IsBlacklisted(p->GetPlayerID())) continue;

        const float d2 = DistXZ(self, p->GetPosition());
        if (d2 < bestD2) { bestD2 = d2; best = p; }
    }

    if (!best) {
        m_pTarget.reset();
        m_CurTargetDist2 = 1e9f;
        return;
    }

    if (!m_pTarget) {
        m_pTarget = best;
        m_CurTargetDist2 = bestD2;
        return;
    }
    
    //近いターゲット
    const float curD2 = DistXZ(self, m_pTarget->GetPosition());
    if (best.get() != m_pTarget.get() && bestD2 < curD2 * m_StickinessRatio) {
        m_pTarget = best;
        m_CurTargetDist2 = bestD2;
    }
    else {
        m_CurTargetDist2 = curD2;
    }
    
    //遠くなったら忘れさせる
    const float forget2 = m_ForgetDistance * m_ForgetDistance;
    if (m_CurTargetDist2 > forget2) {
        Blacklist(m_pTarget->GetPlayerID());
        m_pTarget.reset();
        m_CurTargetDist2 = 1e9f;
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
        manager->SetReload(BulletKinds::Mesh_2, muzzle, yaw);
        m_ShotState.m_ShotCD = m_ShotState.ShotCooldownFrames;
    }
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


void CComPlayer::TickWander(float turnStep, float moveStep)
{
    //body取得
    auto body = Body();
    if (!body) return;

    const auto tuning = GetTuning();

    const float WanderDelta = 0.08f;
    const float WanderClamp = 0.6f;

    //乱数設定
    int randomBit = std::rand() & 1;

    if (randomBit != 0)
    {
        m_WanderAngle += WanderDelta;
    }
    else
    {
        m_WanderAngle -= WanderDelta;
    }

    //クランプ
    if (m_WanderAngle > WanderClamp)
    {
        m_WanderAngle = WanderClamp;
    }

    if (m_WanderAngle < -WanderClamp)
    {
        m_WanderAngle = -WanderClamp;
    }

    //回頭
    float yawRed = body->GetRotation().y;
    yawRed = Approach(yawRed, yawRed + m_WanderAngle, turnStep);

    //位置
    D3DXVECTOR3 pos = body->GetPosition();

    //分離の情報
    D3DXVECTOR3 dir(0, 0, 0);
    float Dist = 1e9f;  //大きい値
    ComputeSeparation(pos, dir, Dist);

    //後退
    D3DXVECTOR3 Dir = ForwardFromYaw(yawRed);
    pos += Dir * moveStep;
    pos.x += dir.x * 0.02f;
    pos.z += dir.z * 0.02f;

    //見た目に反映
    body->SetRotation({ 0.f,yawRed,0.f });
    body->SetPosition(pos);
    body->CCharacter::Update();

    //砲塔を車体に追尾
    SyncCannonToBody();
}

//IDがリストに登録されているか判定
//読み取り専用
bool CComPlayer::IsBlacklisted(int id) const
{
    auto it = m_TargetBlackList.find(id);
    return it != m_TargetBlackList.end();
}

//フレームごとにブラックリストを更新
void CComPlayer::TickBlacklist()
{
    for (auto it = m_TargetBlackList.begin(); it != m_TargetBlackList.end();)
    {
        if (--(it->second) <= 0) {	//残りフレーム数を減らす
            it = m_TargetBlackList.erase(it);	//0以下ならその要素を削除して次のア
        }
        else {
            ++it;	//まだ残っているなら次
        }
    }

}

float CComPlayer::NearestItemDist2(float& outDist2) const
{
    //大きい値
    outDist2 = 1e18f;
    if (!m_pItemBox) return outDist2;

    auto body = Body();
    if (!body)
    {
        return outDist2;
    }
    const D3DXVECTOR3 self = body->GetPosition();
    for (auto& box : *m_pItemBox)
    {
        if (!box) continue;
        if (!box->IsActive()) continue;

        const D3DXVECTOR3 dist = box->GetPosition() - self;
        
        const float d2 = dist.x * dist.x + dist.z * dist.z;
        if (d2 < outDist2)
        {
            outDist2 = d2;
        }
        return outDist2;
    }
}

void CComPlayer::MakeItemTarget()
{
    if (!m_pItemBox)
    {
        m_pItemTarget.reset();
        return;
    }

    auto body = Body();
    if (!body)
    {
        m_pItemTarget.reset();
        return;
    }
    const D3DXVECTOR3 self = body->GetPosition();
    std::shared_ptr<CItemBox> best;
    float DistD2 = 1e18f;

    for (auto& box : *m_pItemBox)
    {
        if (!box || !box->IsActive()) continue;
        const D3DXVECTOR3 dist = box->GetPosition() - self;
        const float d2 = dist.x * dist.x + dist.z * dist.z;
        
        if (best && DistD2 <= (m_ItemGetRadius * m_ItemGetRadius)) {
            m_pItemTarget = best;
        }
        else {
            m_pItemTarget.reset();
        }
    }
}

bool CComPlayer::SenseObstacleAABB(const CBoxCollider& selfBox, float yaw, D3DXVECTOR3& outAvoid, float& nearest) const
{
    const float angs[3] = { 0.f, +m_ProdeAngleRad, -m_ProdeAngleRad };
    const float step = 0.30;
    const float prode = m_ProdeDist;
    
    bool any = false;
    outAvoid = { 0,0,0 };
    nearest = 1e9f;

    for (float a : angs)
    {
        D3DXVECTOR3 dir = ForwardFromYaw(yaw + a);
        dir.y = 0.f;
        float hitD;
        if (HasObstacleAheadWithBox(selfBox, dir, prode, step, hitD))
        {
            any = true;
            nearest = std::min(nearest, hitD);
            //前線ベクトル
            D3DXVECTOR3 tang = { dir.z, 0.f,-dir.x };
            outAvoid.x += tang.x;
            outAvoid.z += tang.z;
        }
    }
    return any;
}

float CComPlayer::SteerWithAvoidAABB(float curYaw, float desiredYaw, float turnStep)
{
    auto body = Body();
    if (!body)
    {
        return curYaw;
    }
    
    auto box = std::dynamic_pointer_cast<CBoxCollider>(body->GetCollider());
    if (!box)
    {

        return curYaw;
    }
    D3DXVECTOR3 avoid;
    float nearHit;
    const bool blocke = SenseObstacleAABB(*box, curYaw, avoid, nearHit);
    
    if(m_AvoidHolde > 0)
    { 
        --m_AvoidHolde;
        return curYaw + turnStep * (float)m_AvoidSide;
    }
    //基本的には左
    if (blocke)
    {
        if (m_AvoidSide  == 0)
        {
            m_AvoidSide = (avoid.x + avoid.z >= 0) ? +1 : -1;
        }
        m_AvoidHolde = m_AvoidMax;
        return curYaw + turnStep * m_AvoidSide;
    }
    
    //通常時の動作
    const float d = Wrap(curYaw - desiredYaw);
    if (d > turnStep) return curYaw + turnStep;
    if (d < -turnStep) return curYaw - turnStep;
    return curYaw + d;
}









