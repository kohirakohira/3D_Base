#include "CShot.h"

CShot::CShot()
{
}

CShot::~CShot()
{
}

void CShot::Initialize(int id)
{
	m_Display = false;
	m_MoveSpeed = 0.2f;
	m_LifeFramesInit = 120;
	m_Gravity = 0.1f;
	m_VelocityY = 0.0f;
	m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		m_Shot[i].m_Display = false;
	}
#endif
}

void CShot::Update()
{

	if (!m_Display) return;

	//�����ړ�
	m_vPosition += m_MoveDirection * m_MoveSpeed;

	////����
	//if (m_Gravity != 0.0f)
	//{
	//	m_VelocityY -= m_Gravity;
	//	m_vPosition.y += m_VelocityY;
	//}

	//�e����
	if (--m_LifeFrames <= 0) {
		m_Display = false;
	}

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) {
			// �ړ������Ɉړ����x���������킹�����̂����W�ɔ��f
			m_vPosition += m_Shot[i].m_MoveDirection * m_Shot[i].m_MoveSpeed;

			// �����x�ɏd�͂��^�����Ă���
			m_Shot[i].m_Velocity -= m_Shot[i].m_Gravity;
			// �����x��Y��^����
			m_vPosition.y += m_Shot[i].m_Velocity / 2 * m_Shot[i].m_Gravity;

			m_Shot[i].m_DisplayTime--;
			if (m_Shot[i].m_DisplayTime < 0) {
				//�����Ȃ����ɒu���Ă���
				m_vPosition = D3DXVECTOR3(0.f, -10.f, 0.f);
				m_Shot[i].m_Display = false;
			}
		}
	}
#endif
}

void CShot::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (!m_Display) return;
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}

void CShot::Reload(const D3DXVECTOR3& Pos, float RotY)
{
	//�e��V�K���˂Ƃ��ď�����
	m_vPosition = Pos;
	m_vRotation.y = RotY;		// �e�̌���(������)���ς���

	//�O����Y��]rotY�ŉ񂵂��������ړ������ɂ���
	m_MoveDirection = D3DXVECTOR3(std::sinf(RotY), 0.0f, std::cosf(RotY));	//���K��
	m_VelocityY = 0.0f;

	m_LifeFrames = (m_LifeFramesInit > 0) ? m_LifeFramesInit : 120;
	m_Display = true;

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) return;

		m_vPosition = Pos;
		m_vRotation.y = RotY;		// �e�̌���(������)���ς���
		m_Shot[i].m_Display = true;
		m_Shot[i].m_Velocity = 0.f;
		m_Shot[i].m_DisplayTime = FPS * 3;


		// Z���x�N�g��
		m_Shot[i].m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);

		// Y����]�s��
		D3DXMATRIX mRotationY;
		// Y����]�s����쐬
		D3DXMatrixRotationY(
			&mRotationY,	// (out)�s��
			m_vRotation.y);	// �v���C���[��Y�����̉�]�l

		// Y����]�s����g����Z���x�N�g�������W�ϊ�����
		D3DXVec3TransformCoord(
			&m_Shot[i].m_MoveDirection,	// (out)Z���x�N�g��
			&m_Shot[i].m_MoveDirection,	// (in) Z���x�N�g��
			&mRotationY);		// Y����]�s��
	}
#endif
}

