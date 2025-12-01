#include "CComPlayer.h"

//-----ライブラリ-----
#include <cmath>

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"       // 戦車：車体クラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"   // 戦車：砲塔クラス

#undef min

//静的レジストリ.複数をいっきに扱う
std::vector<CComPlayer*>& CComPlayer::Instances() {
    static std::vector<CComPlayer*> registry;
    return registry;
}

void CComPlayer::SetTarget(const std::shared_ptr<CCharacterObjectBase>& actor)
{
    m_pTarget = actor;
}

CComPlayer::CComPlayer()
    : m_KeepDistance        ( 9.0f )   //0ならベタ詰め
    , m_pTarget             ()
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
    , m_BodyRadius          ( 1.f )
    , m_Respawn             ( false )
    , m_ObstacleRadius      ( 3.0f )
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

void CComPlayer::OnHit(CCharacterObjectBase* other)
{
}

void CComPlayer::Create(int id)
{
    m_PlayerID = id;

    //それぞれのIDを渡して既存のBody,Cannonの設計に準拠する
    m_pBody = std::make_shared<CBody>(id);
    m_pCannon = std::make_shared<CCannon>(id);

    //生存.描画フラグ
    m_IsAlive = true;
    m_IsActive = true;
    m_Drawflag = true;

    //自分がまだ登録されていなければ、全体リストに登録する
    if (!m_Registered) {
        Instances().push_back(this);
        m_Registered = true;
    }

}

// コライダーの作成
void CComPlayer::CreateCollider()
{
    m_pBody->CreateBoxCollider(m_pBody->GetMinPos(), m_pBody->GetMaxPos());
    m_pCannon->CreateBoxCollider(m_pCannon->GetMinPos(), m_pCannon->GetMaxPos());
}

//不正値を防ぐ
void CComPlayer::SanitizeParams()
{
    //if (m_Tuning.moveSpeed <= 0.0f)             m_Tuning.moveSpeed;
    //if (m_Tuning.bodyTurnSpeed <= 0.0f)         m_Tuning.bodyTurnSpeed;
    //if (m_Tuning.cannonHeight <= 0.0f)          m_Tuning.cannonHeight;
    //if (m_Tuning.turretTurnSpeed <= 0.0f)       m_Tuning.turretTurnSpeed;
    if (m_AvoidRadius < 0.0f)                   m_AvoidRadius = 0.0f;
    if (m_AvoidWeight < 0.0f)                   m_AvoidWeight = 0.0f;
    if (m_AttacRadius < 0.0f)                   m_AttacRadius = 10.0f;
    if (m_SeekRadius < 0.0f)                    m_SeekRadius = 5.0f;
    if (m_FireConeDeg < 0.0f)                   m_FireConeDeg = 10.0f;
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
        std::shared_ptr<CBody> ob = other ? other->GetBody() : nullptr;
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
    std::shared_ptr<CBody> body = GetBody();
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
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), m_Tuning.turretTurnSpeed);
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
        yaw = Approach(yaw, yaw + Wrap(desiredYaw - yaw), m_Tuning.turretTurnSpeed);
    }
 
    //前進量の決定
    float step = m_Tuning.moveSpeed;
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
    body->CStaticMeshObject::Update();
}

// 砲塔があれば常にターゲットを向く
void CComPlayer::TickAimTo(const D3DXVECTOR3& targetPos)
{
    std::shared_ptr<CCannon> cannon = GetCannon();
    const std::shared_ptr<CBody> body = GetBody();
    if (!cannon) return;

    // 砲塔の基準位置
    D3DXVECTOR3 base = body ? body->GetPosition() : cannon->GetPosition();
    base.y += m_Tuning.cannonHeight;

    // 目標方位
    const D3DXVECTOR3 target = targetPos - base;
    const float desiredYaw = std::atan2f(target.x, target.z);

    float cyaw = cannon->GetRotation().y;
    cyaw = Approach(cyaw, cyaw + Wrap(desiredYaw - cyaw), m_Tuning.turretTurnSpeed);

    cannon->SetPosition(base);
    cannon->SetRotation(D3DXVECTOR3(0.0f, cyaw, 0.0f));
    cannon->CStaticMeshObject::Update();
}

