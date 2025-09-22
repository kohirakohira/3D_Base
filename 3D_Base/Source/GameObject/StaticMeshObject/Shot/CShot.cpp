#include "CShot.h"
#include <Collision/CollisionManager/CCollisionManager.h>


CShot::CShot()
	: m_Shot()
{
}

CShot::~CShot()
{
}

void CShot::Initialize(int id)
{
	m_Shot.m_Display = false;

}

void CShot::Update()
{
	if (m_Shot.m_Display == true) {
		// �ړ������Ɉړ����x���������킹�����̂����W�ɔ��f
		m_vPosition += m_Shot.m_MoveDirection * m_Shot.m_MoveSpeed;

		// �����x�ɏd�͂��^�����Ă���
		m_Shot.m_Velocity -= m_Shot.m_Gravity;
		// �����x��Y��^����
		m_vPosition.y += m_Shot.m_Velocity / 2 * m_Shot.m_Gravity;

		m_Shot.m_DisplayTime--;
		if (m_Shot.m_DisplayTime < 0) {
			//�����Ȃ����ɒu���Ă���
			m_vPosition = D3DXVECTOR3(0.f, -10.f, 0.f);
			m_Shot.m_Display = false;
		}
	}
}

void CShot::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (m_Shot.m_Display == true) 
	{
		CStaticMeshObject::Draw(View, Proj, Light, Camera);
	}
}

void CShot::Reload(const D3DXVECTOR3& Pos, float RotY)
{
	if (m_Shot.m_Display == true) return;

	m_vPosition = Pos;
	m_vRotation.y = RotY;		// �e�̌���(������)���ς���
	m_Shot.m_Display = true;
	m_Shot.m_Velocity = 0.f;
	m_Shot.m_DisplayTime = FPS * 12;


	// Z���x�N�g��
	m_Shot.m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);

	// Y����]�s��
	D3DXMATRIX mRotationY;
	// Y����]�s����쐬
	D3DXMatrixRotationY(
		&mRotationY,	// (out)�s��
		m_vRotation.y);	// �v���C���[��Y�����̉�]�l

	// Y����]�s����g����Z���x�N�g�������W�ϊ�����
	D3DXVec3TransformCoord(
		&m_Shot.m_MoveDirection,	// (out)Z���x�N�g��
		&m_Shot.m_MoveDirection,	// (in) Z���x�N�g��
		&mRotationY);		// Y����]�s��
}

bool CShot::IsActive() const
{
	if (m_Shot.m_Display)
	{
		return true;
	}
	return false;
}
