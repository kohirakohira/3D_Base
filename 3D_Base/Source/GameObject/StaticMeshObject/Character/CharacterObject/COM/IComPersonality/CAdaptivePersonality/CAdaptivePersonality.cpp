// CAdaptivePersonality.cpp
#include "CAdaptivePersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

// ƒ^[ƒQƒbƒg—Dæ“xF‘‡“I‚É”»’f
float CAdaptivePersonality::EvaluateTargetPriority(
    const D3DXVECTOR3& selfPos,
    const std::shared_ptr<CCharacterObjectBase>& candidate,
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    float distance,
    float currentTargetDistance) const
{
    float score = 0.0f;

    // ‹——£ƒXƒRƒA
    score += 100.0f / (distance + 1.0f);

    //HP—Dæ.HP‚ª2‚µ‚©‚È‚¢‚Ì‚Å1—Dæ
    float hpRatio = static_cast<float>(candidate->GetHP()) /
        static_cast<float>(candidate->GetMaxHP());
    score += (1.0f - hpRatio) * 50.0f;
    
    //Œ»İ‚Ìƒ^[ƒQƒbƒg‚ğˆÛ
    if (currentTarget && candidate.get() == currentTarget.get())
    {
        score += 30.0f;
    }

    return score;
}

// ’ÇÕFó‹µ‚É‰‚¶‚Äs“®
BehaviorDecision CAdaptivePersonality::DecideChaseAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    float distanceToTarget,
    int nearbyEnemyCount,
    float hpRatio) const
{
    BehaviorDecision decision;

    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0.0f;
    float baseYaw = std::atan2f(toTarget.x, toTarget.z);

    // •¡”“G‚ÉˆÍ‚Ü‚ê‚Ä‚¢‚éê‡
    if (nearbyEnemyCount >= 2)
    {
        // Î‚ß‚ÉˆÚ“®
        decision.desiredYaw = Util::Wrap(baseYaw + D3DX_PI * 0.25f);
        decision.moveSpeedMultiplier = 0.8f;
    }
    // HP’á‰º
    else if (hpRatio < 0.3f)
    {
        // Td‚É
        decision.desiredYaw = baseYaw;
        decision.moveSpeedMultiplier = 0.7f;
        decision.shouldEvade = true;
    }
    else
    {
        // ’Êí’ÇÕ
        decision.desiredYaw = baseYaw;
        decision.moveSpeedMultiplier = 1.0f;
    }

    decision.shouldFire = true;
    decision.keepDistance = 9.0f;

    return decision;
}

// UŒ‚Fü‰ñ‚µ‚È‚ª‚çŒ‚‚Â
BehaviorDecision CAdaptivePersonality::DecideAttackAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    float distanceToTarget,
    int nearbyEnemyCount,
    float hpRatio,
    int stateFrames) const
{
    BehaviorDecision decision;

    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0.0f;
    float toYaw = std::atan2f(toTarget.x, toTarget.z);

    // HP’á‰º‚Í‹——£‚ğæ‚é
    if (hpRatio < 0.3f)
    {
        decision.desiredYaw = Util::Wrap(toYaw + D3DX_PI);  // “¦‚°‚é
        decision.moveSpeedMultiplier = 1.1f;
        decision.keepDistance = 15.0f;
        decision.shouldEvade = true;
    }
    // •¡”“G‚Í“¦‚°‚È‚ª‚çUŒ‚
    else if (nearbyEnemyCount >= 2)
    {
        printf("[DecidAttackAction], decision%d \n",
            decision.shouldEvade);
        
        decision.desiredYaw = Util::Wrap(toYaw + D3DX_PI * 0.7f);
        decision.moveSpeedMultiplier = 1.0f;
        decision.keepDistance = 12.0f;
    }
    else
    {
        // ’ÊíFü‰ñUŒ‚
        const int period = 60;
        float sign = ((stateFrames / period) % 2 == 0) ? +1.f : -1.f;
        decision.desiredYaw = Util::Wrap(toYaw + sign * (D3DX_PI * 0.5f));
        decision.moveSpeedMultiplier = 1.0f;
        decision.keepDistance = 9.0f;
    }

    decision.shouldFire = true;

    return decision;
}

