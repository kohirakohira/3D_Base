#pragma once

#include <memory>

class CComPlayer;
class CCharacterObjectBase;

//COMのタイプ
enum class MoveType
{
	Aggressive,		//特攻タイプ
	Persistent,		//追いかける
	Human,			//人間に一番近い
};

//行動パラメータ
struct BehaviorDecision
{
    float desiredYaw = 0.0f;            // 目標方向
    float moveSpeedMultiplier = 1.0f;   // 速度倍率
    bool shouldFire = true;             // 射撃するか
    bool shouldEvade = false;           // 回避するか
    float keepDistance = 9.0f;          // 維持距離
};

/*
    COMインスタフェース
    それぞれのCOMを作成する時に継承させる
*/

class IComInterfase 
{
    IComInterfase();

    virtual ~IComInterfase() = 0;

    //取得・セット
    virtual MoveType GetMoveType() const = 0;
    virtual void SetMoveType(MoveType type)  = 0{ m_MoveType = type; }

    //ターゲット
    virtual float EvaluateTargetPriority(
        const D3DXVECTOR3& selfPos,
        const std::shared_ptr<CCharacterObjectBase>& candidate,
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        float Distance, float currentDistance ) const = 0;

    //追跡時の行動
    virtual BehaviorDecision DecivdeChaseAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        float distanceToTarget,
        int nearbyEnemyCount,
        float hp) const = 0;

private:
    MoveType m_MoveType;
};
