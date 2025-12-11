#include "CComShot.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

void CComShot::Initialize(int ownerID)
{
    m_OwnerID = ownerID;
    m_Cooldown = 0;
}

bool CComShot::TryFire(const D3DXVECTOR3& targetPos, const D3DXVECTOR3& targetVelocity, CBody* body, CCannon* cannon)
{
    if (!m_pShotManager) return false;

    //クールダウン中
    if (m_Cooldown > 0) return false;

    //砲口位置を計算
    D3DXVECTOR3 muzzle;
    float yaw = 0.0f;
    ComputeMuzzle(muzzle, yaw, body, cannon);

    //予測射撃
    PredictedShot prediction = PredictTargetPosition(muzzle, targetPos, targetVelocity);
    const D3DXVECTOR3 aimPoint = prediction.aimPoint;

    //狙い位置への角度を計算
    D3DXVECTOR3 to = aimPoint - muzzle;
    to.y = 0.0f;
    const float d2 = to.x * to.x + to.z * to.z;
    if (d2 <= 1e-6f) return false;

    const float desiredYaw = std::atan2f(to.x, to.z);
    const float err = std::fabs(Util::Wrap(desiredYaw - yaw));

    // 許容角度
    float allowedAngle = Util::ToRad(m_Config.fireAngleDeg);
    allowedAngle *= (0.5f + prediction.confidence * 0.5f);

    // 角度が許容範囲内なら発射
    if (err <= allowedAngle)
    {
        m_pShotManager->Create(muzzle, yaw, true, m_OwnerID);
        m_Cooldown = m_Config.cooldownFrames;
        return true;
    }

    return false;
}

void CComShot::TickCooldown()
{
    if (m_Cooldown > 0)
    {
        --m_Cooldown;
    }
}

PredictedShot CComShot::PredictTargetPosition(const D3DXVECTOR3& muzzlePos, const D3DXVECTOR3& targetPos, const D3DXVECTOR3& targetVelocity) const
{
    PredictedShot result;
    result.canHit = false;
    result.confidence = 0.0f;
    result.aimPoint = targetPos;

    //ターゲットまでの距離
    D3DXVECTOR3 toTarget = targetPos - muzzlePos;
    toTarget.y = 0.0f;
    float distance = D3DXVec3Length(&toTarget);

    if (distance < 0.1f)
    {
        result.canHit = true;
        result.confidence = 1.0f;
        return result;
    }

    //弾がターゲットに届くまでの時間
    float flightTime = distance / m_Config.bulletSpeed;

    //ターゲットの予測位置
    D3DXVECTOR3 predictedPos;
    predictedPos.x = targetPos.x + targetVelocity.x * flightTime;
    predictedPos.y = targetPos.y;
    predictedPos.z = targetPos.z + targetVelocity.z * flightTime;

    // 反復計算で精度を上げる
    D3DXVECTOR3 toPredicted = predictedPos - muzzlePos;
    toPredicted.y = 0.0f;
    float newDist = D3DXVec3Length(&toPredicted);
    float newFlightTime = newDist / m_Config.bulletSpeed;

    predictedPos.x = targetPos.x + targetVelocity.x * newFlightTime;
    predictedPos.z = targetPos.z + targetVelocity.z * newFlightTime;

    result.aimPoint = predictedPos;
    result.canHit = true;

    //速度が遅いほど予測しやすい
    float speed = D3DXVec3Length(&targetVelocity);
    result.confidence = 1.0f / (1.0f + speed * 1.5f);

    return result;
}

//砲口のワールド座標とYaw角を計算
void CComShot::ComputeMuzzle(
    D3DXVECTOR3& outPos,
    float& outYaw,
    CBody* body,
    CCannon* cannon) const
{
    D3DXVECTOR3 base(0, 0, 0);
    float yaw = 0.0f;

    if (body)
    {
        base = body->GetPosition();
        yaw = body->GetRotation().y;
    }

    if (cannon)
    {
        if (!body) base = cannon->GetPosition();
        yaw = cannon->GetRotation().y;  // 砲塔の向きを優先
    }

    base.y += m_Config.cannonHeight;
    const D3DXVECTOR3 forward = Util::ForwardFromYaw(yaw);

    outPos = base + forward * m_Config.muzzleOffsetZ;
    outYaw = yaw;
}