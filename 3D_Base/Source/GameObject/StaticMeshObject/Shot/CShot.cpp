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

	//…•½ˆÚ“®
	m_vPosition += m_MoveDirection * m_MoveSpeed;

	////‚’¼
	//if (m_Gravity != 0.0f)
	//{
	//	m_VelocityY -= m_Gravity;
	//	m_vPosition.y += m_VelocityY;
	//}

	//’eŽõ–½
	if (--m_LifeFrames <= 0) {
		m_Display = false;
	}

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) {
			// ˆÚ“®•ûŒü‚ÉˆÚ“®‘¬“x‚ð‚©‚¯‡‚í‚¹‚½‚à‚Ì‚ðÀ•W‚É”½‰f
			m_vPosition += m_Shot[i].m_MoveDirection * m_Shot[i].m_MoveSpeed;

			// ‰Á‘¬“x‚Éd—Í‚ª—^‚¦‚ç‚ê‚Ä‚¢‚­
			m_Shot[i].m_Velocity -= m_Shot[i].m_Gravity;
			// ‰Á‘¬“x‚ÉY‚ð—^‚¦‚é
			m_vPosition.y += m_Shot[i].m_Velocity / 2 * m_Shot[i].m_Gravity;

			m_Shot[i].m_DisplayTime--;
			if (m_Shot[i].m_DisplayTime < 0) {
				//Œ©‚¦‚È‚¢Š‚É’u‚¢‚Ä‚¨‚­
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
	//’e‚ðV‹K”­ŽË‚Æ‚µ‚Ä‰Šú‰»
	m_vPosition = Pos;
	m_vRotation.y = RotY;		// ’e‚ÌŒü‚«(Œ©‚½–Ú)‚à•Ï‚¦‚é

	//‘O•û‚ðY‰ñ“]rotY‚Å‰ñ‚µ‚½•ûŒü‚ðˆÚ“®•ûŒü‚É‚·‚é
	m_MoveDirection = D3DXVECTOR3(std::sinf(RotY), 0.0f, std::cosf(RotY));	//³‹K‰»
	m_VelocityY = 0.0f;

	m_LifeFrames = (m_LifeFramesInit > 0) ? m_LifeFramesInit : 120;
	m_Display = true;

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) return;

		m_vPosition = Pos;
		m_vRotation.y = RotY;		// ’e‚ÌŒü‚«(Œ©‚½–Ú)‚à•Ï‚¦‚é
		m_Shot[i].m_Display = true;
		m_Shot[i].m_Velocity = 0.f;
		m_Shot[i].m_DisplayTime = FPS * 3;


		// ZŽ²ƒxƒNƒgƒ‹
		m_Shot[i].m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);

		// YŽ²‰ñ“]s—ñ
		D3DXMATRIX mRotationY;
		// YŽ²‰ñ“]s—ñ‚ðì¬
		D3DXMatrixRotationY(
			&mRotationY,	// (out)s—ñ
			m_vRotation.y);	// ƒvƒŒƒCƒ„[‚ÌY•ûŒü‚Ì‰ñ“]’l

		// YŽ²‰ñ“]s—ñ‚ðŽg‚Á‚ÄZŽ²ƒxƒNƒgƒ‹‚ðÀ•W•ÏŠ·‚·‚é
		D3DXVec3TransformCoord(
			&m_Shot[i].m_MoveDirection,	// (out)ZŽ²ƒxƒNƒgƒ‹
			&m_Shot[i].m_MoveDirection,	// (in) ZŽ²ƒxƒNƒgƒ‹
			&mRotationY);		// YŽ²‰ñ“]s—ñ
	}
#endif
}

