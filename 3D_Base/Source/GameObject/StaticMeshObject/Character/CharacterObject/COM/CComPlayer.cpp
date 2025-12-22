#include "CComPlayer.h"

//-----ライブラリ-----
#include <cmath>
#include <algorithm>

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"       // 戦車：車体クラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"   // 戦車：砲塔クラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/CAdaptivePersonality/CAdaptivePersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/CAggressivePersonality/CAggressivePersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/CPersistentPersonality/CPersistentPersonality.h"

//-----サウンド-----
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス

#undef min

//静的レジストリ.複数をいっきに扱う
std::vector<CComPlayer*>& CComPlayer::Instances() {
    static std::vector<CComPlayer*> registry;
    return registry;
}

CComPlayer::CComPlayer()
    : m_KeepDistance(9.0f)   //0ならベタ詰め
    , m_AvoidRadius(10.0f)
    , m_AvoidWeight(2.0f)
    , m_Registered(false)
    , m_StateFrames(0)
    , m_SeekRadius(5.0f)
    , m_AttacRadius(50.0f)
    , m_FireConeDeg(10.0f)
    , m_ClosenessRadius(1.f)     //近くにしすぎない
    , m_EvadeDuration(60)
    , m_ComEnabled(true)
    , m_EvadeFrames(60)
    , m_IsTarget(false)   //最初はターゲットではない
    , m_pAllPlayer(nullptr)
    , m_LastSeenPos(D3DXVECTOR3(0, 0, 0))
    , m_State(State::Seek)
    , m_WanderAngle(0.f)
    , m_pBoxCollider(nullptr)
    , m_MapCenter(D3DXVECTOR3(0.0f, 0.0f, 0.0f))  // マップ中央
    , m_WanderRadius(15.0f)                        // 15m以内を徘徊
    , m_CenterPullStrength(0.3f)                   // 引き寄せ強度
    , m_pSimpleObstacles(nullptr)
    , m_LookAheadSkep(2.0f)
    //========================================
    // 障害物回避パラメータ
    //========================================
    , m_ProbeDist(8.0f)                         // 8メートル先まで探査
    , m_AvoidHoldFrames(0.0f)
    , m_AvoidSide(0)
    , m_AvoidMaxFrames(30.0f)                   // 30フレーム回避維持
    , m_BodyRadius(1.5f)                        // 自機半径
    , m_Respawn(false)
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

void CComPlayer::Create(int id)
{
    m_PlayerID = id;

    //それぞれのIDを渡して既存のBody,Cannonの設計に準拠する
    m_pBody = std::make_shared<CBody>(id);
    m_pCannon = std::make_shared<CCannon>(id);

    // プレイヤーの体力に最大体力を入れる
    m_Chara.m_Hp = m_Chara.m_MaxHp;
    // プレイヤーの無敵時間を初期化
    m_Chara.m_MutekiCnt = 0;
    m_Chara.m_MutekiTimer = 0.3;

    // プレイヤーのフラグを初期化
    m_Chara.m_Drawflag = true;
    m_Chara.m_Damage  = false;
    m_Chara.m_Death   = false;
    m_Chara.m_Kill    = false;
    m_Chara.m_Muteki = false;
    m_Chara.m_Respawn = false;
    // サウンドフラグ
	m_Chara.m_HitWall	= false;
	m_Chara.m_HitBox	= false;
	m_Chara.m_HitBlast	= false;

    //継承したものも初期化
    m_IsActive = true;
    m_IsAlive = true;

    //自分がまだ登録されていなければ、全体リストに登録する
    if (!m_Registered) {
        Instances().push_back(this);
        m_Registered = true;
    }

    m_pCannon->InitCannonRay();

    // ターゲットセレクターの初期化
    m_TargetSelector.Initialize(id);
    m_TargetSelector.SetForgetDistance(60.0f);
    m_TargetSelector.SetStickinessRatio(0.8f);
    m_TargetSelector.SetBlacklistDuration(120);
    m_TargetSelector.SetRetargetInterval(120);

    //射撃の初期化
    m_ComShot.Initialize(id);

    ShotConfig shotCfg;
    shotCfg.cooldownFrames = 120;
    shotCfg.fireAngleDeg = 12.0f;
    shotCfg.muzzleOffsetZ = 0.5f;
    shotCfg.cannonHeight = m_Tuning.cannonHeight;
    shotCfg.bulletSpeed = 0.8f;
    m_ComShot.SetConfig(shotCfg);

    m_pCannon->Init();

    switch (id)
    {
    case 1:
        //一番近い敵
        SetPersonalityType(PersonalityType::Aggressive);
        m_TargetSelector.SetForgetDistance(100.0f);     // 広い範囲で認識
        m_TargetSelector.SetStickinessRatio(1.0f);      // 粘着しない
        m_TargetSelector.SetRetargetInterval(30);       // 頻繁に再評価
        m_ComShot.SetShotCollDown(100);                 // ショットのクールダウン
        break;

    case 2:
        //プレイヤーより
        SetPersonalityType(PersonalityType::Adaptive);
        m_TargetSelector.SetForgetDistance(60.0f);      // 通常範囲
        m_TargetSelector.SetStickinessRatio(0.8f);      // 適度に粘着
        m_TargetSelector.SetRetargetInterval(90);       // 適度に再評価
        m_ComShot.SetShotCollDown(120);                  // プレイヤーと同じような条件
        break;

    case 3:
        //絶対にターゲットを変えない
        SetPersonalityType(PersonalityType::Persistent);
        m_TargetSelector.SetForgetDistance(1e9);    //どこまでも追う
        m_TargetSelector.SetStickinessRatio(0.0f);      //絶対に粘着
        m_TargetSelector.SetRetargetInterval(9999);     //再評価しない
        m_ComShot.SetShotCollDown(60);          
        break;

    default:
        //どれにも該当しない場合は人間型
        SetPersonalityType(PersonalityType::Adaptive);
        m_TargetSelector.SetForgetDistance(60.0f);
        m_TargetSelector.SetStickinessRatio(0.8f);
        m_TargetSelector.SetRetargetInterval(120);
        break;
    }

    // 共通設定
    m_TargetSelector.SetBlacklistDuration(120);
}

