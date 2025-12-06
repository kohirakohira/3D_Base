#pragma once

//ライブラリ
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>

#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComBody/IComBody.h"

class CCharacterObjectBase;
class IComBody;
class CComUtility;
class CNavGrid;

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

//フレームでCOMにどう動くかを渡すための行動指示
struct ComCommand
{
    //本体の向き
    float desiredBodyYaw = 0.0f;

    //前進量
    float moveStep = 0.0f;

    //砲塔関連
    bool  aimAtTarget = false;      //砲塔をターゲットに向けるべきか
    bool  tryFire = false;          //発射を試みるか

    //今の状態
    enum class State
    {
        Seek,
        Chase,
        Attack,
        Evade,
        ItemSeek,
    } state = State::Seek;
};

/*COMの頭クラス*/

class CComBrain
{
public:
    struct Config
    {
        float keepDistance = 9.0f;      //ターゲットとの戦闘距離
        float attackRadius = 10.0f;     //攻撃モードに入る距離
        float seekRadius = 5.0f;        //探索中のときどの範囲に敵がいたら追いかけるか    
        float avoidRadius = 10.0f;      //COM同士の分離
        float avoidWeight = 2.0f;       //分離側の重み
        float fireAngleEpsDeg = 10.0f;  //角度誤差の許容範囲
        float forgetDistance = 60.0f;   //どの距離でターゲットを追うのをやめるか
        float stickinessRatio = 0.8f;   //ターゲットの乗り換えのしにくさ
    };

    CComBrain();

    void SetConfig(const Config& cfg) { m_Config = cfg; }

    // --- 毎フレーム呼ぶ ---
    // observation を渡すと、そのフレームの command が返ってくる
    void Update(const ComObservation& obs, ComCommand& outCmd);

    ComCommand::State GetState() const { return m_State; }

    // ターゲットへの弱参照
    std::weak_ptr<CCharacterObjectBase> GetTarget() const { return m_Target; }

    // ターゲット候補リストを渡す
    void SetPlayersRef(const std::vector<std::shared_ptr<CCharacterObjectBase>>* allPlayers)
    {
        m_pAllPlayer = allPlayers;
    }

    //外部からターゲットを設定できるように
    void SetTarget(std::shared_ptr<CCharacterObjectBase> target)
    {
        m_Target = target;
    }

    //Configを取得
    const Config& GetConfig() const { return m_Config; }

    //個別に取得したい場合
    float GetKeepDistance() const { return m_Config.keepDistance; }

    void SetNavGrid(CNavGrid* navGrid) { m_pNavGrid = navGrid; }

private:
    Config m_Config;

    ComCommand::State m_State;
    int   m_StateFrames;
    int   m_LostSightFrames;

    std::weak_ptr<CCharacterObjectBase> m_Target;
    const std::vector<std::shared_ptr<CCharacterObjectBase>>* m_pAllPlayer;

    std::unordered_map<int, int> m_TargetBlackList;
    int   m_BlackListTime;

    float m_CurrentTargetDist2;
    float m_WanderAngle;

    int   m_RetargetIntervalFrames;
    int   m_RetargetTimer;

    //便利関数クラス
    std::shared_ptr<CComUtility> m_pUtility;

    //ナビゲーション
    CNavGrid* m_pNavGrid = nullptr;

    //安定化用のメンバ変数
    int m_CircleDirection = 1;           // 周回方向
    float m_LastDecidedYaw = 0.0f;       // 最後に決定した方向
    int m_DecisionCooldown = 0;          // 方向決定のクールダウン
    static constexpr int DECISION_INTERVAL = 15;  // 方向を再評価する間隔（フレーム）

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

    //戦術判断
    bool IsDirectionSafe(const D3DXVECTOR3& from, float yaw, float checkDist) const;
    float EvaluatePosition(const D3DXVECTOR3& pos, const D3DXVECTOR3& targetPos) const;
    D3DXVECTOR3 FindBestAttackPosition(const D3DXVECTOR3& selfPos, const D3DXVECTOR3& targetPos) const;
    D3DXVECTOR3 FindSafeEscapeDirection(const D3DXVECTOR3& selfPos, const D3DXVECTOR3& threatPos) const;

    float StabilizeDirection(float newYaw, float currentYaw);
};
