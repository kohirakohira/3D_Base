// CAggressivePersonality.cpp
#include "CAggressivePersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

// ターゲット優先度：とにかく近い敵
float CAggressivePersonality::EvaluateTargetPriority(
    const D3DXVECTOR3& selfPos,
    const std::shared_ptr<CCharacterObjectBase>& candidate,
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    float distance,
    float currentTargetDistance) const
{
    // 距離だけで判断
    return 1000.0f / (distance + 0.1f);
}

//ひたすら突っ込む
BehaviorDecision CAggressivePersonality::DecideChaseAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    float distanceToTarget,
    int nearbyEnemyCount,
    float hpRatio) const
{
    BehaviorDecision decision;

    // まっすぐ突っ込む
    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0.0f;
    decision.desiredYaw = std::atan2f(toTarget.x, toTarget.z);

    //速度は最大
    decision.moveSpeedMultiplier = 1.2f;

    //常に撃つ
    decision.shouldFire = true;

    //回避しない
    decision.shouldEvade = false;

    //距離0でも突っ込む
    decision.keepDistance = 0.0f;

    return decision;
}

// 攻撃：接近戦
BehaviorDecision CAggressivePersonality::DecideAttackAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    float distanceToTarget,
    int nearbyEnemyCount,
    float hpRatio,
    int stateFrames) const
{
    BehaviorDecision decision;

    // ターゲットに向かって突っ込む
    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0.0f;
    decision.desiredYaw = std::atan2f(toTarget.x, toTarget.z);

    // 近づくほど速く
    decision.moveSpeedMultiplier = 1.0f + (1.0f / (distanceToTarget + 1.0f)) * 0.5f;

    // 常に撃つ
    decision.shouldFire = true;

    // HPが低くても回避しない
    decision.shouldEvade = false;

    // ベタ付き
    decision.keepDistance = 2.0f;

    return decision;
}

// ターゲット切り替え.常に一番近い敵を狙う
bool CAggressivePersonality::ShouldSwitchTarget(
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    const std::shared_ptr<CCharacterObjectBase>& newCandidate,
    float currentDist,
    float newDist) const
{
    // 新しい敵が近ければ即座に切り替え
    return newDist < currentDist * 0.9f;
}

//TurretParams CAggressivePersonality::GetTurretParams() const
//{
//    TurretParams params;
//    params.turretSpeedMultiplier = 1.3f;    // 砲塔回転速い
//    params.aimAccuracy = 1.0f;              // 精度高い
//    params.fireAngleTolerance = 15.0f;      // 多少ずれても撃つ
//    params.predictionAccuracy = 0.9f;       // 予測やや甘い
//    return params;
//}