//砲口のワールド座標とヨー角を計算
void CComPlayer::ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const
{
    const auto& tuning = GetTuning();

    auto body = GetBody();
    auto cannon = GetCannon();

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

    base.y += tuning.cannonHeight; //砲塔の高さオフセット
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
//旧Update
#if 0
void CComPlayer::Update()
{
    CCharacterObjectBase::Update();

    SanitizeParams();

    if (!m_ComEnabled) { 
        SyncCannonToBody(); //COM無効なら止めて砲塔追従だけ
        return;
    }
    
    TickBlacklist();

    auto body = GetBody();
    auto cannon = GetCannon();
    if (!body) { 
        if (cannon) cannon->CStaticMeshObject::Update();
        return;
    }

    //障害物の乗り込み対策.yを0で固定
    auto pos = body->GetPosition();
    body->SetPosition(pos.x, pos.y = 0, pos.z);

    //定期リターゲット
    if (--m_RetargetTimer <= 0 || !m_pTarget) {
        //MakeFixedTimeTarget();
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

    //状態遷移はここだけで行う
    EvaluateTransitions(dist2);

    //実行
    switch (m_State) {
    //case State::Seek:     StepSeek();     break;
    case State::Chase:    StepChase();    break;
    case State::Attack:   StepAttack();   break;
    case State::Evade:    StepEvade();    break;
    case State::ItemSeek: StepItemSeek(); break;
    }
    ++m_StateFrames;
}
#endif

void CComPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
    if (m_Drawflag)
    {
        m_pBody->Draw(View, Proj, Light, Camera);
        m_pCannon->Draw(View, Proj, Light, Camera);
        m_pCannon->SetScale(D3DXVECTOR3(1.8f, 1.8f, 1.8f));
    }
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


//旧StepSeek,StepAttack,StepChase
#if 0
//探索処理
void CComPlayer::StepSeek()
{
    auto Target = m_pTarget.lock();

    //パラメータ取得
    TickWander(m_Tuning.bodyTurnSpeed, m_Tuning.moveSpeed);     //動作

    if (Target)
    {
        //回頭して狙ってうつ
        TickAimTo(Target->GetPosition());
        TryAutoFire();
    }
}

void CComPlayer::StepChase()
{
    auto Target = m_pTarget.lock();

    //パラメータ
    TickWander(m_Tuning.bodyTurnSpeed, m_Tuning.moveSpeed);

    if (Target)
    {
        TickAimTo(Target->GetPosition());
        TryAutoFire();
    }
}

//攻撃、基本的には弾発射処理
void CComPlayer::StepAttack()
{
    auto Target = m_pTarget.lock();

    TickWander(m_Tuning.bodyTurnSpeed, m_Tuning.moveSpeed);
    if (Target)
    {
        TickAimTo(Target->GetPosition());
        TryAutoFire();
    }
}
#endif
void CComPlayer::Update()
{
    SanitizeParams();

    //見た目だけ更新
    if (!m_ComEnabled) {
        CStaticMeshObject::Update();
        return;
    }

    auto tuning = GetTuning();

    FollowPath(tuning.turretTurnSpeed, tuning.moveSpeed);

    //毎フレームブラックリスト更新
    TickBlacklist();

    auto body = GetBody();
    auto cannon = GetCannon();

    //bodyが存在しない
    if (!body) {
        if (cannon) cannon->CStaticMeshObject::Update();   //cannonだけ動かす
        return;
    }

    auto Target = m_pTarget.lock();

    //定期リターゲット
    if (--m_RetargetTimer <= 0 || !Target) {
        MakeFixedTimeTarget();
        m_RetargetTimer = m_RetargetInterval;
    }

    //距離を計算
    float dist2 = 1e18f;
    if (Target) {
        const D3DXVECTOR3 d = Target->GetPosition() - body->GetPosition();
        dist2 = d.x * d.x + d.z * d.z;
        m_LostSightFrames = 0;
    }
    else {
        ++m_LostSightFrames;
    }

    D3DXVECTOR3 pos = body->GetPosition();
    body->SetPosition(pos.x, pos.y = 0, pos.z);

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
    //case State::ItemSeek: StepItemSeek(); break;
    }
    ++m_StateFrames;
}

bool CComPlayer::IsInDangerZone(const D3DXVECTOR3& pos) const
{
    if (!m_pSimpleObstacles) return false;

    for (const auto& z : *m_pSimpleObstacles)
    {
        const float dx = pos.x - z.pos.x;
        const float dz = pos.z - z.pos.z;
        const float r = z.radius + m_ObstacleRadius; // 自分の大きさも足す
        if (dx * dx + dz * dz < r * r)
        {
            return true; // 危険ゾーンに入っている
        }
    }
    return false;
}


//旧MakeFixedTimeTarget
#if 0
void CComPlayer::MakeFixedTimeTarget()
{
    if (!m_pAllPlayer) return;
    auto body = GetBody();
    if (!body) return;

    const D3DXVECTOR3 self = body->GetPosition();

    std::shared_ptr<CCharacterObjectBase> best;
    float bestD2 = std::numeric_limits<float>::infinity();

    //auto pAllPlayer = m_pTarget.lock();

    for (auto& p : *m_pTarget.lock()) {
        if (!p) continue;
        if (p.get() == this) continue;  // 自分は除外
        if (IsBlacklisted(p.get())) continue;
        {
            const int pid = ResolveActorID(p);
            if (pid < 0) continue;                // ID取れない対象は無視
            if (IsBlacklisted(pid)) continue;     // 既存のIDベースBLをそのまま利用
        }

        const float d2 = DistXZ(self, p->GetPosition());
        if (d2 < bestD2) { bestD2 = d2; best = p; }
    }

    auto Target = m_pTarget.lock();

    if (!best) {
        Target.reset();
        m_CurTargetDist2 = 1e9f;
        return;
    }

    if (!Target) {
        Target = best;
        m_CurTargetDist2 = bestD2;
        return;
    }

    //近いターゲット
    const float curD2 = DistXZ(self, Target->GetPosition());
    if (best.get() != Target.get() && bestD2 < curD2 * m_StickinessRatio) {
        Target = best;
        m_CurTargetDist2 = bestD2;
    }
    else {
        m_CurTargetDist2 = curD2;
    }

    //遠くなったら忘れさせる
    const float forget2 = m_ForgetDistance * m_ForgetDistance;
    if (m_CurTargetDist2 > forget2) {
        Blacklist(GetPlayerID());
        const int tid = ResolveActorID(Target);
        if (tid >= 0) Blacklist(tid);   // 自分ではなく“ターゲット”のIDをBLへ
        Target.reset();
        m_CurTargetDist2 = 1e9f;
    }
}
#endif
// 一番近いターゲットを狙う
void CComPlayer::MakeFixedTimeTarget()
{
    // プレイヤー配列が無い or Body が無い場合はターゲット解除
    if (!m_pAllPlayer) {
        m_pTarget.reset();
        m_CurTargetDist2 = 1e9f;
        return;
    }

    auto body = GetBody();
    if (!body) {
        m_pTarget.reset();
        m_CurTargetDist2 = 1e9f;
        return;
    }

    const D3DXVECTOR3 self = body->GetPosition();

    //キャラ共通ベース型で OK
    std::shared_ptr<CCharacterObjectBase> best;
    float bestD2 = 1e9f;

    // 一番近いターゲットを探す（Player でも COM でも可）
    for (auto& p : *m_pAllPlayer) {
        if (!p) continue;

        // 自分自身は除外
        if (p->GetPlayerID() == m_PlayerID) continue;

        // ブラックリストも除外
        if (IsBlacklisted(p->GetPlayerID())) continue;

        const float d2 = DistXZ(self, p->GetPosition()); // XZ 平面距離^2
        if (d2 < bestD2) {
            bestD2 = d2;
            best = p; // shared_ptr<Base> 同士なので代入 OK
        }
    }

    // 候補がいない → ターゲット解除
    if (!best) {
        m_pTarget.reset();
        m_CurTargetDist2 = 1e9f;
        return;
    }

    // 現在のターゲット
    auto cur = m_pTarget.lock();

    // まだターゲットがいない / 失効している → 新しいターゲットに即決
    if (!cur) {
        m_pTarget = best;   
        m_CurTargetDist2 = bestD2;
        return;
    }

    // すでにターゲットがいる場合：乗り換え条件チェック
    const float curD2 = DistXZ(self, cur->GetPosition());
    if (best.get() != cur.get() && bestD2 < curD2 * m_StickinessRatio) {
        // もっと十分近い敵がいたら乗り換え
        m_pTarget = best;
        m_CurTargetDist2 = bestD2;
    }
    else {
        m_CurTargetDist2 = curD2;
    }

    // 遠くなりすぎたら忘れるブラックリスト入り
    const float forget2 = m_ForgetDistance * m_ForgetDistance;
    if (m_CurTargetDist2 > forget2) {
        Blacklist(cur->GetPlayerID());
        m_pTarget.reset();
        m_CurTargetDist2 = 1e9f;
    }
}

float CComPlayer::SteerWithAvoidAABB(float curYaw, float desiredYaw, float turnStep)
{
    auto body = GetBody();
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

    if (m_AvoidHolde > 0)
    {
        --m_AvoidHolde;
        return curYaw + turnStep * (float)m_AvoidSide;
    }
    //基本的には左
    if (blocke)
    {
        if (m_AvoidSide == 0)
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


float CComPlayer::NearestItemDist2(float& outDist2) const
{
    //大きい値
    outDist2 = 1e18f;
    if (!m_pItemBox) return outDist2;

    auto body = GetBody();
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

bool CComPlayer::FollowPath(float turnStep, float moveSte)
{
#if 0
    auto body = GetBody();
    if (!body) return false;

    const D3DXVECTOR3 pos = body->GetPosition();
    //常に見続ける
    while (!m_Path.empty())
    {
        D3DXVECTOR3 w = m_Path.front(); //先頭要素
        float dx = w.x - pos.x;
        float dz = w.z - pos.z;

        if (dx * dx + pos.z * pos.z > m_LookAheadSkep * m_LookAheadSkep)
        {
            m_Path.pop_front();
        }
        else break;
    }
    if (m_Path.empty()) return false;

    const D3DXVECTOR3 w = m_Path.front();
    float cur = body->GetRotation().y;
    float d = std::atan2f(w.x - pos.x, w.z - pos.z);

    // 既存の分離はYawにブレンド（詰まり回避）
    //desired = BlendYawBySeparation(desired, p);

    //float nextYaw = Approach(curYaw, curYaw + Wrap(desired - curYaw), turnStep);
#endif
}


#if 1
void CComPlayer::SafeAdvance(float nextYaw, float moveStep)
{
    auto body = GetBody();
    if (!body) return;

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y = 0.0f;

    // COM 同士の分離
    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    ComputeSeparation(pos, sep, nearest);

    // 次の候補位置を計算
    D3DXVECTOR3 nextPos = pos + ForwardFromYaw(nextYaw) * moveStep;
    nextPos.x += sep.x * 0.02f;
    nextPos.z += sep.z * 0.02f;
    nextPos.y = 0.0f;

    //ここで危険ゾーン判定
    if (IsInDangerZone(nextPos))
    {
        //今の向きのまま、そのフレームは前進しない
        body->SetRotation({ 0.f, nextYaw, 0.f });
        body->CStaticMeshObject::Update();  //見た目更新
        SyncCannonToBody();
        return;
    }

    // 実際に移動する
    body->SetRotation({ 0.f, nextYaw, 0.f });
    body->SetPosition(nextPos);
    body->CStaticMeshObject::Update();  //見た目更新
    SyncCannonToBody();
}
#endif

//旧StepSeek,StepChase,StepAttack
#if 1
void CComPlayer::StepSeek()
{
    auto body = GetBody();
    if (!body) return;
    const auto tuning = GetTuning();

    // Wander 更新を入れる
    TickWander(tuning.turretTurnSpeed, tuning.moveSpeed);

    const float cur = body->GetRotation().y;
    const float desired = cur + m_WanderAngle;

    const float next = SteerWithAvoidAABB(cur, desired, tuning.bodyTurnSpeed);
    //SafeAdvance(next, tuning.moveSpeed);

    if (auto Target = m_pTarget.lock())
    {
        TickAimTo(Target->GetPosition());
        TryAutoFire();
    }
}

void CComPlayer::StepChase()
{
    auto Target = m_pTarget.lock();

    auto body = GetBody(); if (!body || !Target) { StepSeek(); return; }
    const auto t = GetTuning();

    const D3DXVECTOR3 self = body->GetPosition();
    const D3DXVECTOR3 tp = Target->GetPosition();
    const float cur = body->GetRotation().y;

    //目標へ向く角度
    float desired = std::atan2f((tp - self).x, (tp - self).z);

    // 近づき過ぎないようKeepDistance 付近では少し横移動を入れる
    const float dist = DistXZ(self, tp);
    if (dist < m_KeepDistance * 0.9f) {
        desired = Wrap(desired + (D3DX_PI * 0.5f) * ((m_StateFrames / 60) % 2 ? +1.f : -1.f));
    }

    const float next = SteerWithAvoidAABB(cur, desired, t.bodyTurnSpeed);
    //SafeAdvance(next, t.moveSpeed);

    TickAimTo(tp);
    TryAutoFire();
}

void CComPlayer::StepAttack()
{
    auto Target = m_pTarget.lock();

    auto body = GetBody(); if (!body || !Target) { StepSeek(); return; }
    const auto t = GetTuning();

    const D3DXVECTOR3 self = body->GetPosition();
    const D3DXVECTOR3 tp = Target->GetPosition();
    const float cur = body->GetRotation().y;

    // 基本は周回
    const int   period = 60;
    const float sign = ((m_StateFrames / period) % 2 == 0) ? +1.f : -1.f;
    const float toYaw = std::atan2f((tp - self).x, (tp - self).z);

    // 接線方向
    float desired = Wrap(toYaw + sign * (D3DX_PI * 0.5f));

    // 半径誤差補正
    const float dist = DistXZ(self, tp);
    if (dist > m_KeepDistance * 1.2f) {
        desired = toYaw; // 外れすぎたら寄る
    }
    else if (dist < m_KeepDistance * 0.8f) {
        desired = Wrap(toYaw + D3DX_PI); // 近すぎたら離れる
    }

    const float next = SteerWithAvoidAABB(cur, desired, t.bodyTurnSpeed);
    //SafeAdvance(next, t.moveSpeed);

    TickAimTo(tp);
    TryAutoFire();
}
#endif

//退避
void CComPlayer::StepEvade()
{
    //ターゲットと反対方向に少し下がる
    std::shared_ptr<CBody> body = GetBody();
    if (!body) return;

    const D3DXVECTOR3 selfPos = body->GetPosition();
    auto Target = m_pTarget.lock();

    D3DXVECTOR3 targetPos = selfPos;
    if (Target)
    {
        targetPos = Target->GetPosition();
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
        const float step = m_Tuning.moveSpeed * 0.6f;
        const D3DXVECTOR3 pos = selfPos + away * step;

        //逃げ方向へ向きを寄せる
        float yaw = body->GetRotation().y;
        const float desired = std::atan2f(away.x, away.z);
        const float delta = Wrap(desired - yaw);
        yaw = Approach(yaw, yaw + delta, m_Tuning.turretTurnSpeed);

        body->SetPosition(pos);
        body->SetRotation(D3DXVECTOR3(0.0f, yaw, 0.0f));

        //位置とか回転反映の直後に同期
        SyncCannonToBody();
        //TryAutoFire();  //逃げながら発射

        body->CStaticMeshObject::Update();

        //砲塔の見た目を更新
        if (auto cannon = GetCannon())
        {
            cannon->CStaticMeshObject::Update();
        }
    }
}

#if 0
float CComPlayer::SteerWithAvoidAABB(float curYaw, float desiredYaw, float turnStep)
{
    auto body = GetBody();
    if (!body) return curYaw;

    const D3DXVECTOR3 selfPos = body->GetPosition();

    //障害物情報がなければ、純粋にdesiredYawへ寄せるだけ
    if (!m_pSimpleObstacles || m_pSimpleObstacles->empty())
    {
        const float d = Wrap(desiredYaw - curYaw);
        return Approach(curYaw, curYaw + d, turnStep);
    }

    // 3本の仮想レイを試す：正面、少し左、少し右
    const float angs[3] = { 0.f, +m_ProbeAngleRad, -m_ProbeAngleRad };

    float bestYaw = desiredYaw;
    float bestScore = -1e9f;

    for (float a : angs)
    {
        const float testYaw = desiredYaw + a;
        float hitD;
        const bool blocked = HasObstacleAheadSimple(
            selfPos, testYaw,
            m_ObstacleProbeDist,
            m_ObstacleProbeStep,
            hitD);

        // スコア設計：
        //  - 障害物がない方向を超優先
        //  - desired からあまり外れない方向を優先
        float score = 0.0f;
        if (!blocked)
        {
            score += 1000.0f; // ぶつからない方向
        }
        else
        {
            // ぶつかる場合も、近いよりは遠くでぶつかる方向をマシとする
            score -= (m_ObstacleProbeDist - hitD);
        }

        // desired からのズレはなるべく小さくしたい
        score -= std::fabs(Wrap(testYaw - desiredYaw)) * 10.0f;

        if (score > bestScore)
        {
            bestScore = score;
            bestYaw = testYaw;
        }
    }

    // 最終的に選ばれた bestYaw に向かうよう、curYaw を turnStep 分だけ近づける
    const float d = Wrap(bestYaw - curYaw);
    return Approach(curYaw, curYaw + d, turnStep);
}
#endif
//アイテム取得.アイテム認識
void CComPlayer::StepItemSeek()
{
#if 0
    auto item = std::shared_ptr<CItemBox>();
    auto body = GetBody();

    if (!item)
    {
        return;
    }

    if (item->IsActive())
    {
        for (int i = 0; i < ITEM_MAX; i++)
        {
            if (m_ItemGetRadius > m_BodyRadius)
            {
                item->GetItem();
                item->GetItemInfo();
            }
        }
    }
#endif
    
}

void CComPlayer::EvaluateTransitions(float dist2)
{
    auto Target = m_pTarget.lock();

    //2乗
    const float attackEnter2 = Sqr(std::max(m_KeepDistance * 1.05f, 3.f));
    const float attackExit2 = Sqr(std::max(m_KeepDistance * 1.25f, 5.f));
    const float evadeDist2 = Sqr(m_KeepDistance * 0.60f);
    const int   loseFrames = 120;

    switch (m_State) {
    case State::Seek:
        if (Target) ChangeState(State::Chase);
        break;
    case State::Chase:
        if (!Target) { ChangeState(State::Seek);  break; }
        if (dist2 <= evadeDist2) { ChangeState(State::Evade); break; }
        if (dist2 <= attackEnter2) { ChangeState(State::Attack); break; }
        break;
    case State::Attack:
        if (!Target) { ChangeState(State::Seek);  break; }
        if (dist2 < evadeDist2) { ChangeState(State::Evade); break; }
        if (dist2 > attackExit2) { ChangeState(State::Chase); break; }
        break;
    case State::Evade:
        if (!Target) { ChangeState(State::Seek);  break; }
        if (dist2 >= attackEnter2) { ChangeState(State::Chase); break; }
        else if (dist2 >= evadeDist2) { ChangeState(State::Attack); break; }
        if (m_LostSightFrames > loseFrames) { ChangeState(State::Seek); }
        break;
#if 0
    case State::ItemSeek:
        if (!Target) ChangeState(State::Seek);
        break;
    }
#endif
    }


#if 0
//COM弾発射処理
void CComPlayer::TryAutoFire()
{
    //auto manager = m_pShotManager.lock();
    //if (!manager || !m_pTarget) return;

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
        //manager->SetReload(BulletKinds::Mesh_2, muzzle, yaw);
        m_ShotState.m_ShotCD = m_ShotState.ShotCooldownFrames;
    }
}
#endif
}
//砲塔と車体の同期
void CComPlayer::SyncCannonToBody()
{
    auto body = GetBody();
    auto cannon = GetCannon();
    if (!body || !cannon) return;

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y += m_Tuning.cannonHeight;   //砲塔の高さオフセット
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

void CComPlayer::TickWander(float turnStep, float moveStep)
{
    //body取得
    auto body = GetBody();
    if (!body) return;

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
    //pos.x += dir.x * 0.02f;
    //pos.z += dir.z * 0.02f;

    //見た目に反映
    body->SetRotation({ 0.f,yawRed,0.f });
    body->SetPosition(pos);
    body->CStaticMeshObject::Update();

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

void CComPlayer::MakeItemTarget()
{
    if (!m_pItemBox)
    {
        m_pItemTarget.reset();
        return;
    }

    auto body = GetBody();
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
    auto body = GetBody();
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

    if (m_AvoidHolde > 0)
    {
        --m_AvoidHolde;
        return curYaw + turnStep * (float)m_AvoidSide;
    }
    //基本的には左
    if (blocke)
    {
        if (m_AvoidSide == 0)
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