#include "CShot.h"
#include <cmath>

static constexpr int kDefaultShotLifeFrames = 180; //3�b��

//yow����O�����̒P�ʃx�N�g��������
D3DXVECTOR3 CShot::ForwardFromYaw(float yaw)
{
	return D3DXVECTOR3(std::sinf(yaw), 0.0f, std::cosf(yaw));
}

CShot::CShot()
	: m_Shot()
{
}

CShot::~CShot()
{
}

void CShot::Initialize(int id)
{
	m_ID = id;

	//����͔�\���E����0
	m_Shot = Shot{};
	m_Shot.m_Display = false;
	m_Shot.m_DisplayTime = 0;
}

void CShot::Update()
{
	if (!m_Shot.m_Display) return;

	//���ʈړ�
	D3DXVECTOR3 pos = GetPosition();
	pos += m_Shot.m_MoveDirection * m_Shot.m_MoveSpeed;

	//�d��
	if (m_Shot.m_Gravity != 0.0f)
	{
		//�������ɗ��Ƃ��݌v
		m_Shot.m_MoveSpeed -= m_Shot.m_Gravity;
		m_Shot.m_Velocity -= m_Shot.m_Gravity;
		pos.y += m_Shot.m_Velocity;
	}

	SetPosition(pos);

	if (m_Shot.m_DisplayTime > 0) --m_Shot.m_DisplayTime;
	if (m_Shot.m_DisplayTime <= 0) {
		m_Shot.m_Display = false;
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
			m_vPosition.y += m_Shot[i].m_Velocity;

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
	if (!m_Shot.m_Display) return;
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) {
			CStaticMeshObject::Draw(View, Proj, Light, Camera);
		}
	}
#endif
}

void CShot::Reload(const D3DXVECTOR3& Pos, float RotY)
{

	//���[���h���W
	SetPosition(Pos);
	SetRotation(D3DXVECTOR3(0.0f, RotY, 0.0f));

	//�i�s�����Ɗe�탊�Z�b�g
	m_Shot.m_MoveDirection = ForwardFromYaw(RotY);
	m_Shot.m_Velocity = 0.0f;
	m_Shot.m_DisplayTime = (m_Shot.m_DisplayTime > 0) ? m_Shot.m_DisplayTime : kDefaultShotLifeFrames;
	m_Shot.m_Display = true;

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) return;

		m_vPosition = Pos;
		m_vRotation.y = RotY;		// �e�̌���(������)���ς���
		m_Shot[i].m_Display = true;
		m_Shot[i].m_Velocity = 0.f;
		m_Shot[i].m_DisplayTime = FPS * 1;


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

bool CShot::IsActive() const
{
	return m_Shot.m_Display && (m_Shot.m_DisplayTime > 0);
}
