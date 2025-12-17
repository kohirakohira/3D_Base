#include "CComTargetSelector.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>
#include <limits>

CComTargetSelector::CComTargetSelector()
    : m_pTarget(nullptr)
    , m_CurrentTargetDist(1e9f)
    , m_LostSightFrames(0)
    , m_BlacklistDuration(120)
    , m_OwnerID(-1)
    , m_ForgetDistance(60.0f)
    , m_StickinessRatio(0.8f)
    , m_RetargetInterval(120)
    , m_RetargetTimer(0)
{
}

void CComTargetSelector::Initialize(int ownerID)
{
    m_OwnerID = ownerID;
    m_pTarget = nullptr;
    m_CurrentTargetDist = 1e9f;
    m_LostSightFrames = 0;
    m_RetargetTimer = 0;
    m_Blacklist.clear();
}

void CComTargetSelector::Update()
{
    // ブラックリストの時間を減らす
    TickBlacklist();

    // ターゲットの速度を計算
    if (m_pTarget)
    {
        D3DXVECTOR3 currentPos = m_pTarget->GetPosition();
        m_TargetVelocity = currentPos - m_LastTargetPos;
        m_LastTargetPos = currentPos;
        m_LostSightFrames = 0;
    }
    else
    {
        m_TargetVelocity = { 0, 0, 0 };
        ++m_LostSightFrames;
    }

    if (m_RetargetTimer > 0)
    {
        --m_RetargetTimer;
    }
}

TargetResult CComTargetSelector::SelectTarget(
    const D3DXVECTOR3& selfPos,
    const std::vector<std::shared_ptr<CCharacterObjectBase>>* allPlayers)
{
    TargetResult result;
    result.isValid = false;

    if (!allPlayers) return result;

    // タイマーが残っていて、現在のターゲットが有効ならそのまま
    if (m_RetargetTimer > 0 && m_pTarget)
    {
        result.target = m_pTarget;
        result.distance = m_CurrentTargetDist;
        result.isValid = true;
        return result;
    }

    // 最適なターゲットを探す
    std::shared_ptr<CCharacterObjectBase> best;
    float bestDist = std::numeric_limits<float>::infinity();

    for (const auto& p : *allPlayers)
    {
        if (!p) continue;
        if (p->GetPlayerID() == m_OwnerID) continue;  // 自分は除外
        if (IsBlacklisted(p->GetPlayerID())) continue;
        if (p->GetDeath()) continue;  // 死んでいるターゲットは除外

        const float dist = Util::DistXZ(selfPos, p->GetPosition());
        if (dist < bestDist)
        {
            bestDist = dist;
            best = p;
        }
    }

    // 適切なターゲットが見つからない
    if (!best)
    {
        m_pTarget.reset();
        m_CurrentTargetDist = 1e9f;
        return result;
    }

    // 新しいターゲットか、現在のターゲットがいない場合
    if (!m_pTarget)
    {
        m_pTarget = best;
        m_CurrentTargetDist = bestDist;
        m_RetargetTimer = m_RetargetInterval;

        result.target = m_pTarget;
        result.distance = m_CurrentTargetDist;
        result.isValid = true;
        return result;
    }

    // 現在のターゲットとの距離
    const float curDist = Util::DistXZ(selfPos, m_pTarget->GetPosition());

    // より近いターゲットがいる場合
    if (best.get() != m_pTarget.get() && bestDist < curDist * m_StickinessRatio)
    {
        m_pTarget = best;
        m_CurrentTargetDist = bestDist;
    }
    else
    {
        m_CurrentTargetDist = curDist;
    }

    // 遠くなりすぎたら忘れる
    const float forgetDistSq = m_ForgetDistance * m_ForgetDistance;
    if (m_CurrentTargetDist * m_CurrentTargetDist > forgetDistSq)
    {
        AddToBlacklist(m_pTarget->GetPlayerID());
        m_pTarget.reset();
        m_CurrentTargetDist = 1e9f;
        return result;
    }

    m_RetargetTimer = m_RetargetInterval;

    result.target = m_pTarget;
    result.distance = m_CurrentTargetDist;
    result.isValid = true;
    return result;
}

void CComTargetSelector::ClearTarget()
{
    //ターゲットが死亡していたら
    if (!m_pTarget) return;

    // ターゲットが死亡していたらクリア
    if (m_pTarget->GetDeath() == true)
    {
        m_pTarget.reset();
        m_CurrentTargetDist = 1e9f;
        m_LostSightFrames = 0;
    }
}

void CComTargetSelector::ForceSetTarget(std::shared_ptr<CCharacterObjectBase> target)
{
    m_pTarget = target;
    m_LostSightFrames = 0;
    m_RetargetTimer = m_RetargetInterval;
}

void CComTargetSelector::AddToBlacklist(int targetID)
{
    m_Blacklist[targetID] = m_BlacklistDuration;
}

void CComTargetSelector::RemoveFromBlacklist(int targetID)
{
    m_Blacklist.erase(targetID);
}

bool CComTargetSelector::IsBlacklisted(int targetID) const
{
    return m_Blacklist.find(targetID) != m_Blacklist.end();
}

void CComTargetSelector::ClearBlacklist()
{
    m_Blacklist.clear();
}

void CComTargetSelector::TickBlacklist()
{
    for (auto it = m_Blacklist.begin(); it != m_Blacklist.end();)
    {
        if (--(it->second) <= 0)
        {
            it = m_Blacklist.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/*
void CComTargetSelector::UpdateAimingEnemies(
    const D3DXVECTOR3& selfPos,
    const std::vector<std::shared_ptr<CCharacterObjectBase>>* allPlayers)
{
    m_AimingEnemies.clear();
    if (!allPlayers) return;

    for (const auto& p : *allPlayers)
    {
        if (!p || p->GetPlayerID() == m_OwnerID) continue;
        if (p->GetDeath()) continue;

        D3DXVECTOR3 enemyPos = p->GetPosition();
        float enemyYaw = p->GetRotation().y;

        // 敵から自分への方向
        D3DXVECTOR3 toSelf = selfPos - enemyPos;
        toSelf.y = 0.0f;
        float dist = std::sqrtf(toSelf.x * toSelf.x + toSelf.z * toSelf.z);
        if (dist < 1e-6f) continue;

        float angleToSelf = std::atan2f(toSelf.x, toSelf.z);
        float angleDiff = std::fabs(Util::Wrap(angleToSelf - enemyYaw));

        // 30度以内でこっちを向いている
        if (angleDiff < D3DX_PI / 6.0f)
        {
            AimingEnemy ae;
            ae.playerID = p->GetPlayerID();
            ae.aimAngle = angleDiff;
            ae.distance = dist;
            m_AimingEnemies.push_back(ae);
        }
    }
}

bool CComTargetSelector::IsBeingTargeted() const
{
    return !m_AimingEnemies.empty();
}

int CComTargetSelector::GetMostDangerousEnemy() const
{
    if (m_AimingEnemies.empty()) return -1;

    int mostDangerous = -1;
    float highestDanger = 0.0f;

    for (const auto& ae : m_AimingEnemies)
    {
        // 近くて、正確に狙っているほど危険
        float danger = (1.0f / (ae.distance + 1.0f)) * (1.0f - ae.aimAngle / (D3DX_PI / 6.0f));
        if (danger > highestDanger)
        {
            highestDanger = danger;
            mostDangerous = ae.playerID;
        }
    }
    return mostDangerous;
}
*/