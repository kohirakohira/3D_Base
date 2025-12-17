#pragma once

#pragma once
#include <memory>
#include <vector>
#include <unordered_map>

/*
COMのターゲット選択クラス
*/

//前方宣言
class CCharacterObjectBase;

//ターゲット選択結果
struct TargetResult
{
    std::shared_ptr<CCharacterObjectBase> target;   //選択されたターゲット
    float distance = 0.0f;                          //ターゲットまでの距離
    bool isValid = false;                           //有効なターゲットか
};

class CComTargetSelector
{
public:
    CComTargetSelector();
    ~CComTargetSelector() = default;

    //初期化更新
    void Initialize(int ownerID);
    void Update();  

    // プレイヤーリストから最適なターゲットを選択
    TargetResult SelectTarget(
        const D3DXVECTOR3& selfPos,
        const std::vector<std::shared_ptr<CCharacterObjectBase>>* allPlayers);

    // 現在のターゲットを取得
    std::shared_ptr<CCharacterObjectBase> GetCurrentTarget() const { return m_pTarget; }

    // ターゲットをクリア
    void ClearTarget();

    // ターゲットを強制設定
    void ForceSetTarget(std::shared_ptr<CCharacterObjectBase> target);

    //ブラックリスト処理
    void AddToBlacklist(int targetID);
    void RemoveFromBlacklist(int targetID);
    bool IsBlacklisted(int targetID) const;
    void ClearBlacklist();

    //パラメータ設定
    void SetForgetDistance(float dist) { m_ForgetDistance = dist; }
    void SetStickinessRatio(float ratio) { m_StickinessRatio = ratio; }
    void SetBlacklistDuration(int frames) { m_BlacklistDuration = frames; }
    void SetRetargetInterval(int frames) { m_RetargetInterval = frames; }

    //状態取得
    float GetCurrentTargetDistance() const { return m_CurrentTargetDist; }
    bool HasTarget() const { return m_pTarget != nullptr; }
    int GetLostSightFrames() const { return m_LostSightFrames; }

    //ターゲットの速度を取得
    D3DXVECTOR3 GetTargetVelocity() const { return m_TargetVelocity; }

private:
    //ブラックリストの更新
    void TickBlacklist();

private:
    //ターゲット情報
    std::shared_ptr<CCharacterObjectBase> m_pTarget;    // 現在のターゲット
    float m_CurrentTargetDist = 1e9f;                   // ターゲットまでの距離
    int m_LostSightFrames = 0;                          // 見失ってからのフレーム数

    // ブラックリスト
    std::unordered_map<int, int> m_Blacklist;       //IDから残りフレーム
    int m_BlacklistDuration = 120;                  //ブラックリスト継続時間

    // パラメータ
    int m_OwnerID = -1;                 // 自分のID（除外用）
    float m_ForgetDistance = 60.0f;     // これ以上離れたら忘れる
    float m_StickinessRatio = 0.8f;     // ターゲット切り替えの閾値
    int m_RetargetInterval = 120;       // 再選択間隔
    int m_RetargetTimer = 0;            // 再選択タイマー

    // ターゲット追跡用
    D3DXVECTOR3 m_LastTargetPos = { 0, 0, 0 };
    D3DXVECTOR3 m_TargetVelocity = { 0, 0, 0 };

    /*
    struct AimingEnemy
{
    int playerID;
    float aimAngle;         // どれだけこっちを向いているか
    float distance;
};

std::vector<AimingEnemy> m_AimingEnemies;

void UpdateAimingEnemies(const D3DXVECTOR3& selfPos,
    const std::vector<std::shared_ptr<CCharacterObjectBase>>* allPlayers);
bool IsBeingTargeted() const;
int GetMostDangerousEnemy() const;
    */
};