// コライダーの作成
void CComPlayer::CreateCollider()
{
    m_pBody->CreateBoxCollider(m_pBody->GetMinPos(), m_pBody->GetMaxPos());
    m_pCannon->CreateBoxCollider(m_pCannon->GetMinPos(), m_pCannon->GetMaxPos());
}

void CComPlayer::SetShotManager(std::shared_ptr<CShotManager> shot)
{
    m_pShotManager = shot;
    m_pCannon->SetShotManager(shot);
    m_ComShot.SetShotManager(shot);
}

//不正値を防ぐ
void CComPlayer::SanitizeParams()
{
    if (m_Tuning.moveSpeed <= 0.0f)             m_Tuning.moveSpeed = 0.08f;
    if (m_Tuning.bodyTurnSpeed <= 0.0f)         m_Tuning.bodyTurnSpeed = 0.03f;
    if (m_Tuning.cannonHeight <= 0.0f)          m_Tuning.cannonHeight = 0.3f;
    if (m_Tuning.turretTurnSpeed <= 0.0f)       m_Tuning.turretTurnSpeed = 0.03f;
    if (m_AvoidRadius < 0.0f)                   m_AvoidRadius = 0.0f;
    if (m_AvoidWeight < 0.0f)                   m_AvoidWeight = 0.0f;
    if (m_AttacRadius < 0.0f)                   m_AttacRadius = 10.0f;
    if (m_SeekRadius < 0.0f)                    m_SeekRadius = 5.0f;
    if (m_FireConeDeg < 0.0f)                   m_FireConeDeg = 10.0f;
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
        yaw = Util::Approach(yaw, yaw + Util::Wrap(desiredYaw - yaw), m_Tuning.turretTurnSpeed);
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
        yaw = Util::Approach(yaw, yaw + Util::Wrap(desiredYaw - yaw), m_Tuning.turretTurnSpeed);
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
        const D3DXVECTOR3 fwd = Util::ForwardFromYaw(yaw);
        pos += fwd * step;
    }

    // 反映
    body->SetRotation(D3DXVECTOR3(0.0f, yaw, 0.0f));
    body->SetPosition(pos);
    body->CStaticMeshObject::Update();
}

void CComPlayer::TickAimTo(const D3DXVECTOR3& targetPos)
{
    auto cannon = GetCannon();
    auto body = GetBody();
    if (!cannon) return;

    auto target = m_TargetSelector.GetCurrentTarget();
    if (!target || target->GetDeath())
    {
        return; //死亡ターゲットは照準しない
    }

    //ターゲット位置を平滑化
    m_SmoothedTargetPos.x += (targetPos.x - m_SmoothedTargetPos.x) * m_AimSmoothFactor;
    m_SmoothedTargetPos.y += (targetPos.y - m_SmoothedTargetPos.y) * m_AimSmoothFactor;
    m_SmoothedTargetPos.z += (targetPos.z - m_SmoothedTargetPos.z) * m_AimSmoothFactor;

    //性格クラスから砲塔パラメータを取得
    TurretParams turretParams;
    if (m_pPersonality)
    {
        turretParams = m_pPersonality->GetTurretParames();
    }

    // 砲口位置を取得
    D3DXVECTOR3 muzzle;
    float currentYaw;
    m_ComShot.ComputeMuzzle(muzzle, currentYaw, body, cannon);

    // 予測位置を計算
    D3DXVECTOR3 targetVel = m_TargetSelector.GetTargetVelocity();
    
    //予測精度を適用
    targetVel.x *= turretParams.predictionAccuracy;
    targetVel.z *= turretParams.predictionAccuracy;

    PredictedShot prediction = m_ComShot.PredictTargetPosition(
        muzzle, m_SmoothedTargetPos, targetVel);

    // 砲塔の基準位置
    D3DXVECTOR3 base = body ? body->GetPosition() : cannon->GetPosition();
    base.y += m_Tuning.cannonHeight;

    // 目標方
    const D3DXVECTOR3 toAim = prediction.aimPoint - base;
    const float desiredYaw = std::atan2f(toAim.x, toAim.z);

    //砲塔回転速度を性格で調整
    float turretSpeed = (m_Tuning.turretTurnSpeed + 0.02f) * turretParams.turretSpeedMultiplier;

    float cyaw = cannon->GetRotation().y;
    cyaw = Util::Approach(cyaw, cyaw + Util::Wrap(desiredYaw - cyaw), turretSpeed);

    cannon->SetPosition(base);
    cannon->SetRotation(D3DXVECTOR3(0.0f, cyaw, 0.0f));
    cannon->CStaticMeshObject::Update();
}


