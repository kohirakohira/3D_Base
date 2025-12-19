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

TurretParams CPersistentPersonality::GetTurretParames() const
{
    //パラメータを各COMごとに設定
    TurretParams prames;
    prames.turretSpeedMultiplier = 1.4f;
    prames.aimAccuracy = 1.0f;
    prames.fireAngleTolerance = 12.f;  //標準
    prames.predictionAccuracy = 1.f;

    return prames;
}


BehaviorDecision CPersistentPersonality::DecideMultiEnemyAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    const D3DXVECTOR3& clusterCenter,
    int nearbyEnemyCount,
    float hpRatio) const
{
    BehaviorDecision decision;

    // Persistent は他の敵を無視してターゲットだけを追う
    D3DXVECTOR3 toTarget = targetPos - selfPos;
    toTarget.y = 0.0f;
    decision.desiredYaw = std::atan2f(toTarget.x, toTarget.z);

    // ただし、囲まれているときは少し斜めに移動して包囲を抜ける
    if (nearbyEnemyCount >= 3)
    {
        // 包囲突破：ターゲット方向に斜め移動
        decision.desiredYaw = Util::Wrap(decision.desiredYaw + D3DX_PI * 0.2f);
        decision.moveSpeedMultiplier = 1.3f;  // 速く突破
    }
    else
    {
        decision.moveSpeedMultiplier = 1.1f;
    }

    decision.shouldFire = true;
    decision.shouldEvade = false;  // 逃げない
    decision.keepDistance = 5.0f;

    return decision;
}

float CPersistentPersonality::GetEscapeWeight(int nearbyEnemyCount, float hpRatio) const
{
    // ほぼ逃げない
    return (nearbyEnemyCount >= 3) ? 0.3f : 0.0f;
}

float CPersistentPersonality::GetApproachWeight(int nearbyEnemyCount, float hpRatio) const
{
    // 常にターゲットを追う
    return 1.5f;
}


