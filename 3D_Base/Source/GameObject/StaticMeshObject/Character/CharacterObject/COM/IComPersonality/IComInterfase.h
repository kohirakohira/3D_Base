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
*/

class IComInterfase 
{
    IComInterfase();

    virtual ~IComInterfase() = 0;

    virtual MoveType GetType() const = 0;

    virtual float EvaluateTargetPriority(
        const D3DXVECTOR3& selfPos,
        const std::shared_ptr<CCharacterObjectBase>& candidate,
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        float Distance
    ) = 0;

};
