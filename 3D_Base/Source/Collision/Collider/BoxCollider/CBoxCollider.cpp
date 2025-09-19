#include "CBoxCollider.h"

CBoxCollider::CBoxCollider()
	: m_Min()
	, m_Max()
	, m_MaxPos()
	, m_MinPos()
{
}

CBoxCollider::~CBoxCollider()
{
}

bool CBoxCollider::CheckCollisionSphere(const CSphereCollider& sphere) const
{
	return sphere.CheckCollisionBox(*this);
}

bool CBoxCollider::CheckCollisionBox(const CBoxCollider& box) const
{
	return (m_MinPos.x <= box.m_MaxPos.x && m_MaxPos.x >= box.m_MinPos.x) &&
		(m_MinPos.y <= box.m_MaxPos.y && m_MaxPos.y >= box.m_MinPos.y) &&
		(m_MinPos.z <= box.m_MaxPos.z && m_MaxPos.z >= box.m_MinPos.z);
}

void CBoxCollider::SetPosition(const D3DXVECTOR3& pos)
{
	m_CenterPos = pos;
	m_MaxPos = m_CenterPos + m_Max;
	m_MinPos = m_CenterPos + m_Min;
}