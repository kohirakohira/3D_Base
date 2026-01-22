#include "CComMovement.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComObstacleAvoidance/CComObstacleAvoidance.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComPlayer.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"

#include <algorithm>
#include <cstdlib>

CComMovement::CComMovement()
	: m_WanderAngle(0.0f)
{
}

void CComMovement::ComputeSeparation(
	const D3DXVECTOR3& selfPos,
	D3DXVECTOR3& outSep,
	float& outNearest) const
{
	outSep = D3DXVECTOR3(0, 0, 0);	//分離ベクトル初期値
	outNearest = 1e9f;				//実距離よりも大きい数にしておく

	
	if (m_Params.avoidRadius <= 0.0f)
	{
		return;
	}

	//オーナーを取得
	auto owner = m_pOwner.lock();

	//オーナー存在チェック
	if (!owner)
	{
		return;
	}

	const float avoidRadius = m_Params.avoidRadius;
	const float avoidRadiusSq = avoidRadius * avoidRadius;	//二乗距離

	//他のCOMとの分離計算
	for (CComPlayer* other : CComPlayer::Instances())
	{
		if (other == owner.get())
		{
			continue;
		}

		std::shared_ptr<CBody> ob;

		if (other)
		{
			ob = other->GetBody();
		}
		else
		{
			ob = nullptr;
		}

		if (!ob)
		{
			continue;
		}

		D3DXVECTOR3 offset = selfPos - ob->GetPosition();
		offset.y = 0.0f;

		const float distSq = offset.x * offset.x + offset.z * offset.z;

		if (distSq <= 1e-6f)
		{
			outSep.x += 0.1f;
			continue;
		}

		//一番近い相手までの距離を更新
		outNearest = std::min(outNearest, std::sqrtf(distSq));

		if (distSq < avoidRadius)
		{
			//近いほど強い反発
			const float invDistSq = 1.0f / distSq;
			outSep.z += offset.x * invDistSq;
			outSep.z += offset.z * invDistSq;
		}
	}
}

