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
    float baseYaw = std::atan2f(toTarget.x, toTarget.z);

    //他COMとの重なり回避
    if (nearbyEnemyCount >= 2)
    {
        // 周囲に2体以上いる場合、少し角度をずらす
        // 自分のIDや位置に基づいてずらす方向を決める（ランダムだと振動する）
        float offset = (selfPos.x + selfPos.z > 0) ? 0.3f : -0.3f;  // 約17度
        decision.desiredYaw = baseYaw + offset;
    }
    else
    {
        decision.desiredYaw = baseYaw;
    }

    //速度は最大
    decision.moveSpeedMultiplier = 1.0f;

    //常に撃つ
    decision.shouldFire = true;

    //回避しない
    decision.shouldEvade = false;

    //距離0でも突っ込む
    decision.keepDistance = 2.0f;

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

    //常に撃つ
    decision.shouldFire = true;

    //HPが低くても回避しない
    decision.shouldEvade = false;

    //3mはたもつ
    decision.keepDistance = 3.0f;

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

TurretParams CAggressivePersonality::GetTurretParames() const
{
    TurretParams params;
    params.turretSpeedMultiplier = 1.3;     //砲塔倍率
    params.aimAccuracy = 1.0f;              //昇順速度
    params.fireAngleTolerance = 15.f;       //角度許容範囲
    params.predictionAccuracy = 0.9f;       //予測精度

    return params;
}

