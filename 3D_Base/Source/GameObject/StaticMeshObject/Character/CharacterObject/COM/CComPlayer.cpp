#include "CComPlayer.h"

//-----ライブラリ-----
#include <cmath>
#include <algorithm>

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/CAdaptivePersonality/CAdaptivePersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/CAggressivePersonality/CAggressivePersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/CPersistentPersonality/CPersistentPersonality.h"

//-----サウンド-----
#include "Assets//Sound//CSoundManager.h"

#undef min

// 静的レジストリ（複数をいっきに扱う）
std::vector<CComPlayer*>& CComPlayer::Instances() {
    static std::vector<CComPlayer*> registry;
    return registry;
}

CComPlayer::CComPlayer()
    : m_KeepDistance(9.0f)
    , m_AvoidRadius(10.0f)
    , m_AvoidWeight(2.0f)
    , m_Registered(false)
    , m_StateFrames(0)
    , m_SeekRadius(5.0f)
    , m_AttacRadius(50.0f)
    , m_FireConeDeg(10.0f)
    , m_ClosenessRadius(1.f)
    , m_EvadeDuration(60)
    , m_ComEnabled(true)
    , m_EvadeFrames(60)
    , m_IsTarget(false)
    , m_pAllPlayer(nullptr)
    , m_LastSeenPos(D3DXVECTOR3(0, 0, 0))
    , m_State(State::Seek)
    , m_WanderAngle(0.f)
    , m_pBoxCollider(nullptr)
    , m_MapCenter(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
    , m_WanderRadius(15.0f)
    , m_CenterPullStrength(0.3f)
    , m_LookAheadSkep(2.0f)
    , m_Respawn(false)
{
}

CComPlayer::~CComPlayer()
{
    if (m_Registered) {
        auto& comList = Instances();
        comList.erase(std::remove(comList.begin(), comList.end(), this), comList.end());
        m_Registered = false;
    }
}

void CComPlayer::Create(int id)
{
    m_PlayerID = id;

    m_pBody = std::make_shared<CBody>(id);
    m_pCannon = std::make_shared<CCannon>(id);

    // ===== 障害物回避クラスの初期化 =====
    m_ObstacleAvoidance.SetSelfRadius(1.5f);
    m_ObstacleAvoidance.SetProbeDist(5.0f);
    m_ObstacleAvoidance.SetProbeStep(0.5f);
    m_ObstacleAvoidance.SetProbeAngle(D3DX_PI / 4.0f);

    // プレイヤーの体力に最大体力を入れる
    m_Chara.m_Hp = m_Chara.m_MaxHp;
    m_Chara.m_MutekiCnt = 0;
    m_Chara.m_MutekiTimer = 0.3;

    m_Chara.m_Drawflag = true;
    m_Chara.m_Damage = false;
    m_Chara.m_Death = false;
    m_Chara.m_Kill = false;
    m_Chara.m_Muteki = false;
    m_Chara.m_Respawn = false;
    m_Chara.m_HitWall = false;
    m_Chara.m_HitBox = false;
    m_Chara.m_HitBlast = false;

    m_IsActive = true;
    m_IsAlive = true;

    if (!m_Registered) {
        Instances().push_back(this);
        m_Registered = true;
    }

    m_pCannon->InitCannonRay();

    m_TargetSelector.Initialize(id);
    m_TargetSelector.SetForgetDistance(60.0f);
    m_TargetSelector.SetStickinessRatio(0.8f);
    m_TargetSelector.SetBlacklistDuration(120);
    m_TargetSelector.SetRetargetInterval(120);

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
        SetPersonalityType(PersonalityType::Aggressive);
        m_TargetSelector.SetForgetDistance(100.0f);
        m_TargetSelector.SetStickinessRatio(1.0f);
        m_TargetSelector.SetRetargetInterval(30);
        m_ComShot.SetShotCollDown(100);
        break;

    case 2:
        SetPersonalityType(PersonalityType::Adaptive);
        m_TargetSelector.SetForgetDistance(60.0f);
        m_TargetSelector.SetStickinessRatio(0.8f);
        m_TargetSelector.SetRetargetInterval(90);
        m_ComShot.SetShotCollDown(120);
        break;

    case 3:
        SetPersonalityType(PersonalityType::Persistent);
        m_TargetSelector.SetForgetDistance(1e9);
        m_TargetSelector.SetStickinessRatio(0.0f);
        m_TargetSelector.SetRetargetInterval(9999);
        m_ComShot.SetShotCollDown(60);
        break;

    default:
        SetPersonalityType(PersonalityType::Adaptive);
        m_TargetSelector.SetForgetDistance(60.0f);
        m_TargetSelector.SetStickinessRatio(0.8f);
        m_TargetSelector.SetRetargetInterval(120);
        break;
    }

    m_TargetSelector.SetBlacklistDuration(120);
}

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

