#include "CGround.h"

CGround::CGround()
	: m_pPlayer		( nullptr )
{
}

CGround::~CGround()
{
	m_pPlayer = nullptr;
}

void CGround::Update()
{
//	//�n�ʂ��X�N���[��������
//	m_vPosition.z -= 0.2f;
//	if (m_vPosition.z < -100.f) {
//		m_vPosition.z = 0.f;
//	}

	//-------------------------
	// Y���̃��C����
	//-------------------------
	RAY ray = m_pPlayer->GetRayY();	//�v���C���[�������Ă��郌�C�̏��
	float Distance = 0.f;
	D3DXVECTOR3 Intersect(0.f, 0.f, 0.f);

	//�v���C���[�̃��C�Ɠ����蔻��
	if (IsHitForRay(ray, &Distance, &Intersect) == true)
	{
		D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
		Pos.y = Intersect.y + 1.f; //1.f�ŏ�����֕␳
		m_pPlayer->SetPosition(Pos);
	}

	//-------------------------
	// �\���̃��C����
	//-------------------------
	CROSSRAY CrossRay = m_pPlayer->GetCrossRay();
	//�ǂƂ̓����蔻��
	CalculatePositionFromWall(&CrossRay);
	D3DXVECTOR3 Pos = m_pPlayer->GetPosition();
	Pos.x = CrossRay.Ray[CROSSRAY::XL].Position.x;
	Pos.z = CrossRay.Ray[CROSSRAY::XL].Position.z;
	m_pPlayer->SetPosition(Pos);

}