void CComPlayer::Update()
{
    // 死亡処理の更新
    Death();
    // 無敵処理の更新
    Muteki();

    SanitizeParams();
    SyncCannonToBody(); //常に砲塔と車体を同期

    auto tuning = GetTuning();

    auto body = GetBody();
    auto cannon = GetCannon();
    if (!body) {
        if (cannon) cannon->CStaticMeshObject::Update();
        return;
    }

    m_TargetSelector.Update();

    if (body)
    {
        TargetResult result = m_TargetSelector.SelectTarget(
            body->GetPosition(),
            m_pAllPlayer
        );
    }

    m_ComShot.TickCooldown();   //クールダウン更新

    //死亡したら処理スキップ
    if (m_Chara.m_Death == true)
    {
        return;
    }

    //COMの木箱乗り上げ対策
    D3DXVECTOR3 pos = body->GetPosition();
    body->SetPosition(pos.x, pos.y = 0, pos.z);

    //距離計算
    float dist2 = 1e18f;
    auto target = m_TargetSelector.GetCurrentTarget();
    if (target && body)
    {
        const D3DXVECTOR3 d = target->GetPosition() - body->GetPosition();
        dist2 = d.x * d.x + d.z * d.z;
    }

    //状態遷移はここだけで行う
    EvaluateTransitions(dist2);

    //実行
    switch (m_State) {
    case State::Seek:     StepSeek();     break;
    case State::Chase:    StepChase();    break;
    case State::Attack:   StepAttack();   break;
    case State::Evade:    StepEvade();    break;
    }
    ++m_StateFrames;
}


#if 1
bool CComPlayer::FollowPath(float turnStep, float moveStep)
{
    auto body = GetBody();
    if (!body) return false;

    // パスが空なら何もしない
    if (m_Path.empty()) return false;

    const D3DXVECTOR3 pos = body->GetPosition();

    // 次のウェイポイントに到達したら削除
    const float arriveThreshold = 2.0f;  // 到達判定距離
    while (!m_Path.empty())
    {
        D3DXVECTOR3 wp = m_Path.front();
        float dx = wp.x - pos.x;
        float dz = wp.z - pos.z;
        float dist2 = dx * dx + dz * dz;

        if (dist2 < arriveThreshold * arriveThreshold)
        {
            m_Path.pop_front();
        }
        else
        {
            break;
        }
    }

    if (m_Path.empty()) return false;

    // 次のウェイポイントへ向かう
    const D3DXVECTOR3 wp = m_Path.front();
    float curYaw = body->GetRotation().y;
    float desiredYaw = std::atan2f(wp.x - pos.x, wp.z - pos.z);

    const float nextYaw = SteerWithAvoidAABB(curYaw, desiredYaw, turnStep);
    SafeAdvance(nextYaw, moveStep);

    return true;
}

#endif

void CComPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
    if (m_Chara.m_Drawflag)
    {
        m_pBody->Draw(View, Proj, Light, Camera);
        m_pCannon->Draw(View, Proj, Light, Camera);
        m_pCannon->DrawRay(View, Proj);
    }
}