void CComPlayer::SanitizeParams()
{
    if (m_Tuning.moveSpeed <= 0.0f)         m_Tuning.moveSpeed = 0.08f;
    if (m_Tuning.bodyTurnSpeed <= 0.0f)     m_Tuning.bodyTurnSpeed = 0.03f;
    if (m_Tuning.cannonHeight <= 0.0f)      m_Tuning.cannonHeight = 0.3f;
    if (m_Tuning.turretTurnSpeed <= 0.0f)   m_Tuning.turretTurnSpeed = 0.03f;
    if (m_AvoidRadius < 0.0f)               m_AvoidRadius = 0.0f;
    if (m_AvoidWeight < 0.0f)               m_AvoidWeight = 0.0f;
    if (m_AttacRadius < 0.0f)               m_AttacRadius = 10.0f;
    if (m_SeekRadius < 0.0f)                m_SeekRadius = 5.0f;
    if (m_FireConeDeg < 0.0f)               m_FireConeDeg = 10.0f;
}

void CComPlayer::ComputeSeparation(const D3DXVECTOR3& selfPos,
    D3DXVECTOR3& outSep, float& outNearest) const
{
    outSep = D3DXVECTOR3(0, 0, 0);
    outNearest = 1e9f;

    if (m_AvoidRadius <= 0.0f) return;

    const float avoidRadius = m_AvoidRadius;
    const float avoidRadiusSq = avoidRadius * avoidRadius;

    for (CComPlayer* other : Instances()) {
        if (other == this) continue;
        std::shared_ptr<CBody> ob = other ? other->GetBody() : nullptr;
        if (!ob) continue;

        D3DXVECTOR3 offset = selfPos - ob->GetPosition();
        offset.y = 0.0f;

        const float distSq = offset.x * offset.x + offset.z * offset.z;
        if (distSq <= 1e-6f) {
            outSep.x += 0.1f;
            continue;
        }

        outNearest = std::min(outNearest, std::sqrtf(distSq));

        if (distSq < avoidRadiusSq) {
            const float invDistSq = 1.0f / distSq;
            outSep.x += offset.x * invDistSq;
            outSep.z += offset.z * invDistSq;
        }
    }
}

void CComPlayer::ChangeState(State state)
{
    m_State = state;
    m_StateFrames = 0;
}

