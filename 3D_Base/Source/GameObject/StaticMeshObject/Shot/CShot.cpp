#include "CShot.h"

CShot::CShot()
{
	//弾情報の初期化.
	m_Shot = {
		false,
		{0.0f, 0.0f, 0.0f},
		10.0f,
		0,
		-9.8f,
		0.0f
	};
}

CShot::~CShot()
{
}

void CShot::Initialize(int id)
{
}

void CShot::Update()
{
	//時間定数宣言.
	const float TIME = 1.0f / FPS;

	if (m_Shot.m_Display == true)
	{
		// 移動方向に移動速度をかけ合わせたものを座標に反映
		m_vPosition += m_Shot.m_MoveDirection * m_Shot.m_MoveSpeed * TIME;

		if (m_Shot.m_DisplayTime <= 120)
		{
			// 加速度に重力が与えられていく
			m_Shot.m_Velocity += m_Shot.m_Gravity * TIME;
			// 加速度にYを与える
			m_vPosition.y += m_Shot.m_Velocity * TIME;
		}

		m_Shot.m_DisplayTime--;
		if (m_Shot.m_DisplayTime < 0) {
			//見えない所に置いておく
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
	for (int i = 0; i < ShotMax; i++)
	{
		//弾が発射されていたら戻す.
		if (m_Shot.m_Display == true) return;

		m_vPosition = { Pos.x, Pos.y + 0.3f, Pos.z + 1.5f };
		m_vRotation.y = RotY;		// 弾の向き(見た目)も変える
		m_Shot.m_Display = true;
		m_Shot.m_Velocity = 0.f;
		m_Shot.m_DisplayTime = FPS * 3;	//三秒描画.

		// Z軸ベクトル
		m_Shot.m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);

		// Y軸回転行列
		D3DXMATRIX mRotationY;
		// Y軸回転行列を作成
		D3DXMatrixRotationY(
			&mRotationY,	// (out)行列
			m_vRotation.y);	// プレイヤーのY方向の回転値

		// Y軸回転行列を使ってZ軸ベクトルを座標変換する
		D3DXVec3TransformCoord(
			&m_Shot.m_MoveDirection,	// (out)Z軸ベクトル
			&m_Shot.m_MoveDirection,	// (in) Z軸ベクトル
			&mRotationY);		// Y軸回転行列
	}
}

bool CShot::IsActive() const
{
	if (m_Shot.m_Display)
	{
		return true;
	}
	return false;
}
