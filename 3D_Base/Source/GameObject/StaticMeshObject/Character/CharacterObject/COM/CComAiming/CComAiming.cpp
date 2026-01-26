#include "CComAiming.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/IComPersonality/IComPersonality.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComShot/CComShot.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

CComAiming::CComAiming()
{
}

void CComAiming::AimAt(
	std::shared_ptr<CCannon> cannon,
	std::shared_ptr<CBody> body,
	const D3DXVECTOR3& targetPos,
	const D3DXVECTOR3& targetVelocity,
	const TurretParams& turretParams,
	CComShot& comShot)
{
	if (!cannon)
	{
		return;
	}

	//ƒ^[ƒQƒbƒgˆÊ’u‚ð•½ŠŠ‰»
	m_SmoothedTargetPos.x += (targetPos.x - m_SmoothedTargetPos.x) * m_Config.smoothFactor;
	m_SmoothedTargetPos.y += (targetPos.y - m_SmoothedTargetPos.y) * m_Config.smoothFactor;
	m_SmoothedTargetPos.z += (targetPos.z - m_SmoothedTargetPos.z) * m_Config.smoothFactor;

	//–CŒûˆÊ’uŽæ“¾
	D3DXVECTOR3 muzzle;
	float currentYaw;
	comShot.ComputeMuzzle(muzzle, currentYaw, body, cannon);

	//—\‘ª¸“x‚ð“K—p‚µ‚½‘¬“x
	D3DXVECTOR3 adjustedVel = targetVelocity;
	adjustedVel.x *= turretParams.predictionAccuracy;
	adjustedVel.z *= turretParams.predictionAccuracy;

	//—\‘ªˆÊ’u‚ðŒvŽZ
	PredictedShot prediction = comShot.PredictTargetPosition(
		muzzle, m_SmoothedTargetPos, adjustedVel);

	//–C“ƒ‚ÌŠî€ˆÊ’u
	D3DXVECTOR3 base;

	if (body)
	{
		base = body->GetPosition();
	}
	else
	{
		base = cannon->GetPosition();
	}

	base.y += m_Config.cannonHeight;

	//–Ú•W•ûŒü
	const D3DXVECTOR3 toAim = prediction.aimPoint - base;
	const float desiredYaw = std::atan2f(toAim.x, toAim.z);

	//–C“ƒ‰ñ“]‘¬“x‚ð«Ši‚Å’²®
	float turretSpeed = (m_Config.turretTurnSpeed + 0.02f) * turretParams.turretSpeedMultiplier;

	float cyaw = cannon->GetRotation().y; 
	cyaw = Util::Approach(cyaw, cyaw + Util::Wrap(desiredYaw - cyaw), turretSpeed);

	cannon->SetPosition(base);
	cannon->SetRotation(D3DXVECTOR3(0.0f, cyaw, 0.0f));
	cannon->CStaticMeshObject::Update();
}

void CComAiming::SyncToBody(
	std::shared_ptr<CCannon> cannon,
	std::shared_ptr<CBody> body
)
{
	if (!body || !cannon)
	{
		return;
	}

	D3DXVECTOR3 pos = body->GetPosition();
	pos.y += m_Config.cannonHeight;
	cannon->SetPosition(pos);
}


