#pragma once

//-----継承するクラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"

//-----外部のヘッダー-----
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"
#include "Collision/Collider/BoxCollider/CBoxCollider.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/CPlayer.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComTargetSelector/CComTargetSelector.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComShot/CComShot.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/IComPersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CSimplePathfinder/CSimplePathfinder.h"
#include "GameObject/StaticMeshObject/Shot/CShot.h"

// 障害物回避クラス（SimpleObstacle構造体もここで定義されている）
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComObstacleAvoidance/CComObstacleAvoidance.h"

//-----ライブラリ-----
#include <d3dx9math.h>
#include <unordered_map>
#include <limits>
#include <unordered_set>
#include <memory>
#include <deque>


class CComPlayer
    : public CCharacterObjectBase
{
public:

    CComPlayer();
    ~CComPlayer() override;

    // 動作関数
    void Update() override;
    // 描画関数
    void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

    // プレイヤーかCOMを判定する
    bool IsPlayer() const override { return false; }

    // 操作可能かどうか
    void SetHasControl(bool enable) override { m_HasControl = enable; }
    bool HasControl() const override { return m_HasControl; }

    void Create(int id);

    static std::vector<CComPlayer*>& Instances();

    // COMの有効無効を決める
    void SetComEnabled(bool enabled) { m_ComEnabled = enabled; }
    bool IsComEnabled() const { return m_ComEnabled; }

    // キャラクターマネージャーで使う用
    void AttachShotManager(std::shared_ptr<CShotManager>& mgr) { m_pShotManager = mgr; }

    // プレイヤーを取得する（読み取り専用）
    void SetPlayersRef(const std::vector<std::shared_ptr<CCharacterObjectBase>>* all) { m_pAllPlayer = all; }

    // 障害物用のBOXセット
    void SetBoxColliders(const std::vector<std::shared_ptr<CBoxCollider>>* colliders)
    {
        m_pBoxCollider = colliders;
    }

    int GetPlayerID() const { return m_PlayerID; }

    void CreateCollider();

    // 弾マネージャーの設定
    void SetShotManager(std::shared_ptr<CShotManager> shot) override;

    D3DXVECTOR3 GetPosition() const override
    {
        if (m_pBody) return m_pBody->GetPosition();
        return D3DXVECTOR3(0, 0, 0);
    }

    // 位置設定
    void SetPosition(const D3DXVECTOR3& pos) override;

    // 回転取得
    D3DXVECTOR3 GetRotation() const override
    {
        if (m_pBody) return m_pBody->GetRotation();
        return D3DXVECTOR3(0, 0, 0);
    }
    // 回転設定
    void SetRotation(const D3DXVECTOR3& rot) override { if (m_pBody) return m_pBody->SetRotation(rot); }

    void FindNearestTarget();

    // 障害物設定（CComObstacleAvoidanceに委譲）
    void SetSimpleObstacles(const std::vector<SimpleObstacle>* obstacles)
    {
        m_ObstacleAvoidance.SetObstacles(obstacles);
    }

    // ターゲット取得
    std::shared_ptr<CCharacterObjectBase> GetTarget() const
    {
        return m_TargetSelector.GetCurrentTarget();
    }

    // 射線判定用障害物を設定
    void SetFireLineObstacles(const std::vector<CStaticMeshObject*>* obstacles)
    {
        m_pFireLineObstacles = obstacles;
    }

    // 性格設定
    void SetPersonality(std::unique_ptr<IComPersonality> personality);
    void SetPersonalityType(PersonalityType type);
    PersonalityType GetPersonalityType() const;

    // 経路探索器を設定
    void SetPathfinder(CSimplePathfinder* pathfinder) { m_pPathfinder = pathfinder; }

private:

    // 列挙型
    // COMの状態
    enum class State
    {
        Seek,       // 探索
        Chase,      // 追跡
        Attack,     // 攻撃
        Evade,      // 離脱
    };

    // 関数
    void StepSeek();                                                // 探索処理
    void StepChase();                                               // 追跡処理
    void StepAttack();                                              // 攻撃処理
    void StepEvade();                                               // 離脱処理
    void TryAutoFire();                                             // COMの弾発射処理
    void SanitizeParams();                                          // パラメータ調整
    void TickChaseTo(const D3DXVECTOR3& targetPos);                 // 追尾
    void TickAimTo(const D3DXVECTOR3& targetPos);                   // 砲塔追尾
    void TickWander();                                              // 徘徊
    void SyncCannonToBody();                                        // 砲塔を車体に追従させる
    void TransitionTo(State state);                                 // ステータスを変更する
    void EvaluateTransitions(float dist);                           // 条件に応じて状態変更

    // レイでの障害物判定
    bool HitObjectRay();

    bool FollowPath(float turnStep, float moveStep);

    // 前進
    void SafeAdvance(float nextYaw, float moveStep);

    // 分離（COMが重なったりするのを防ぐ計算）
    void ComputeSeparation(const D3DXVECTOR3& selfPos,
        D3DXVECTOR3& outSep, float& outNearest) const;

    // COMの状態変更
    void ChangeState(State state);

    // 外部クラス
    const std::vector<std::shared_ptr<CCharacterObjectBase>>* m_pAllPlayer;
    const std::vector<std::shared_ptr<CBoxCollider>>* m_pBoxCollider;
    std::unordered_set<const CCharacterObjectBase*> m_Black;
    std::deque<D3DXVECTOR3> m_Path;

    // COMの各パラメータ
    bool    m_ComEnabled;
    float   m_KeepDistance;
    float   m_AvoidRadius;
    float   m_AvoidWeight;
    float   m_SeekRadius;
    float   m_AttacRadius;
    float   m_FireConeDeg;
    float   m_ClosenessRadius;
    int     m_EvadeDuration;
    int     m_EvadeFrames;
    D3DXVECTOR3 m_LastSeenPos;
    bool    m_IsTarget;
    bool    m_Registered;

    // 探索処理パラメータ
    State   m_State;
    int     m_StateFrames;
    float   m_WanderAngle;

    bool    m_Respawn;

    D3DXVECTOR3 m_MapCenter;
    float m_WanderRadius;
    float m_CenterPullStrength;

    float m_LookAheadSkep;

    // COMの複数敵判定
    float m_MultiEnemyRadius = 8.0f;
    float m_EscapeWeight = 0.6f;
    float m_ApproachWeight = 0.4f;
    int   m_MultiEnemyThreshold = 2;

    // 障害物リスト（射線判定用）
    const std::vector<CStaticMeshObject*>* m_pFireLineObstacles = nullptr;

    // 複数体敵対応関数
    int CountNeardyEnemies(float radius, D3DXVECTOR3& outClusterCenter) const;

    float ComputeBlendedDirection(const D3DXVECTOR3& self,
        const D3DXVECTOR3& targetPos,
        const D3DXVECTOR3& clusterCenter,
        float escapeWeight,
        float approachWeight) const;


    // ===ダメージの設定・取得===
    virtual void SetDamage(bool flg) override { m_Chara.m_Damage = flg; }
    virtual bool GetDamage() const override { return m_Chara.m_Damage; }

    // =====死亡の設定・取得=====
    virtual void SetDeath(bool flg) override { m_Chara.m_Death = flg; }
    virtual bool GetDeath() const override { return m_Chara.m_Death; }

    // =====無敵の設定・取得=====
    void SetMuteki(bool flg) override { m_Chara.m_Muteki = flg; }
    bool GetMuteki() const override { return m_Chara.m_Muteki; }

    int GetPlayerID() override { return m_PlayerID; }

    // 砲塔レイで最初に当たったキャラクターを取得
    std::shared_ptr<CCharacterObjectBase> GetRayHitCharacter() const;

    // 目的地へのパスを計算
    bool RequestPath(const D3DXVECTOR3& goal);

    // 追尾クラス
    CComTargetSelector m_TargetSelector;

    // COMショットクラス
    CComShot m_ComShot;

    CShot m_Shot;

    std::unique_ptr<IComPersonality> m_pPersonality;

    CSimplePathfinder* m_pPathfinder = nullptr;
    int m_PathRecalcTimer = 0;
    static const int PATH_RECALC_INTERVAL = 60;

    // COMが追尾中にガタガタする対策
    D3DXVECTOR3 m_SmoothedTargetPos = { 0,0,0 };
    float m_AimSmoothFactor = 0.25f;

    // ===== 障害物回避クラス =====
    CComObstacleAvoidance m_ObstacleAvoidance;
};