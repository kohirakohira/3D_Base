#pragma once
#include <vector>

/*
	COM‚ÌáŠQ•¨‰ñ”ğƒNƒ‰ƒX
*/

class CComObstacleAvoidance
{
public:
	//ŠÈˆÕ“I‚ÈáŠQ•¨
	struct SimpleObstacle
	{
		D3DXVECTOR3 pos;
		float radius;
	};

	void SetObstacles(const std::vector<SimpleObstacle>* obstacles)
	{
		m_pObstacles = obstacles;
	}

	void SetSelfRadius(float radius) { m_SelfRadius = radius; }
	void SetProbeDist(float dist) { m_ProbeDist = dist; }
	void SetProbeStep(float step) { m_ProbeStep = step; }
	void SetProdeAngle(float angle) { m_ProbeAngleRad = angle; }

	//‘O•û‚ÉáŠQ•¨‚ª‚ ‚é‚©
	bool HasObstacleAhead(
		const D3DXVECTOR3& selfPos,
		float yaw,
		float probeDist,
		float step,
		float& outHitDist)const;

	//ŠëŒ¯ƒ][ƒ“”»’è
	bool IsInDangerZone(const D3DXVECTOR3& pos) const;

	//áŠQ•¨‚ğ”ğ‚¯‚é•ûŒü‚ğŒvZ
	float ComputeAvoidYaw(
		const D3DXVECTOR3& selfPos,
		float curYaw,
		float desiredYaw,
		float turnStep)const;

private:
	const std::vector<SimpleObstacle>* m_pObstacles;	//áŠQ•¨ƒŠƒXƒg
	float m_SelfRadius;									//©•ª‚Ì”¼Œa
	float m_ProbeDist;									//‘O•û‚ğ’T‚é‹——£
	float m_ProbeStep;									//’T¸‚Ì‚İ•
	float m_ProbeAngleRad;								//’T¸‚·‚éŠp“x‚Ì”ÍˆÍ
};