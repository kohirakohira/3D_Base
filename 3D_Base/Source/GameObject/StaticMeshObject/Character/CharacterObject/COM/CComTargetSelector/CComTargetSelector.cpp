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

    // ターゲットがいない場合は見失いフレームを増加
    if (!m_pTarget)
    {
        ++m_LostSightFrames;
    }
    else
    {
        m_LostSightFrames = 0;
    }

    // リターゲットタイマーを減らす
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

    // より近いターゲットがいる場合（スティッキネス考慮）
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
    m_pTarget.reset();
    m_CurrentTargetDist = 1e9f;
    m_LostSightFrames = 0;
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