// 前方に当たり判定を設置する
bool CComPlayer::HasObstacleAheadWithBox(const CBoxCollider& selfBox,
    const D3DXVECTOR3& forward,
    float  probeDist,
    float  step,
    float& outHitDist)const
{
    if (!m_pBoxCollider || m_pBoxCollider->empty()) return false;

    CBoxCollider ghost = selfBox;
    const D3DXVECTOR3 base = selfBox.GetPosition();

    for (float d = step; d <= probeDist; d += step)
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

// 障害物を検知
bool CComPlayer::SenseObstacleAABB(const CBoxCollider& selfBox, float yaw, D3DXVECTOR3& outAvoid, float& nearest) const
{
    const float angs[3] = { 0.f, +m_ProbeAngleRad, -m_ProbeAngleRad };
    const float step = 0.5f;
    const float probeDist = m_ProbeDist;

    bool any = false;
    outAvoid = { 0, 0, 0 };
    nearest = 1e9f;

    for (float a : angs)
    {
        D3DXVECTOR3 dir = Util::ForwardFromYaw(yaw + a);
        dir.y = 0.f;
        float hitD;
        if (HasObstacleAheadWithBox(selfBox, dir, probeDist, step, hitD))
        {
            any = true;
            nearest = std::min(nearest, hitD);
            // 接線ベクトル
            D3DXVECTOR3 tang = { dir.z, 0.f, -dir.x };
            outAvoid.x += tang.x;
            outAvoid.z += tang.z;
        }
    }
    return any;
}

//レイとオブジェクト
bool CComPlayer::HitObjectRay()
{
    auto tuning = GetTuning();
    auto body = GetBody();
    auto target = m_TargetSelector.GetCurrentTarget();          //現在のターゲットを取得
    D3DXVECTOR3 muzzle = m_pCannon->GetMuzzlePosition();        //砲口の向きを取得

    D3DXVECTOR3 targetPos = target->GetPosition();
    D3DXVECTOR3 targetRot = target->GetRotation();
    D3DXVECTOR3 pos = body->GetPosition();
    float desired;

    desired = std::atan2f((pos - targetPos).x, (pos - targetPos).z);
    //射線が通らないような障害物があれば
    if (m_pCannon->IsPositionInSight(targetPos, 0.5f))
    {
        const float next = SteerWithAvoidAABB(targetRot.y, desired, tuning.bodyTurnSpeed);
        SafeAdvance(next, tuning.bodyTurnSpeed);
    }

    return false;
}

void CComPlayer::StepEvade()
{
    std::shared_ptr<CBody> body = GetBody();
    if (!body) return;

    const D3DXVECTOR3 selfPos = body->GetPosition();

    D3DXVECTOR3 targetPos = selfPos;

    auto target = m_TargetSelector.GetCurrentTarget();

    if (target)
    {
        targetPos = target->GetPosition();
    }

    //水平面でターゲットの反対方向に移動
    D3DXVECTOR3 away = selfPos - targetPos;
    away.y = 0.0f;

    const float len2 = away.x * away.x + away.z * away.z;
    if (len2 > 1e-6f)
    {
        // 正規化
        const float invLen = 1.0f / std::sqrtf(len2);
        away.x *= invLen;
        away.z *= invLen;

        // 逃げ方向
        float yaw = body->GetRotation().y;
        const float desired = std::atan2f(away.x, away.z);

        // 障害物回避を適用
        const float next = SteerWithAvoidAABB(yaw, desired, m_Tuning.turretTurnSpeed);
        SafeAdvance(next, m_Tuning.moveSpeed * 0.6f);

        TryAutoFire();

        // 砲塔の見た目を更新
        if (auto cannon = GetCannon())
        {
            cannon->CStaticMeshObject::Update();
        }
    }
}


void CComPlayer::StepSeek()
{
    auto body = GetBody();
    if (!body) return;

    auto cannon = GetCannon();
    if (!cannon) return;

    const auto tuning = GetTuning();

    const D3DXVECTOR3 center(0.f, 0.f, 0.f);
    D3DXVECTOR3 pos = body->GetPosition();
    const float curYaw = body->GetRotation().y;
    D3DXVECTOR3 d = center - pos;

    const float dist2 = d.x * d.x + d.z * d.z;
    float desiredYaw;

    auto target = m_TargetSelector.GetCurrentTarget();

    const float centerRadius = 10.0f;
    if (dist2 > centerRadius * centerRadius)
    {
        desiredYaw = std::atan2f(d.x, d.z);
    }
    else
    {
        //中心にいく.中心を優先しやすい
        TickWander();
        desiredYaw = curYaw + m_WanderAngle;
    }

    const float next = SteerWithAvoidAABB(curYaw, desiredYaw, tuning.bodyTurnSpeed);
    SafeAdvance(next, tuning.moveSpeed);

    //ターゲットがいる時点で砲塔はむく
    if (target)
    {
        TickAimTo(target->GetPosition());
    }

    //当たっているとレイ側が判定する半径
    float targetRadius = 3.f;
    float hitDistance;

    D3DXVECTOR3 collisonTarget;

    SyncCannonToBody();
}

#if 1
void CComPlayer::StepChase()
{
    auto body = GetBody();
    auto target = m_TargetSelector.GetCurrentTarget();
    if (!body || !target)
    {
        StepSeek();
        return;
    }

    const auto t = GetTuning();
    const D3DXVECTOR3 self = body->GetPosition();
    const D3DXVECTOR3 tp = target->GetPosition();
    const float cur = body->GetRotation().y;
    const float dist = Util::DistXZ(self, tp);

    // 障害物が間にあるときだけ経路探索を使う
    bool usePathfinding = false;
    float hitD;
    float toTargetYaw = std::atan2f((tp - self).x, (tp - self).z);

    if (HasObstacleAheadSimple(self, toTargetYaw, dist, 1.0f, hitD) == true)
    {
        usePathfinding = true;
    }

    if (usePathfinding && m_pPathfinder)
    {
        ++m_PathRecalcTimer;
        if (m_Path.empty() || m_PathRecalcTimer >= PATH_RECALC_INTERVAL)
        {
            RequestPath(tp);
            m_PathRecalcTimer = 0;
        }

        if (FollowPath(t.bodyTurnSpeed, t.moveSpeed))
        {
            TickAimTo(tp);
            TryAutoFire();
            return;
        }
    }

    // 通常移動
    D3DXVECTOR3 clusterCenter;
    int nearbyCount = CountNeardyEnemies(m_MultiEnemyRadius, clusterCenter);
    float hpRatio = static_cast<float>(m_Chara.m_Hp) / static_cast<float>(m_Chara.m_MaxHp);

    float desired;
    float speedMult = 1.0f;

    if (m_pPersonality)
    {
        BehaviorDecision decision = m_pPersonality->DecideChaseAction(
            self, tp, dist, nearbyCount, hpRatio);
        desired = decision.desiredYaw;
        speedMult = decision.moveSpeedMultiplier;
        m_KeepDistance = decision.keepDistance;
    }
    else
    {
        desired = std::atan2f((tp - self).x, (tp - self).z);
    }

    const float next = SteerWithAvoidAABB(cur, desired, t.bodyTurnSpeed);
    SafeAdvance(next, t.moveSpeed * speedMult);

    TickAimTo(tp);
    TryAutoFire();
}

#endif
void CComPlayer::StepAttack()
{
    auto body = GetBody();

    auto target = m_TargetSelector.GetCurrentTarget();
    if (!body || !target)
    {
        StepSeek();
        return;
    }

    const auto t = GetTuning();

    const D3DXVECTOR3 self = body->GetPosition();
    const D3DXVECTOR3 tp = target->GetPosition();
    const float cur = body->GetRotation().y;
    float hitD;

    // 複数敵チェック
    D3DXVECTOR3 clusterCenter;
    int nearbyCount = CountNeardyEnemies(m_MultiEnemyRadius, clusterCenter);

    float desired;

    if (nearbyCount >= m_MultiEnemyThreshold)
    {
        //囲まれているなら逃げつつ攻める
        desired = ComputeBlendedDirection(self, tp, clusterCenter,
            m_EscapeWeight * 0.8f,      //攻撃時は逃げを弱める
            m_ApproachWeight * 1.2f);   //攻めを強める
    }
    else
    {
        // 通常の周回攻撃
        const int   period = 60;
        const float sign = ((m_StateFrames / period) % 2 == 0) ? +1.f : -1.f;
        const float toYaw = std::atan2f((tp - self).x, (tp - self).z);

        // 接線方向
        desired = Util::Wrap(toYaw + sign * (D3DX_PI * 0.5f));

        // 半径誤差補正
        const float dist = Util::DistXZ(self, tp);
        if (dist > m_KeepDistance * 1.2f)
        {
            desired = toYaw; // 外れすぎたら寄る
        }
        else if (dist < m_KeepDistance * 0.8f)
        {
            desired = Util::Wrap(toYaw + D3DX_PI); // 近すぎたら離れる
        }
    }

    /*
    // 複数敵チェック
    D3DXVECTOR3 clusterCenter;
    int nearbyCount = CountNeardyEnemies(m_MultiEnemyRadius, clusterCenter);
    float hpRatio = static_cast<float>(m_Chara.m_Hp) / static_cast<float>(m_Chara.m_MaxHp);

    float desired;
    float speedMult = 1.0f;

    //性格に応じた複数敵対応
    if (nearbyCount >= m_MultiEnemyThreshold && m_pPersonality)
    {
        BehaviorDecision decision = m_pPersonality->DecideMultiEnemyAction(
            self, tp, clusterCenter, nearbyCount, hpRatio);

        desired = decision.desiredYaw;
        speedMult = decision.moveSpeedMultiplier;
        m_KeepDistance = decision.keepDistance;
    }
    else
    {
        // 通常の周回攻撃
        const int period = 60;
        const float sign = ((m_StateFrames / period) % 2 == 0) ? +1.f : -1.f;
        const float toYaw = std::atan2f((tp - self).x, (tp - self).z);
        desired = Util::Wrap(toYaw + sign * (D3DX_PI * 0.5f));

        const float dist = Util::DistXZ(self, tp);
        if (dist > m_KeepDistance * 1.2f) desired = toYaw;
        else if (dist < m_KeepDistance * 0.8f) desired = Util::Wrap(toYaw + D3DX_PI);
    }
    */

    if (target->GetDeath() == true)
    {
        m_TargetSelector.ClearTarget();
        return;
    }

    const float next = SteerWithAvoidAABB(cur, desired, t.bodyTurnSpeed);
    SafeAdvance(next, t.moveSpeed);

    TickAimTo(tp);

    if (HasObstacleAheadSimple(self, cur, m_ObstacleProbeDist, m_ObstacleProbeStep, hitD))
    {
        return;
    }

    TryAutoFire();
}


void CComPlayer::EvaluateTransitions(float dist2)
{
     float evadeMult = 0.60f;
    float attackEnterMult = 1.05f;

    if (m_pPersonality)
    {
        evadeMult = m_pPersonality->GetEvadeDistanceMultiplier();
        attackEnterMult = m_pPersonality->GetAttackEnterDistanceMultiplier();
    }

    const float attackEnter2 = Util::Sqr(std::max(m_KeepDistance * attackEnterMult, 3.f));
    const float attackExit2 = Util::Sqr(std::max(m_KeepDistance * 1.25f, 5.f));
    const float evadeDist2 = Util::Sqr(m_KeepDistance * evadeMult);

    const int   loseFrames = 120;

    const bool hasTarget = m_TargetSelector.HasTarget();
    const int lostFrames = m_TargetSelector.GetLostSightFrames();

    switch (m_State) {
    case State::Seek:
        if (hasTarget) ChangeState(State::Chase);
        break;
    case State::Chase:
        if (!hasTarget) { ChangeState(State::Seek);  break; }
        if (dist2 <= evadeDist2) { ChangeState(State::Evade); break; }
        if (dist2 <= attackEnter2) { ChangeState(State::Attack); break; }
        break;
    case State::Attack:
        if (!hasTarget) { ChangeState(State::Seek);  break; }
        if (dist2 < evadeDist2) { ChangeState(State::Evade); break; }
        if (dist2 > attackExit2) { ChangeState(State::Chase); break; }
        break;
    case State::Evade:
        if (!hasTarget) { ChangeState(State::Seek);  break; }
        if (dist2 >= attackEnter2) { ChangeState(State::Chase); break; }
        else if (dist2 >= evadeDist2) { ChangeState(State::Attack); break; }
        if (lostFrames > loseFrames) { ChangeState(State::Seek); }
        break;
    }
}

void CComPlayer::TryAutoFire()
{
    auto target = m_TargetSelector.GetCurrentTarget();
    if (!target) return;

    auto cannon = GetCannon();
    auto body = GetBody();
    if (!cannon || !body) return;

    if (!m_ComShot.IsReady()) return;

    // 砲塔レイでターゲット位置にヒット判定
    const float targetRadius = 2.f;  // ターゲットの当たり判定半径
    float hitDistance;

    if (cannon->RaycastToPosition(target->GetPosition(), targetRadius, hitDistance))
    {
        // 障害物チェック
        float obstacleHitD;
        if (HasObstacleAheadSimple(body->GetPosition(), cannon->GetRotation().y,
            hitDistance, m_ObstacleProbeStep, obstacleHitD))
        {
            // 障害物がターゲットより手前にある
            if (obstacleHitD < hitDistance)
            {
                return;
            }
        }

        //レイがターゲットに当たったら発射
        m_ComShot.TryFireOnRayHit(body, cannon);
    }
}

// 砲塔と車体の同期
void CComPlayer::SyncCannonToBody()
{
    auto body = GetBody();
    auto cannon = GetCannon();
    if (!body || !cannon) return;

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y += m_Tuning.cannonHeight;   // 砲塔の高さオフセット
    cannon->SetPosition(pos);         // 位置を同期
}

// ステータスを変更する
void CComPlayer::TransitionTo(State state)
{
    // ステータスが同じであればスキップ
    if (m_State == state) return;

    // ステータス更新
    m_State = state;
    m_StateFrames = 0;

    if (state == State::Evade)
    {
        m_EvadeFrames = m_EvadeDuration;
    }
}

#if 1
//危険ゾーン判定
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

float CComPlayer::SteerWithAvoidAABB(float curYaw, float desiredYaw, float turnStep)
{
    auto body = GetBody();
    if (!body) return curYaw;

    const D3DXVECTOR3 selfPos = body->GetPosition();

    //障害物情報がなければ、純粋にdesiredYawへ寄せるだけ
    if (!m_pSimpleObstacles || m_pSimpleObstacles->empty())
    {
        const float d = Util::Wrap(desiredYaw - curYaw);
        return Util::Approach(curYaw, curYaw + d, turnStep);
    }

    //3本の仮想レイを試す：正面、少し左、少し右
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
        //  - 障害物がない方向
        //  - desired からあまり外れない方向を優先
        float score = 0.0f;
        if (!blocked)
        {
            score += 1000.0f; // ぶつからない方向
        }
        else
        {
            //ぶつかる場合も、近いよりは遠くでぶつかる方向をマシとする
            score -= (m_ObstacleProbeDist - hitD);
        }

        // desired からのズレはなるべく小さくしたい
        score -= std::fabs(Util::Wrap(testYaw - desiredYaw)) * 10.0f;

        if (score > bestScore)
        {
            bestScore = score;
            bestYaw = testYaw;
        }
    }

    //最終的に選ばれたbestYawに向かうよう、curYawをturnStep分だけ近づける
    const float d = Util::Wrap(bestYaw - curYaw);
    return Util::Approach(curYaw, curYaw + d, turnStep);
}

