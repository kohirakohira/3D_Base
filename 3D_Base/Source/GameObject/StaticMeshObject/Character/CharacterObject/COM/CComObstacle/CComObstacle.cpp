#include "CComObstacle.h"

CComObstacle::CComObstacle()
	: m_pObstacles			(nullptr)
	, m_SelfRadius			(1.0f)
	, m_ProbeDist			(5.0f)
	, m_ProbeStep			(0.5f)
	, m_ProbeAngleRad		(0.785f)
{
}
