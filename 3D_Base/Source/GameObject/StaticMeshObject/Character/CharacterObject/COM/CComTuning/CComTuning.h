#pragma once

// 移動関連パラメータ
struct MovementParams
{
    float moveSpeed = 0.08f;
    float bodyTurnSpeed = 0.03f;
    float turretTurnSpeed = 0.03f;
    float cannonHeight = 0.3f;
};

// 距離関連パラメータ
struct DistanceParams
{
    float keepDistance = 9.0f;      //ターゲットとの距離を保つ
    float seekRadius = 5.0f;        //探索半径
    float attackRadius = 50.0f;     //攻撃開始半径
    float closenessRadius = 1.0f;   //最小接近距離
};

// COM同士の分離パラメータ
struct SeparationParams
{
    float avoidRadius = 10.0f;      //回避半径
    float avoidWeight = 2.0f;       //分離ベクトルの重み
};

// 障害物回避パラメータ
struct ObstacleParams
{
    float probeDist = 8.0f;         //探査距離
    float probeStep = 0.1f;         //探査刻み幅
    float selfRadius = 1.5f;        //自機の半径
    float probeAngleRad = 0.436f;   //探査角度
    float avoidMaxFrames = 30.0f;   //最大回避フレーム
};

// 複数敵対応パラメータ
struct MultiEnemyParams
{
    float detectionRadius = 8.0f;   //検出範囲
    float escapeWeight = 0.6f;      //逃げの重み
    float approachWeight = 0.4f;    //攻めの重み
    int threshold = 2;              //複数敵と判定する閾値
};

// 徘徊パラメータ
struct WanderParams
{
    D3DXVECTOR3 mapCenter = D3DXVECTOR3(0, 0, 0);
    float wanderRadius = 15.0f;
    float centerPullStrength = 0.3f;
    float wanderDelta = 0.10f;
    float wanderClamp = 0.6f;
};

// 回避行動パラメータ
struct EvadeParams
{
    int duration = 60;              //回避継続フレーム
    float speedMultiplier = 0.6f;   //回避時の速度倍率
};

// 射撃パラメータ
struct FireParams
{
    float coneDeg = 10.0f;          //射撃許容角度
    float targetRadius = 2.0f;      //ターゲット判定半径
};


class CComTuning
{
public:
    CComTuning();

    // パラメータの検証・修正
    void Sanitize();

    // 各パラメータへのアクセス
    MovementParams& Movement() { return m_Movement; }
    const MovementParams& Movement() const { return m_Movement; }

    DistanceParams& Distance() { return m_Distance; }
    const DistanceParams& Distance() const { return m_Distance; }

    SeparationParams& Separation() { return m_Separation; }
    const SeparationParams& Separation() const { return m_Separation; }

    ObstacleParams& Obstacle() { return m_Obstacle; }
    const ObstacleParams& Obstacle() const { return m_Obstacle; }

    MultiEnemyParams& MultiEnemy() { return m_MultiEnemy; }
    const MultiEnemyParams& MultiEnemy() const { return m_MultiEnemy; }

    WanderParams& Wander() { return m_Wander; }
    const WanderParams& Wander() const { return m_Wander; }

    EvadeParams& Evade() { return m_Evade; }
    const EvadeParams& Evade() const { return m_Evade; }

    FireParams& Fire() { return m_Fire; }
    const FireParams& Fire() const { return m_Fire; }

    //互換性のための旧インターフェース
    float GetMoveSpeed() const { return m_Movement.moveSpeed; }
    float GetBodyTurnSpeed() const { return m_Movement.bodyTurnSpeed; }
    float GetTurretTurnSpeed() const { return m_Movement.turretTurnSpeed; }
    float GetCannonHeight() const { return m_Movement.cannonHeight; }
    float GetKeepDistance() const { return m_Distance.keepDistance; }
    void SetKeepDistance(float dist) { m_Distance.keepDistance = dist; }

private:
    MovementParams m_Movement;
    DistanceParams m_Distance;
    SeparationParams m_Separation;
    ObstacleParams m_Obstacle;
    MultiEnemyParams m_MultiEnemy;
    WanderParams m_Wander;
    EvadeParams m_Evade;
    FireParams m_Fire;
};