//COMの正面方向に、一定距離以内に障害物があるか
bool CComPlayer::HasObstacleAheadSimple(const D3DXVECTOR3& selfPos, float yaw, float probeDist, float step, float& outHitDist) const
{

    auto Tuning = GetTuning();

    outHitDist = probeDist;
    if (!m_pSimpleObstacles || m_pSimpleObstacles->empty()) return false;   //障害物リストがない

    //自身の半径を取得
    const float selfR = m_ObstacleRadius;

    for (float d = step; d <= probeDist; d += step)
    {
        D3DXVECTOR3 p = selfPos + Util::ForwardFromYaw(yaw) * d;
        p.y = 0.0f;

        for (const auto& ob : *m_pSimpleObstacles)
        {
            D3DXVECTOR3 v = p - ob.pos;
            v.y = 0.0f;
            const float dist2 = v.x * v.x + v.z * v.z;
            const float r = selfR + ob.radius;   // 自分と障害物の安全距離
            if (dist2 <= r * r)
            {
                outHitDist = d;
                return true;
            }
        }
    }

    return false;
}


void CComPlayer::SafeAdvance(float nextYaw, float step)
{
    auto body = GetBody();
    if (!body) return;

    auto tuning = GetTuning();

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y = 0.0f;

    const float curYaw = body->GetRotation().y;

    //COM同士の分離
    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    ComputeSeparation(pos, sep, nearest);

    //試す角度オフセット
    const float offsets[] = { 0.f, +0.4f, -0.4f, +0.8f, -0.8f, +D3DX_PI * 0.5f, -D3DX_PI * 0.5f, D3DX_PI };

    float safeYaw = nextYaw;
    bool foundSafe = false;

    // 安全な方向を探す
    for (float offset : offsets)
    {
        float tryYaw = nextYaw + offset;

        D3DXVECTOR3 nextPos = pos + Util::ForwardFromYaw(tryYaw) * step;
        nextPos.x += sep.x * 0.02f;
        nextPos.z += sep.z * 0.02f;
        nextPos.y = 0.0f;

        if (!IsInDangerZone(nextPos))
        {
            safeYaw = tryYaw;
            foundSafe = true;
            break;
        }
    }

    // 滑らかに回転
    float targetYaw = foundSafe ? safeYaw : nextYaw;
    float smoothYaw = Util::Approach(curYaw, curYaw + Util::Wrap(targetYaw - curYaw), tuning.bodyTurnSpeed);

    // 回転を先に反映
    body->SetRotation({ 0.f, smoothYaw, 0.f });

    // 移動判定
    if (!foundSafe)
    {
        // 安全な方向が見つからないなら回転だけして停止
        body->CStaticMeshObject::Update();
        SyncCannonToBody();
        return;
    }

    // 安全な方向との角度差
    float angleDiffToSafe = std::fabs(Util::Wrap(safeYaw - smoothYaw));

    // 移動量の決定
    float actualStep = 0.0f;

    if (angleDiffToSafe < D3DX_PI * 0.15f)  
    {
        // ほぼ安全な方向を向いたなら通常速度
        actualStep = step;
    }
    else if (angleDiffToSafe < D3DX_PI * 0.3f)  
    {
        //まあまあ向いたなら半分の速度
        actualStep = step * 0.5f;
    }
    else if (angleDiffToSafe < D3DX_PI * 0.5f)  
    {
        //少し向いたなら微速前進
        actualStep = step * 1.f;    
    }
    else
    {
        //まだ全然向いていないなら回転に専念
        actualStep = 0.0f;
    }

    // 移動実行
    if (actualStep > 0.0f)
    {
        // smoothYaw 方向に進む
        D3DXVECTOR3 nextPos = pos + Util::ForwardFromYaw(smoothYaw) * actualStep;
        nextPos.x += sep.x * 0.02f;
        nextPos.z += sep.z * 0.02f;
        nextPos.y = 0.0f;

        // 移動先が安全かチェック
        if (!IsInDangerZone(nextPos))
        {
            if (m_PlayerID == 1)
            {
                //移動SEの再生.
                CSoundManager::PlayLoop(CSoundManager::SE_Move2);
            }
            if (m_PlayerID == 2)
            {
                //移動SEの再生.
                CSoundManager::PlayLoop(CSoundManager::SE_Move3);
            }
            if (m_PlayerID == 3)
            {
                //移動SEの再生.
                CSoundManager::PlayLoop(CSoundManager::SE_Move4);
            }

            body->SetPosition(nextPos);
        }
        else
        {
            if (m_PlayerID == 1)
            {
                // 移動SE停止
                CSoundManager::Stop(CSoundManager::SE_Move2);
            }
            if (m_PlayerID == 2)
            {
                CSoundManager::Stop(CSoundManager::SE_Move3);
            }
            if (m_PlayerID == 3)
            {
                CSoundManager::Stop(CSoundManager::SE_Move4);
            }
        }
        // 危険なら移動しない
    }

    body->CStaticMeshObject::Update();
    SyncCannonToBody();
}

