#pragma once
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/IComPersonality.h"

class CAggressivePersonality : public IComPersonality
{
public:
    CAggressivePersonality() = default;
    ~CAggressivePersonality() override = default;

    //性格タイプ取得
    PersonalityType GetType() const override { return PersonalityType::Aggressive; }

    //性格タイプ設定
    void SetType(PersonalityType personalityType) override { m_PersonalityType = personalityType; }

    //ターゲット選択の優先度を調整
    float EvaluateTargetPriority(
        const D3DXVECTOR3& selfPos,
        const std::shared_ptr<CCharacterObjectBase>& candidate,
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        float distance,
        float currentTargetDistance) const override;

    //追跡時の行動決定
    BehaviorDecision DecideChaseAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        float distanceToTarget,
        int nearbyEnemyCount,
        float hpRatio) const override;

    //攻撃時の行動決定
    BehaviorDecision DecideAttackAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        float distanceToTarget,
        int nearbyEnemyCount,
        float hpRatio,
        int stateFrames) const override;

    //状態遷移の閾値を調整
    float GetEvadeDistanceMultiplier() const override { return 0.3f; }  // 回避距離を短く
    float GetAttackEnterDistanceMultiplier() const override { return 1.5f; }  // 早めに攻撃開始

    //ターゲットを変更するか判定
    bool ShouldSwitchTarget(
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        const std::shared_ptr<CCharacterObjectBase>& newCandidate,
        float currentDist,
        float newDist) const override;

    //戦車のパラメータ取得
    TurretParams GetTurretParames() const override;

    // 複数敵時の行動を決定
    BehaviorDecision DecideMultiEnemyAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        const D3DXVECTOR3& clusterCenter,
        int nearbyEnemyCount,
        float hpRatio) const override;


    // 複数敵時の重みを取得
    float GetEscapeWeight(int nearbyEnemyCount, float hpRatio) const override;
    float GetApproachWeight(int nearbyEnemyCount, float hpRatio) const  override;

};