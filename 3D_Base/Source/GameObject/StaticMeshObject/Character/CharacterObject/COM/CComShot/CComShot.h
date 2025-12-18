#pragma once

#include <memory>


class CShotManager;
class CBody;
class CCannon;
class CComTargetSelector;

/*
COMのショット関連のクラス
*/

//予測射撃結果
struct PredictedShot
{
    D3DXVECTOR3 aimPoint;       // 狙う位置
    float confidence;           // 予測の確信度 (0-1)
    bool canHit;                // 命中可能か
};

//射撃設定
struct ShotConfig
{
    int cooldownFrames = 120;           // クールダウン
    float fireAngleDeg = 360.0f;        // 許容角度
    float muzzleOffsetZ = 0.5f;         // 砲口オフセット
    float cannonHeight = 0.3f;          // 砲塔高さ
    float bulletSpeed = 0.8f;           // 弾速 
};

class CComShot
{
public:
    CComShot();
    ~CComShot() = default;

    //初期化と設定
    void Initialize(int ownerID);
    void SetShotManager(std::shared_ptr<CShotManager> mgr) { m_pShotManager = mgr; }
    void SetConfig(const ShotConfig& cfg) { m_Config = cfg; }
    ShotConfig& GetConfig() { return m_Config; }

    //射撃処理
    bool TryFire(
        const D3DXVECTOR3& targetPos,
        const D3DXVECTOR3& targetVelocity,
        std::shared_ptr<CBody> body,
        std::shared_ptr<CCannon> cannon);

    // クールダウン更新
    void TickCooldown();

    //予測射撃
    PredictedShot PredictTargetPosition(
        const D3DXVECTOR3& muzzlePos,
        const D3DXVECTOR3& targetPos,
        const D3DXVECTOR3& targetVelocity) const;

    //砲口計算
    void ComputeMuzzle(
        D3DXVECTOR3& outPos,
        float& outYaw,
        std::shared_ptr<CBody> body,
        std::shared_ptr<CCannon> cannon) const;

    //状態取得
    bool IsReady() const { return m_Cooldown <= 0; }
    int GetCooldown() const { return m_Cooldown; }

    //ショットクールダウン
    void SetShotCollDown(int collDown) { m_Config.cooldownFrames = collDown; }

    // レイヒット時の発射
    bool TryFireOnRayHit(std::shared_ptr<CBody> body, std::shared_ptr<CCannon> cannon);

private:
    std::shared_ptr<CShotManager> m_pShotManager;
    ShotConfig m_Config;
    int m_OwnerID;
    int m_Cooldown;
};