void CComPlayer::TickWander()
{
    const float WanderDelta = 0.10f;
    const float WanderClamp = 0.6f;

    // たまにだけ方向を揺らす
    if ((std::rand() & 31) == 0) // 1/32フレームぐらい
    {
        const float sign = (std::rand() & 1) ? +1.f : -1.f;
        m_WanderAngle += sign * WanderDelta;
        if (m_WanderAngle > WanderClamp) m_WanderAngle = WanderClamp;
        if (m_WanderAngle < -WanderClamp) m_WanderAngle = -WanderClamp;
    }
}

#endif


//指定半径内の敵の数をカウントし、重心を返す
int CComPlayer::CountNeardyEnemies(float radius, D3DXVECTOR3& outClusterCenter) const
{
    auto body = GetBody();
    if (!body || !m_pAllPlayer)
    {
        outClusterCenter = D3DXVECTOR3(0, 0, 0);
        return 0;
    }

    const D3DXVECTOR3 self = body->GetPosition();
    const float radiusSq = radius * radius;

    int count = 0;
    D3DXVECTOR3 sum(0, 0, 0);

    for (const auto& p : *m_pAllPlayer)
    {
        if (!p) continue;
        if (p.get() == this) continue;  //自分は除外
        if (p->GetDeath()) continue;    //死亡も除外

        const D3DXVECTOR3 enemyPos = p->GetPosition();
        const float dx = enemyPos.x - self.x;
        const float dz = enemyPos.z - self.z;
        const float distSq = dx * dx + dz * dz;

        if (distSq <= radiusSq)
        {
            sum.x += enemyPos.x;
            sum.z += enemyPos.z;
            ++count;
        }
    }

    if (count > 0)
    {
        outClusterCenter.x = sum.x / count;
        outClusterCenter.y = 0.0f;
        outClusterCenter.z = sum.z / count;
    }
    else
    {
        outClusterCenter = self;
    }

    return count;
}

