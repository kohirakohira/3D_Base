#include "CComObstacleAvoidance.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <cmath>

bool CComObstacleAvoidance::HasObstacleAhead(
	const D3DXVECTOR3& selfPos,
	float yaw,
	float probeDist,
	float step,
	float& outHitDist) const
{
	outHitDist = probeDist;

	if (!m_pObstacles || m_pObstacles->empty())
	{
		return false;
	}

	for (float d = step; d <= probeDist; d += step)
	{

	}
}