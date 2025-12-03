#pragma once
#include <unordered_map>
#include <memory>

#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComBody/IComBody.h"

class CCharacterObjectBase;
class IComBody;


//そのフレーム時点でCOMの頭が知ることができる情報
struct ComObservation
{
    D3DXVECTOR3 selfPos;                //自分の位置.ワールド座標XZ平面
    float       selfYaw;                //本体のヨー角

    bool        hasTarget = false;      //有効なターゲットがいるか
    D3DXVECTOR3 targetPos;              //ターゲット位置

    float       dist2 = 0.0f;           //自分とターゲットの距離2
    int         lostSightFrames = 0;    //ターゲットを見失ってからのフレーム
    int         selfPlayerID = -1;      //自分のPlayerID

    //一番近いアイテムまでの距離なども足せる
    float       nearestItemDist2 = 1e18f;
};

// Brain が出す指示
struct ComCommand
{
    // 本体の向き
    float desiredBodyYaw = 0.0f;

    // 前進量（このフレームで進みたい距離。0 なら止まる）
    float moveStep = 0.0f;

    // 砲塔関連
    bool  aimAtTarget = false;   // 砲塔をターゲットに向けるべきか
    bool  tryFire = false;   // 発射を試みるか

    // 今の状態（デバッグ用に外から見えるように）
    enum class State
    {
        Seek,
        Chase,
        Attack,
        Evade,
        ItemSeek,
    } state = State::Seek;
};

/*COMの頭脳クラス*/

class CComBrain
{
public:
    struct Config
    {
        float keepDistance = 9.0f;
        float attackRadius = 10.0f;
        float seekRadius = 5.0f;
        float avoidRadius = 10.0f;
        float avoidWeight = 2.0f;
        float fireAngleEpsDeg = 10.0f;
        float forgetDistance = 60.0f;
        float stickinessRatio = 0.8f;
    };

    CComBrain();

    void SetConfig(const Config& cfg) { m_Config = cfg; }

    // --- 毎フレーム呼ぶ ---
    // observation を渡すと、そのフレームの command が返ってくる
    void Update(const ComObservation& obs, ComCommand& outCmd);

    ComCommand::State GetState() const { return m_State; }

    // ターゲットへの弱参照
    std::weak_ptr<CCharacterObjectBase> GetTarget() const { return m_Target; }

    // ターゲット候補リスト（全プレイヤー）を渡す
    void SetPlayersRef(const std::vector<std::shared_ptr<CCharacterObjectBase>>* allPlayers)
    {
        m_pAllPlayer = allPlayers;
    }

private:
    Config m_Config;

    ComCommand::State m_State;
    int   m_StateFrames;
    int   m_LostSightFrames;

    std::weak_ptr<CCharacterObjectBase> m_Target;
    const std::vector<std::shared_ptr<CCharacterObjectBase>>* m_pAllPlayer = nullptr;

    std::unordered_map<int, int> m_TargetBlackList;
    int   m_BlackListTime;

    float m_CurrentTargetDist2;
    float m_WanderAngle;

    int   m_RetargetIntervalFrames;
    int   m_RetargetTimer;

private:
    // 内部処理
    void UpdateTarget(const ComObservation& obs);
    void EvaluateTransitions(float dist2);
    void ChangeState(ComCommand::State s);

    void StepSeek(const ComObservation& obs, ComCommand& cmd);
    void StepChase(const ComObservation& obs, ComCommand& cmd);
    void StepAttack(const ComObservation& obs, ComCommand& cmd);
    void StepEvade(const ComObservation& obs, ComCommand& cmd);
    void StepItemSeek(const ComObservation& obs, ComCommand& cmd);

    static float Wrap(float a);
    static float Approach(float cur, float goal, float step);

    void TickWander();
    void TickBlacklist();
    bool IsBlacklisted(int id) const;
    void Blacklist(int id);
};
