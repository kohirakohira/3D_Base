#include "CShot.h"

CShot::CShot()
	: m_Shot()
{
}

CShot::~CShot()
{
}

void CShot::Initialize(int id)
{
	for (int i = 0; i < ShotMax; i++)
	{
		m_Shot[i].m_Display = false;
	}
}

void CShot::Update()
{
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) {
			// 移動方向に移動速度をかけ合わせたものを座標に反映
			m_vPosition += m_Shot[i].m_MoveDirection * m_Shot[i].m_MoveSpeed;

			// 加速度に重力が与えられていく
			m_Shot[i].m_Velocity -= m_Shot[i].m_Gravity;
			// 加速度にYを与える
			m_vPosition.y += m_Shot[i].m_Velocity;

			m_Shot[i].m_DisplayTime--;
			if (m_Shot[i].m_DisplayTime < 0) {
				//見えない所に置いておく
				m_vPosition = D3DXVECTOR3(0.f, -10.f, 0.f);
				m_Shot[i].m_Display = false;
			}
		}
	}
}

void CShot::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) {
			CStaticMeshObject::Draw(View, Proj, Light, Camera);
		}
	}
}

void CShot::Reload(const D3DXVECTOR3& Pos, float RotY)
{
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) return;

		m_vPosition = Pos;
		m_vRotation.y = RotY;		// 弾の向き(見た目)も変える
		m_Shot[i].m_Display = true;
		m_Shot[i].m_Velocity = 0.f;
		m_Shot[i].m_DisplayTime = FPS * 1;


		// Z軸ベクトル
		m_Shot[i].m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);

		// Y軸回転行列
		D3DXMATRIX mRotationY;
		// Y軸回転行列を作成
		D3DXMatrixRotationY(
			&mRotationY,	// (out)行列
			m_vRotation.y);	// プレイヤーのY方向の回転値

		// Y軸回転行列を使ってZ軸ベクトルを座標変換する
		D3DXVec3TransformCoord(
			&m_Shot[i].m_MoveDirection,	// (out)Z軸ベクトル
			&m_Shot[i].m_MoveDirection,	// (in) Z軸ベクトル
			&mRotationY);		// Y軸回転行列
	}
}

bool CShot::IsActive() const
{
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display) return true;
	}
	return false;
}
