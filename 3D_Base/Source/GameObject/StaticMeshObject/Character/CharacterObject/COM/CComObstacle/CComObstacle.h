#pragma once

#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComPlayer.h"
#include<memory>
/*
	COMáŠQ•¨”F¯ŠÇ—ƒNƒ‰ƒX
*/

//áŠQ•¨î•ñ
struct  SimpleObstacle
{
	D3DXVECTOR3 pos;
	float radius;
};

class CComObstacle
{
public:


private:
	std::shared_ptr<SimpleObstacle>* m_SimpleObstacle;	//áŠQ•¨î•ñ
	float 	m_SelfRadius;								//©g‚Ì”¼Œa
	float	m_ProbeDist;
	float 	m_ProbeStep;
	float	m_ProbeAngleRad;
};