void CComPlayer::TickChaseTo(const D3DXVECTOR3& targetPos)
{
    std::shared_ptr<CBody> body = GetBody();
    if (!body) return;

    D3DXVECTOR3 pos = body->GetPosition();
    float yaw = body->GetRotation().y;

    D3DXVECTOR3 target = targetPos - pos;
    target.y = 0.0f;
    const float d2 = target.x * target.x + target.z * target.z;

    if (d2 > 1e-6f) {
        const float desiredYaw = std::atan2f(target.x, target.z);
        yaw = Util::Approach(yaw, yaw + Util::Wrap(desiredYaw - yaw), m_Tuning.turretTurnSpeed);
    }

    D3DXVECTOR3 chaseDir(0, 0, 0);
    if (d2 > 1e-6f) {
        const float inv = 1.0f / std::sqrtf(d2);
        chaseDir.x = target.x * inv;
        chaseDir.z = target.z * inv;
    }

    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    ComputeSeparation(pos, sep, nearest);

    D3DXVECTOR3 desire = chaseDir;
    if (m_AvoidWeight > 0.0f && (sep.x != 0.0f || sep.z != 0.0f)) {
        const float sepLen = std::sqrt(sep.x * sep.x + sep.z * sep.z);
        if (sepLen > 1e-6f) {
            sep.x /= sepLen; sep.z /= sepLen;
            desire.x += sep.x * m_AvoidWeight;
            desire.z += sep.z * m_AvoidWeight;
        }
    }

    float desiredYaw = yaw;
    const float desLen2 = desire.x * desire.x + desire.z * desire.z;
    if (desLen2 > 1e-8f) {
        desiredYaw = std::atan2f(desire.x, desire.z);
        yaw = Util::Approach(yaw, yaw + Util::Wrap(desiredYaw - yaw), m_Tuning.turretTurnSpeed);
    }

    float step = m_Tuning.moveSpeed;
    if (d2 > 0.0f) {
        const float dist = std::sqrtf(d2);
        if (m_KeepDistance > 0.0f) {
            const float remain = dist - m_KeepDistance;
            if (remain <= 0.0f) {
                step = 0.0f;
            }
            else if (step > remain) {
                step = remain;
            }
            else {
                if (step > dist) step = dist;
            }
        }
    }

    if (nearest < 1e9f && m_AvoidRadius > 0.0f) {
        float scale = nearest / m_AvoidRadius;
        if (scale < 0.0f) scale = 0.0f;
        if (scale > 1.0f) scale = 1.0f;
        step *= scale;
    }

    if (step > 0.0f) {
        const D3DXVECTOR3 fwd = Util::ForwardFromYaw(yaw);
        pos += fwd * step;
    }

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
        return;
    }

    m_SmoothedTargetPos.x += (targetPos.x - m_SmoothedTargetPos.x) * m_AimSmoothFactor;
    m_SmoothedTargetPos.y += (targetPos.y - m_SmoothedTargetPos.y) * m_AimSmoothFactor;
    m_SmoothedTargetPos.z += (targetPos.z - m_SmoothedTargetPos.z) * m_AimSmoothFactor;

    TurretParams turretParams;
    if (m_pPersonality)
    {
        turretParams = m_pPersonality->GetTurretParames();
    }

    D3DXVECTOR3 muzzle;
    float currentYaw;
    m_ComShot.ComputeMuzzle(muzzle, currentYaw, body, cannon);

    D3DXVECTOR3 targetVel = m_TargetSelector.GetTargetVelocity();

    targetVel.x *= turretParams.predictionAccuracy;
    targetVel.z *= turretParams.predictionAccuracy;

    PredictedShot prediction = m_ComShot.PredictTargetPosition(
        muzzle, m_SmoothedTargetPos, targetVel);

    D3DXVECTOR3 base = body ? body->GetPosition() : cannon->GetPosition();
    base.y += m_Tuning.cannonHeight;

    const D3DXVECTOR3 toAim = prediction.aimPoint - base;
    const float desiredYaw = std::atan2f(toAim.x, toAim.z);

    float turretSpeed = (m_Tuning.turretTurnSpeed + 0.02f) * turretParams.turretSpeedMultiplier;

    float cyaw = cannon->GetRotation().y;
    cyaw = Util::Approach(cyaw, cyaw + Util::Wrap(desiredYaw - cyaw), turretSpeed);

    cannon->SetPosition(base);
    cannon->SetRotation(D3DXVECTOR3(0.0f, cyaw, 0.0f));
    cannon->CStaticMeshObject::Update();
}

