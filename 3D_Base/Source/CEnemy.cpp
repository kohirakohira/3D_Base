#include "CEnemy.h"

CEnemy::CEnemy()
{
}

CEnemy::~CEnemy()
{
}

void CEnemy::Update()
{
	//��O�Ɉړ����Ă���
	m_vPosition.z -= 0.1f;
	if (m_vPosition.z <= 0.f) {
		m_vPosition.z = 20.f;
	}
}
