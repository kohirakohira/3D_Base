#include "CPersistentPersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

// ターゲット優先度：現在のターゲットを最優先
float CPersistentPersonality::EvaluateTargetPriority(
    const D3DXVECTOR3& selfPos,
    const std::shared_ptr<CCharacterObjectBase>& candidate,
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    float distance,
    float currentTargetDistance) const
{
    // 現在のターゲットなら高スコア
    if (currentTarget && candidate.get() == currentTarget.get())
    {
        return 10000.0f;
    }

    // それ以外は距離で判断
    return 100.0f / (distance + 1.0f);
}

//追いかける
BehaviorDecision CPersistentPersonality::DecideChaseAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    float distanceToTarget,
    int nearbyEnemyCount,
    float hpRatio) const
{
    BehaviorDecision decision;

    //まっすぐ追いかける
    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0.0f;
    decision.desiredYaw = std::atan2f(toTarget.x, toTarget.z);

    // 遠いほど速く追いかける
    decision.moveSpeedMultiplier = 1.0f + std::min(distanceToTarget * 0.02f, 0.3f);

    // 追跡中も撃つ
    decision.shouldFire = true;

    // 他の敵は無視
    decision.shouldEvade = false;

    // 近づく
    decision.keepDistance = 6.0f;

    return decision;
}

//攻撃.逃がさないようにする
BehaviorDecision CPersistentPersonality::DecideAttackAction(
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

    // 相手が逃げてもついていく
    if (distanceToTarget > 10.0f)
    {
        // 追跡モード
        decision.desiredYaw = toYaw;
        decision.moveSpeedMultiplier = 1.2f;
    }
    else
    {
        // 周回しつつ接近
        const int period = 90;  // ゆっくり周回
        float sign = ((stateFrames / period) % 2 == 0) ? +1.f : -1.f;
        decision.desiredYaw = Util::Wrap(toYaw + sign * (D3DX_PI * 0.3f));
        decision.moveSpeedMultiplier = 1.0f;
    }

    decision.shouldFire = true;
    decision.shouldEvade = false;
    decision.keepDistance = 6.0f;

    return decision;
}

// ターゲット切り替え：死ぬまで変えない
bool CPersistentPersonality::ShouldSwitchTarget(
    const std::shared_ptr<CCharacterObjectBase>& currentTarget,
    const std::shared_ptr<CCharacterObjectBase>& newCandidate,
    float currentDist,
    float newDist) const
{
    // 現在のターゲットがいない場合のみ変更
    if (!currentTarget) return true;

    // 現在のターゲットが死亡した場合のみ変更
    if (currentTarget->GetDeath()) return true;

    // それ以外は絶対に変えない
    return false;
}

/*
* TurretParams CPersistentPersonality::GetTurretParams() const
{
    TurretParams params;
    params.turretSpeedMultiplier = 1.4f;    // 砲塔回転を速く
    params.aimAccuracy = 1.0f;              // 精度高い
    params.fireAngleTolerance = 12.0f;      // 標準
    params.predictionAccuracy = 1.0f;       // 予測
    return params;
}
*/