void CComPlayer::Update()
{

    Death();

    if (m_Chara.m_Death)
    {
        return;
    }

    Muteki();

    SanitizeParams();
    SyncCannonToBody();

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

    m_ComShot.TickCooldown();

    if (m_Chara.m_Death == true)
    {
        return;
    }

    D3DXVECTOR3 pos = body->GetPosition();
    body->SetPosition(pos.x, pos.y = 0, pos.z);

    float dist2 = 1e18f;
    auto target = m_TargetSelector.GetCurrentTarget();
    if (target && body)
    {
        const D3DXVECTOR3 d = target->GetPosition() - body->GetPosition();
        dist2 = d.x * d.x + d.z * d.z;
    }

    EvaluateTransitions(dist2);

    switch (m_State) {
    case State::Seek:     StepSeek();     break;
    case State::Chase:    StepChase();    break;
    case State::Attack:   StepAttack();   break;
    case State::Evade:    StepEvade();    break;
    }
    ++m_StateFrames;
}

bool CComPlayer::FollowPath(float turnStep, float moveStep)
{
    auto body = GetBody();
    if (!body) return false;

    if (m_Path.empty()) return false;

    const D3DXVECTOR3 pos = body->GetPosition();

    const float arriveThreshold = 2.0f;
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

    const D3DXVECTOR3 wp = m_Path.front();
    float curYaw = body->GetRotation().y;
    float desiredYaw = std::atan2f(wp.x - pos.x, wp.z - pos.z);

    const float nextYaw = m_ObstacleAvoidance.ComputeAvoidYaw(pos, curYaw, desiredYaw, turnStep);
    SafeAdvance(nextYaw, moveStep);

    return true;
}

void CComPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
    if (m_Chara.m_Drawflag)
    {
        m_pBody->Draw(View, Proj, Light, Camera);
        m_pCannon->Draw(View, Proj, Light, Camera);
        m_pCannon->DrawRay(View, Proj);
    }
}