// ƒ^[ƒQƒbƒgØ‚è‘Ö‚¦
bool CAdaptivePersonality::ShouldSwitchTarget(
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    const std::shared_ptr<CCharacterObjectBase>& newCandidate,
    float currentDist,
    float newDist) const
{
    // Œ»İ‚Ìƒ^[ƒQƒbƒg‚ª‚¢‚È‚¢
    if (!currentTarget) return true;

    // Œ»İ‚Ìƒ^[ƒQƒbƒg‚ª€–S
    if (currentTarget->GetDeath()) return true;

    // V‚µ‚¢“G‚ª‚©‚È‚è‹ß‚¢ê‡‚Ì‚İØ‚è‘Ö‚¦
    if (newDist < currentDist * 0.6f) return true;

    // V‚µ‚¢“G‚ª•m€
    float newHpRatio = static_cast<float>(newCandidate->GetHP()) /
        static_cast<float>(newCandidate->GetMaxHP());
    if (newHpRatio < 0.2f && newDist < currentDist * 0.9f) return true;
    
    //©•ª‚ª€–S
    float thisDeath;

    //‘Šè‚ªƒ_ƒ[ƒW‚ğó‚¯‚½ê‡AnewDist‚É‚Í“ü‚ç‚¸’Ç‚¢‘±‚¯‚é
    if (currentTarget->GetDamage() == true)
    {
        newDist = 1000.f;
        if (newDist < currentDist)
        {
            return true;
        }
    }

    return false;
}

TurretParams CAdaptivePersonality::GetTurretParames() const
{
    TurretParams params;
    params.turretSpeedMultiplier = 1.5;     //–C“ƒ”{—¦
    params.aimAccuracy = 1.0f;              //¸‡‘¬“x
    params.fireAngleTolerance = 10.f;       //Šp“x‹–—e”ÍˆÍ
    params.predictionAccuracy = 1.0f;       //—\‘ª¸“x

    return params;
}

BehaviorDecision CAdaptivePersonality::DecideMultiEnemyAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    const D3DXVECTOR3& clusterCenter,
    int nearbyEnemyCount,
    float hpRatio) const
{
    BehaviorDecision decision;

    // ó‹µ‚É‰‚¶‚Ä“¦‚°‚ÆU‚ß‚ğƒuƒŒƒ“ƒh
    float escapeW = GetEscapeWeight(nearbyEnemyCount, hpRatio);
    float approachW = GetApproachWeight(nearbyEnemyCount, hpRatio);

    // U‚ß•ûŒü
    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0.0f;
    float toTargetLen = std::sqrtf(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
    if (toTargetLen > 1e-6f) { toTarget.x /= toTargetLen; toTarget.z /= toTargetLen; }

    // “¦‚°•ûŒüiŒQ‚ê‚Ì’†S‚©‚ç—£‚ê‚éj
    D3DXVECTOR3 escape = selfPos - clusterCenter;
    escape.y = 0.0f;
    float escapeLen = std::sqrtf(escape.x * escape.x + escape.z * escape.z);
    if (escapeLen > 1e-6f) { escape.x /= escapeLen; escape.z /= escapeLen; }

    // ƒuƒŒƒ“ƒh
    D3DXVECTOR3 blended;
    blended.x = toTarget.x * approachW + escape.x * escapeW;
    blended.z = toTarget.z * approachW + escape.z * escapeW;

    decision.desiredYaw = std::atan2f(blended.x, blended.z);
    decision.moveSpeedMultiplier = 0.9f;  // ‚â‚âTd
    decision.shouldFire = true;
    decision.shouldEvade = (hpRatio < 0.5f);
    decision.keepDistance = 10.0f + nearbyEnemyCount * 2.0f;  // “G‚ª‘½‚¢‚Ù‚Ç‹——£‚ğæ‚é

    return decision;
}

float CAdaptivePersonality::GetEscapeWeight(int nearbyEnemyCount, float hpRatio) const
{
    // HP‚ª’á‚¢‚Ù‚ÇA“G‚ª‘½‚¢‚Ù‚Ç“¦‚°‚ğ—Dæ
    float hpFactor = 1.0f - hpRatio;  // HP50%‚È‚ç0.5
    float countFactor = std::min(nearbyEnemyCount * 0.3f, 1.0f);
    return 0.5f + hpFactor * 0.5f + countFactor * 0.5f;
}

float CAdaptivePersonality::GetApproachWeight(int nearbyEnemyCount, float hpRatio) const
{
    // HP‚ª‚‚¢‚Ù‚ÇU‚ß‚ğ—Dæ
    return 0.5f + hpRatio * 0.5f;
}