// 逃げと攻めをブレンドした方向を計算
float CComPlayer::ComputeBlendedDirection(
    const D3DXVECTOR3& self,
    const D3DXVECTOR3& targetPos,
    const D3DXVECTOR3& clusterCenter,
    float escapeWeight,
    float approachWeight) const
{
    // 攻め方向：ターゲットへ向かう
    D3DXVECTOR3 toTarget = targetPos - self;
    toTarget.y = 0.0f;
    float toTargetLen = std::sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
    if (toTargetLen > 1e-6f)
    {
        toTarget.x /= toTargetLen;
        toTarget.z /= toTargetLen;
    }

    // 逃げ方向：敵の群れの中心から離れる
    D3DXVECTOR3 escape = self - clusterCenter;
    escape.y = 0.0f;
    float escapeLen = std::sqrtf(escape.x * escape.x + escape.z * escape.z);
    if (escapeLen > 1e-6f)
    {
        escape.x /= escapeLen;
        escape.z /= escapeLen;
    }
    else
    {
        // 群れの中心と自分が同じ位置なら、ランダムに逃げる
        escape.x = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
        escape.z = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
    }

    // ブレンド
    D3DXVECTOR3 blended;
    blended.x = toTarget.x * approachWeight + escape.x * escapeWeight;
    blended.z = toTarget.z * approachWeight + escape.z * escapeWeight;
    blended.y = 0.0f;

    // 正規化
    float blendLen = std::sqrtf(blended.x * blended.x + blended.z * blended.z);
    if (blendLen > 1e-6f)
    {
        blended.x /= blendLen;
        blended.z /= blendLen;
    }

    m_ComShot.IsReady();

    // Yaw に変換
    return std::atan2f(blended.x, blended.z);

}


