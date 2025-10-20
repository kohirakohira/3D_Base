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

	//水平移動
	m_vPosition += m_MoveDirection * m_MoveSpeed;

	////垂直
	//if (m_Gravity != 0.0f)
	//{
	//	m_VelocityY -= m_Gravity;
	//	m_vPosition.y += m_VelocityY;
	//}

	//弾寿命
	if (--m_LifeFrames <= 0) {

		m_Display = false;
	}

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Display == true) {
			// 移動方向に移動速度をかけ合わせたものを座標に反映
			m_vPosition += m_MoveDirection * m_MoveSpeed;

			// 加速度に重力が与えられていく
			m_VelocityY -= m_Gravity;
			// 加速度にYを与える
			m_vPosition.y += m_VelocityY / 2 * m_Gravity;

			m_LifeFramesInit--;
			if (m_LifeFramesInit < 0) {
				//見えない所に置いておく
				m_vPosition = D3DXVECTOR3(0.f, -10.f, 0.f);
				m_Display = false;
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
	//弾を新規発射として初期化
	m_vPosition = Pos;
	m_vRotation.y = RotY;		// 弾の向き(見た目)も変える

	//前方をY回転rotYで回した方向を移動方向にする
	m_MoveDirection = D3DXVECTOR3(std::sinf(RotY), 0.0f, std::cosf(RotY));	//正規化
	m_VelocityY = 0.0f;

	m_LifeFrames = (m_LifeFramesInit > 0) ? m_LifeFramesInit : 120;
	m_Display = true;

#if 0
	for (int i = 0; i < ShotMax; i++)
	{
		if (m_Shot[i].m_Display == true) return;

		m_vPosition = Pos;
		m_vRotation.y = RotY;		// 弾の向き(見た目)も変える
		m_Shot[i].m_Display = true;
		m_Shot[i].m_Velocity = 0.f;
		m_Shot[i].m_DisplayTime = FPS * 3;


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
#endif
}