bool CComPlayer::HitObjectRay()
{
    auto tuning = GetTuning();
    auto body = GetBody();
    auto target = m_TargetSelector.GetCurrentTarget();
    D3DXVECTOR3 muzzle = m_pCannon->GetMuzzlePosition();

    D3DXVECTOR3 targetPos = target->GetPosition();
    D3DXVECTOR3 targetRot = target->GetRotation();
    D3DXVECTOR3 pos = body->GetPosition();
    float desired;

    desired = std::atan2f((pos - targetPos).x, (pos - targetPos).z);

    if (m_pCannon->IsPositionInSight(targetPos, 0.5f))
    {
        const float next = m_ObstacleAvoidance.ComputeAvoidYaw(pos, targetRot.y, desired, tuning.bodyTurnSpeed);
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

    D3DXVECTOR3 away = selfPos - targetPos;
    away.y = 0.0f;

    const float len2 = away.x * away.x + away.z * away.z;
    if (len2 > 1e-6f)
    {
        const float invLen = 1.0f / std::sqrtf(len2);
        away.x *= invLen;
        away.z *= invLen;

        float yaw = body->GetRotation().y;
        const float desired = std::atan2f(away.x, away.z);

        const float next = m_ObstacleAvoidance.ComputeAvoidYaw(selfPos, yaw, desired, m_Tuning.turretTurnSpeed);
        SafeAdvance(next, m_Tuning.moveSpeed * 0.6f);

        TryAutoFire();

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
        TickWander();
        desiredYaw = curYaw + m_WanderAngle;
    }

    const float next = m_ObstacleAvoidance.ComputeAvoidYaw(pos, curYaw, desiredYaw, tuning.bodyTurnSpeed);
    SafeAdvance(next, tuning.moveSpeed);

    if (target)
    {
        TickAimTo(target->GetPosition());
    }

    float targetRadius = 3.f;
    float hitDistance;

    D3DXVECTOR3 collisonTarget;

    SyncCannonToBody();
}

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

    bool usePathfinding = false;
    float hitD;
    float toTargetYaw = std::atan2f((tp - self).x, (tp - self).z);

    if (m_ObstacleAvoidance.HasObstacleAhead(self, toTargetYaw, dist, 1.0f, hitD) == true)
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

    const float next = m_ObstacleAvoidance.ComputeAvoidYaw(self, cur, desired, t.bodyTurnSpeed);
    SafeAdvance(next, t.moveSpeed * speedMult);

    TickAimTo(tp);
    TryAutoFire();
}

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

    D3DXVECTOR3 clusterCenter;
    int nearbyCount = CountNeardyEnemies(m_MultiEnemyRadius, clusterCenter);

    float desired;

    if (nearbyCount >= m_MultiEnemyThreshold)
    {
        desired = ComputeBlendedDirection(self, tp, clusterCenter,
            m_EscapeWeight * 0.8f,
            m_ApproachWeight * 1.2f);
    }
    else
    {
        const int   period = 60;
        const float sign = ((m_StateFrames / period) % 2 == 0) ? +1.f : -1.f;
        const float toYaw = std::atan2f((tp - self).x, (tp - self).z);

        desired = Util::Wrap(toYaw + sign * (D3DX_PI * 0.5f));

        const float dist = Util::DistXZ(self, tp);
        if (dist > m_KeepDistance * 1.2f)
        {
            desired = toYaw;
        }
        else if (dist < m_KeepDistance * 0.8f)
        {
            desired = Util::Wrap(toYaw + D3DX_PI);
        }
    }

    if (target->GetDeath() == true)
    {
        m_TargetSelector.ClearTarget();
        return;
    }

    const float next = m_ObstacleAvoidance.ComputeAvoidYaw(self, cur, desired, t.bodyTurnSpeed);
    SafeAdvance(next, t.moveSpeed);

    TickAimTo(tp);

    float probeDist = 5.0f;
    float probeStep = 0.5f;
    if (m_ObstacleAvoidance.HasObstacleAhead(self, cur, probeDist, probeStep, hitD))
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

    const float targetRadius = 2.f;
    float hitDistance;

    if (cannon->RaycastToPosition(target->GetPosition(), targetRadius, hitDistance))
    {
        float obstacleHitD;
        float probeStep = 0.5f;
        if (m_ObstacleAvoidance.HasObstacleAhead(body->GetPosition(), cannon->GetRotation().y,
            hitDistance, probeStep, obstacleHitD))
        {
            if (obstacleHitD < hitDistance)
            {
                return;
            }
        }

        m_ComShot.TryFireOnRayHit(body, cannon);
    }
}

void CComPlayer::SyncCannonToBody()
{
    auto body = GetBody();
    auto cannon = GetCannon();
    if (!body || !cannon) return;

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y += m_Tuning.cannonHeight;
    cannon->SetPosition(pos);
}

void CComPlayer::TransitionTo(State state)
{
    if (m_State == state) return;

    m_State = state;
    m_StateFrames = 0;

    if (state == State::Evade)
    {
        m_EvadeFrames = m_EvadeDuration;
    }
}

