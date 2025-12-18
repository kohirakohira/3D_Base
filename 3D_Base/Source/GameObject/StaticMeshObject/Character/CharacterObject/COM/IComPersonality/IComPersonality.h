#pragma once
#include <d3dx9math.h>
#include <memory>

class CComPlayer;
class CCharacterObjectBase;

//性格タイプ
enum class PersonalityType
{
    Aggressive,     // 特攻型
    Adaptive,       // 人間型
    Persistent      // 執念型
};

// 行動決定結果
struct BehaviorDecision
{
    float desiredYaw = 0.0f;            // 目標方向
    float moveSpeedMultiplier = 1.0f;   // 速度倍率
    bool shouldFire = true;             // 射撃するか
    bool shouldEvade = false;           // 回避するか
    float keepDistance = 9.0f;          // 維持距離
};

//COMごとのパラメータ
struct TurretParams
{
    float turretSpeedMultiplier = 1.0f;   // 砲塔回転速度倍率
    float aimAccuracy = 1.0f;             // 照準精度
    float fireAngleTolerance = 12.0f;     // 射撃許容角度
    float predictionAccuracy = 1.0f;      // 予測精度
};


//性格インターフェース
class IComPersonality
{
public:
    virtual ~IComPersonality() = default;

    // 性格タイプ取得
    virtual PersonalityType GetType() const = 0;

    // ターゲット選択の優先度を調整
    virtual float EvaluateTargetPriority(
        const D3DXVECTOR3& selfPos,
        const std::shared_ptr<CCharacterObjectBase>& candidate,
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        float distance,
        float currentTargetDistance) const = 0;

    // 追跡時の行動決定
    virtual BehaviorDecision DecideChaseAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        float distanceToTarget,
        int nearbyEnemyCount,
        float hpRatio) const = 0;

    // 攻撃時の行動決定
    virtual BehaviorDecision DecideAttackAction(
        const D3DXVECTOR3& selfPos,
        const D3DXVECTOR3& targetPos,
        float distanceToTarget,
        int nearbyEnemyCount,
        float hpRatio,
        int stateFrames) const = 0;

    // 状態遷移の閾値を調整
    virtual float GetEvadeDistanceMultiplier() const = 0;
    virtual float GetAttackEnterDistanceMultiplier() const = 0;

    // ターゲットを変更するか判定
    virtual bool ShouldSwitchTarget(
        const std::shared_ptr<CCharacterObjectBase>& currentTarget,
        const std::shared_ptr<CCharacterObjectBase>& newCandidate,
        float currentDist,
        float newDist) const = 0;

    // 戦車の情報を各性格クラスに渡す
    virtual TurretParams GetTurretParames() const = 0;
    
    //複数の敵時の状態を取得
    virtual float GetEscapeWeight(int nearbyEnemyCount, float hpRadius) const { return 1.0f; } 
    virtual float GetApproachWeight(int nearbyEnemyCount, float hpRadius) const { return 1.0f; }

    /*
    // 複数敵時の行動を決定
virtual BehaviorDecision DecideMultiEnemyAction(
    const D3DXVECTOR3& selfPos,
    const D3DXVECTOR3& targetPos,
    const D3DXVECTOR3& clusterCenter,
    int nearbyEnemyCount,
    float hpRatio) const = 0;


// 複数敵時の重みを取得
virtual float GetEscapeWeight(int nearbyEnemyCount, float hpRatio) const { return 1.0f; }
virtual float GetApproachWeight(int nearbyEnemyCount, float hpRatio) const { return 1.0f; }
    */
};