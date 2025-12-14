// CAdaptivePersonality.cpp
#include "CAdaptivePersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

// ターゲット優先度：総合的に判断
float CAdaptivePersonality::EvaluateTargetPriority(
    const D3DXVECTOR3& selfPos,
    const std::shared_ptr<CCharacterObjectBase>& candidate,
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    float distance,
    float currentTargetDistance) const
{
    float score = 0.0f;

    // 距離スコア
    score += 100.0f / (distance + 1.0f);
#if 1
    // HPスコア（瀕死の敵を優先）
    float hpRatio = static_cast<float>(candidate->GetHP()) /
        static_cast<float>(candidate->GetMaxHP());
    score += (1.0f - hpRatio) * 50.0f;
#endif
    // 現在のターゲットを維持するボーナス（安定性）
    if (currentTarget && candidate.get() == currentTarget.get())
    {
        score += 30.0f;
    }

    return score;
}

// 追跡：状況に応じて行動
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

    // 複数敵に囲まれている場合
    if (nearbyEnemyCount >= 2)
    {
        // 斜めに移動（被弾を減らす）
        decision.desiredYaw = Util::Wrap(baseYaw + D3DX_PI * 0.25f);
        decision.moveSpeedMultiplier = 0.8f;
    }
    // HP低下時
    else if (hpRatio < 0.3f)
    {
        // 慎重に
        decision.desiredYaw = baseYaw;
        decision.moveSpeedMultiplier = 0.7f;
        decision.shouldEvade = true;
    }
    else
    {
        // 通常追跡
        decision.desiredYaw = baseYaw;
        decision.moveSpeedMultiplier = 1.0f;
    }

    decision.shouldFire = true;
    decision.keepDistance = 9.0f;

    return decision;
}

// 攻撃：周回しながら撃つ
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

    // HP低下時は距離を取る
    if (hpRatio < 0.3f)
    {
        decision.desiredYaw = Util::Wrap(toYaw + D3DX_PI);  // 逃げる
        decision.moveSpeedMultiplier = 1.1f;
        decision.keepDistance = 15.0f;
        decision.shouldEvade = true;
    }
    // 複数敵時は逃げながら攻撃
    else if (nearbyEnemyCount >= 2)
    {
        decision.desiredYaw = Util::Wrap(toYaw + D3DX_PI * 0.7f);
        decision.moveSpeedMultiplier = 1.0f;
        decision.keepDistance = 12.0f;
    }
    else
    {
        // 通常：周回攻撃
        const int period = 60;
        float sign = ((stateFrames / period) % 2 == 0) ? +1.f : -1.f;
        decision.desiredYaw = Util::Wrap(toYaw + sign * (D3DX_PI * 0.5f));
        decision.moveSpeedMultiplier = 1.0f;
        decision.keepDistance = 9.0f;
    }

    decision.shouldFire = true;

    return decision;
}

// ターゲット切り替え：慎重に判断
bool CAdaptivePersonality::ShouldSwitchTarget(
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    const std::shared_ptr<CCharacterObjectBase>& newCandidate,
    float currentDist,
    float newDist) const
{
    // 現在のターゲットがいない
    if (!currentTarget) return true;

    // 現在のターゲットが死亡
    if (currentTarget->GetDeath()) return true;

    // 新しい敵がかなり近い場合のみ切り替え
    if (newDist < currentDist * 0.6f) return true;

    // 新しい敵が瀕死
    float newHpRatio = static_cast<float>(newCandidate->GetHP()) /
        static_cast<float>(newCandidate->GetMaxHP());
    if (newHpRatio < 0.2f && newDist < currentDist * 0.9f) return true;

    return false;
}

//TurretParams CAdaptivePersonality::GetTurretParams() const
//{
//    TurretParams params;
//    params.turretSpeedMultiplier = 1.5f;    // 砲塔回転を速くして周回中も追従
//    params.aimAccuracy = 1.0f;              // 精度高い
//    params.fireAngleTolerance = 10.0f;      // 精密射撃
//    params.predictionAccuracy = 1.0f;       // 完璧な予測
//    return params;
//}