void CComPlayer::SafeAdvance(float nextYaw, float step)
{
    auto body = GetBody();
    if (!body) return;

    auto tuning = GetTuning();

    D3DXVECTOR3 pos = body->GetPosition();
    pos.y = 0.0f;

    const float curYaw = body->GetRotation().y;

    D3DXVECTOR3 sep(0, 0, 0);
    float nearest = 1e9f;
    ComputeSeparation(pos, sep, nearest);

    const float offsets[] = { 0.f, +0.4f, -0.4f, +0.8f, -0.8f, +D3DX_PI * 0.5f, -D3DX_PI * 0.5f, D3DX_PI };

    float safeYaw = nextYaw;
    bool foundSafe = false;

    for (float offset : offsets)
    {
        float tryYaw = nextYaw + offset;

        D3DXVECTOR3 nextPos = pos + Util::ForwardFromYaw(tryYaw) * step;
        nextPos.x += sep.x * 0.02f;
        nextPos.z += sep.z * 0.02f;
        nextPos.y = 0.0f;

        if (!m_ObstacleAvoidance.IsInDangerZone(nextPos))
        {
            safeYaw = tryYaw;
            foundSafe = true;
            break;
        }
    }

    float targetYaw = foundSafe ? safeYaw : nextYaw;
    float smoothYaw = Util::Approach(curYaw, curYaw + Util::Wrap(targetYaw - curYaw), tuning.bodyTurnSpeed);

    body->SetRotation({ 0.f, smoothYaw, 0.f });

    if (!foundSafe)
    {
        body->CStaticMeshObject::Update();
        SyncCannonToBody();
        return;
    }

    float angleDiffToSafe = std::fabs(Util::Wrap(safeYaw - smoothYaw));

    float actualStep = 0.0f;

    if (angleDiffToSafe < D3DX_PI * 0.15f)
    {
        actualStep = step;
    }
    else if (angleDiffToSafe < D3DX_PI * 0.3f)
    {
        actualStep = step * 0.5f;
    }
    else if (angleDiffToSafe < D3DX_PI * 0.5f)
    {
        actualStep = step * 1.f;
    }
    else
    {
        actualStep = 0.0f;
    }

    if (actualStep > 0.0f)
    {
        D3DXVECTOR3 nextPos = pos + Util::ForwardFromYaw(smoothYaw) * actualStep;
        nextPos.x += sep.x * 0.02f;
        nextPos.z += sep.z * 0.02f;
        nextPos.y = 0.0f;

        if (!m_ObstacleAvoidance.IsInDangerZone(nextPos))
        {
            body->SetPosition(nextPos);
        }
    }

    body->CStaticMeshObject::Update();
    SyncCannonToBody();
}

void CComPlayer::TickWander()
{
    const float WanderDelta = 0.10f;
    const float WanderClamp = 0.6f;

    if ((std::rand() & 31) == 0)
    {
        const float sign = (std::rand() & 1) ? +1.f : -1.f;
        m_WanderAngle += sign * WanderDelta;
        if (m_WanderAngle > WanderClamp) m_WanderAngle = WanderClamp;
        if (m_WanderAngle < -WanderClamp) m_WanderAngle = -WanderClamp;
    }
}

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
        if (p.get() == this) continue;
        if (p->GetDeath()) continue;

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

float CComPlayer::ComputeBlendedDirection(
    const D3DXVECTOR3& self,
    const D3DXVECTOR3& targetPos,
    const D3DXVECTOR3& clusterCenter,
    float escapeWeight,
    float approachWeight) const
{
    D3DXVECTOR3 toTarget = targetPos - self;
    toTarget.y = 0.0f;
    float toTargetLen = std::sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
    if (toTargetLen > 1e-6f)
    {
        toTarget.x /= toTargetLen;
        toTarget.z /= toTargetLen;
    }

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
        escape.x = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
        escape.z = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
    }

    D3DXVECTOR3 blended;
    blended.x = toTarget.x * approachWeight + escape.x * escapeWeight;
    blended.z = toTarget.z * approachWeight + escape.z * escapeWeight;
    blended.y = 0.0f;

    float blendLen = std::sqrtf(blended.x * blended.x + blended.z * blended.z);
    if (blendLen > 1e-6f)
    {
        blended.x /= blendLen;
        blended.z /= blendLen;
    }

    m_ComShot.IsReady();

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
        if (player.get() == this) continue;
        if (player->GetDeath()) continue;

        CStaticMeshObject* mesh = dynamic_cast<CStaticMeshObject*>(player.get());
        if (!mesh) continue;

        CannonHitRay tempHit;
        if (cannon->RaycastTo(mesh, tempHit))
        {
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
        D3DXVECTOR3 cannonPos = pos;
        cannonPos.y += m_Tuning.cannonHeight;
        m_pCannon->SetPosition(cannonPos);
    }
}

void CComPlayer::FindNearestTarget()
{
    if (m_pAllPlayer) {
        for (auto& p : *m_pAllPlayer) {
        }
    }

    for (CComPlayer* other : Instances()) {
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
    return PersonalityType::Adaptive;
}