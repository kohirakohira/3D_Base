#pragma once

// 基底クラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"

// 見た目（車体・砲塔）
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"

// ショット・アイテム
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"
#include "GameObject/StaticMeshObject/ItemBoxManager/ItemBox/CItemBox.h"

//COM の頭
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComBrain/CComBrain.h"

//便利関数クラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComUtility/CComUtility.h"

// STL
#include <memory>
#include <vector>

class CComPlayer : public CCharacterObjectBase
{
public:

    //定数宣言
    const  float TIME = 1.f / FPS;

    CComPlayer();
    ~CComPlayer() override;

    // 旧Create互換
    void Initialize(int playerId);
    void Create(int playerId) { Initialize(playerId); }

    // 毎フレーム更新／描画
    void Update() override;
    void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

    // 当たり判定など
    void OnHit(CCharacterObjectBase* other) override;

    //ヒット時の処理
    void Hit();

    //ダメージ処理
    void Damage();

    //死亡処理
    void Death();

    // ----- CCharacterObjectBase のインターフェース実装 -----
    std::shared_ptr<CBody>       GetBody() const override { return m_pBody; }
    std::shared_ptr<CCannon>     GetCannon() const override { return m_pCannon; }
    std::shared_ptr<CShotManager>GetShotManager() const override { return m_pShotManager.lock(); }

    D3DXVECTOR3 GetCannonPosition() const override;
    float       GetCannonYaw() const override;

    bool IsPlayer() const override { return false; }

    void SetHasControl(bool enable) override { m_HasControl = enable; }
    bool HasControl() const override { return m_HasControl; }

    //int  GetPlayerID() const { return m_PlayerID; }

    void SetRespawnFlag(bool f) override { m_Respawn = f; }
    bool GetRespawnFlag() const { return m_Respawn; }

    // ----- COM 有効／無効 -----
    void SetComEnabled(bool enabled) { m_ComEnabled = enabled; }
    bool IsComEnabled() const { return m_ComEnabled; }

    // ----- 全 COM リスト -----
    static std::vector<CComPlayer*>& Instances();

    // ----- 外部から差し込む参照 -----
    void AttachShotManager(const std::shared_ptr<CShotManager>& mgr)
    {
        m_pShotManager = mgr;
    }

    void SetPlayersRef(const std::vector<std::shared_ptr<CCharacterObjectBase>>* all);

    void SetItemBoxRef(std::vector<std::shared_ptr<CItemBox>>* boxes)
    {
        m_pItemBox = boxes;
    }

    // 簡易障害物
    struct SimpleObstacle
    {
        D3DXVECTOR3 pos;   // 中心位置（XZ）
        float       radius;
    };
    void SetSimpleObstacles(const std::vector<SimpleObstacle>* obstacles)
    {
        m_pSimpleObstacles = obstacles;
    }

    // Brain に直接アクセスしたい時用
    CComBrain& Brain() { return m_Brain; }
    const CComBrain& Brain() const { return m_Brain; }

private:
    // 内部ショット状態
    struct ShotState
    {
        int   coolDownFrames = 0;           // クールダウン残りフレーム
        int   maxCoolDown = 120;            // クールダウン時間
        float fireAngleEpsDeg = 10.0f;      // この角度以内なら発射
        float muzzleOffsetZ = 1.0f;         // 砲口のオフセット
    };

    //パラメータの安全化
    void SanitizeParams();

    // Brain に渡す観測値を作る
    ComObservation BuildObservation() const;

    // Brain からの指示を反映する
    void ApplyCommand(const ComCommand& cmd);

    // ----- ステアリング系ユーティリティ -----
    static float       Wrap(float r);                      // [-π,π] 正規化
    static float       Approach(float cur, float goal, float step);
    static D3DXVECTOR3 ForwardFromYaw(float yaw);          // (sin,0,cos)

    void ComputeSeparation(const D3DXVECTOR3& selfPos,
        D3DXVECTOR3& outSep,
        float& outNearest) const;

    bool HasObstacleAheadSimple(const D3DXVECTOR3& selfPos,
        float yaw,
        float probeDist,
        float step,
        float& outHitDist) const;

    float SteerWithAvoid(float curYaw, float desiredYaw, float turnStep);
    bool  IsInDangerZone(const D3DXVECTOR3& pos) const;
    void  SafeAdvance(float nextYaw, float moveStep);

    // ----- 砲塔・ショット系 -----
    void SyncCannonToBody();
    void AimTurretAt(const D3DXVECTOR3& targetPos);
    void ComputeMuzzle(D3DXVECTOR3& outPos, float& outYaw) const;
    void TryAutoFire(const ComCommand& cmd);

private:
    // 見た目
    std::shared_ptr<CBody>   m_pBody;
    std::shared_ptr<CCannon> m_pCannon;

    // 管理系
    std::weak_ptr<CShotManager> m_pShotManager;
    const std::vector<std::shared_ptr<CCharacterObjectBase>>* m_pAllPlayer;
    std::vector<std::shared_ptr<CItemBox>>* m_pItemBox;
    std::shared_ptr<CComUtility> m_pUtility;

    //COMの頭クラス
    CComBrain m_Brain;

    //COM 同士・障害物回避用パラメータ
    float m_AvoidRadius;
    float m_AvoidWeight;
    float m_ObstacleRadius;
    float m_ObstacleProbeDist;
    float m_ObstacleProbeStep;
    float m_ProbeAngleRad;

    const std::vector<SimpleObstacle>* m_pSimpleObstacles;

    // ショット
    ShotState m_Shot;

    D3DXVECTOR3 m_PrevPos{};
    bool m_HasPrevPos = false;
    int m_StuckFrames = 0;  //前進できないフレーム数

    // 状態フラグ
    bool m_ComEnabled;
    bool m_HasControl;
    bool m_Respawn;
    bool m_Registered;
    int  m_PlayerID;
};