std::shared_ptr<CCharacterObjectBase> CComPlayer::GetRayHitCharacter() const
{
    auto cannon = GetCannon();
    if (!cannon || !m_pAllPlayer) return nullptr;

    CannonHitRay bestHit;
    bestHit.bHit = false;
    bestHit.Distance = 1e9f;
    std::shared_ptr<CCharacterObjectBase> hitTarget = nullptr;

    for (const auto& player : *m_pAllPlayer)
    {
        if (!player) continue;
        if (player.get() == this) continue;  // 自分は除外
        if (player->GetDeath()) continue;     // 死亡者は除外

        CStaticMeshObject* mesh = dynamic_cast<CStaticMeshObject*>(player.get());
        if (!mesh) continue;

        CannonHitRay tempHit;
        if (cannon->RaycastTo(mesh, tempHit))
        {
            // より近いヒットを優先
            if (tempHit.Distance < bestHit.Distance)
            {
                bestHit = tempHit;
                hitTarget = player;
            }
        }
    }

    return hitTarget;
}

bool CComPlayer::RequestPath(const D3DXVECTOR3& goal)
{
    if (!m_pPathfinder) return false;

    auto body = GetBody();
    if (!body) return false;

    m_Path.clear();
    return m_pPathfinder->FindPath(body->GetPosition(), goal, m_Path);
}

void CComPlayer::SetPosition(const D3DXVECTOR3& pos)
{
    if (m_pBody)
    {
        m_pBody->SetPosition(pos);
    }
    if (m_pCannon)
    {
        //砲塔は高さオフセットを適用する
        D3DXVECTOR3 cannonPos = pos;
        cannonPos.y += m_Tuning.cannonHeight;
        m_pCannon->SetPosition(cannonPos);
    }
}

void CComPlayer::FindNearestTarget()
{
    //m_pAllPlayer から人間プレイヤーを探す
    if (m_pAllPlayer) {
        for (auto& p : *m_pAllPlayer) {
            // 距離比較してbestを更新
        }
    }

    //Instances() から他のCOMも探す
    for (CComPlayer* other : Instances()) {
        // 距離比較してbestを更新
    }
}



void CComPlayer::SetPersonality(std::unique_ptr<IComPersonality> personality)
{
    m_pPersonality = std::move(personality);
}

void CComPlayer::SetPersonalityType(PersonalityType type)
{
    switch (type)
    {
    case PersonalityType::Aggressive:
        m_pPersonality = std::make_unique<CAggressivePersonality>();
        break;
    case PersonalityType::Adaptive:
        m_pPersonality = std::make_unique<CAdaptivePersonality>();
        break;
    case PersonalityType::Persistent:
        m_pPersonality = std::make_unique<CPersistentPersonality>();
        break;
    }
}

PersonalityType CComPlayer::GetPersonalityType() const
{
    if (m_pPersonality)
    {
        return m_pPersonality->GetType();
    }
    return PersonalityType::Adaptive;  // デフォルト
}