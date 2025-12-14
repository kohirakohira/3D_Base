#pragma once
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/IComPersonality.h"

class CAdaptivePersonality : public IComPersonality
{
public:
    CAdaptivePersonality() = default;
    ~CAdaptivePersonality() override = default;

    PersonalityType GetType() const override { return PersonalityType::Adaptive; }

    float EvaluateTargetPriority(
        const D3DXVECTOR3& selfPos,
        const std::shared_ptr<CCharacterObjectBase>& candidate,
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        float distance,
        float currentTargetDistance) const override;

    BehaviorDecision DecideChaseAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        float distanceToTarget,
        int nearbyEnemyCount,
        float hpRatio) const override;

    BehaviorDecision DecideAttackAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        float distanceToTarget,
        int nearbyEnemyCount,
        float hpRatio,
        int stateFrames) const override;

    float GetEvadeDistanceMultiplier() const override { return 0.6f; }
    float GetAttackEnterDistanceMultiplier() const override { return 1.05f; }

    bool ShouldSwitchTarget(
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        const std::shared_ptr<CCharacterObjectBase>& newCandidate,
        float currentDist,
        float newDist) const override;
};