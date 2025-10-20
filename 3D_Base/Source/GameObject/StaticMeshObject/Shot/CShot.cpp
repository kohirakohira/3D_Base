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

	m_pCollider = std::make_shared<CSphereCollider>();
}

CShot::~CShot()
{
}

void CShot::Initialize(int id)
{
}

void CShot::Update()
{
	// コライダーの座標を更新
	m_pCollider->SetPosition(m_vPosition);

	//時間定数宣言.
	const float TIME = 1.0f / FPS;

	if (m_Shot.m_Display == true)
	{
		// 表示時間を減少させる
		m_Shot.m_DisplayTime--;

		// 移動方向に移動速度をかけ合わせたものを座標に反映
		m_vPosition += m_Shot.m_MoveDirection * m_Shot.m_MoveSpeed * TIME;

		if (m_Shot.m_DisplayTime <= 120)
		{
			// 加速度に重力が与えられていく
			m_Shot.m_Velocity += m_Shot.m_Gravity * TIME;
			// 加速度にYを与える
			m_vPosition.y += m_Shot.m_Velocity * TIME;
		}

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

		// 回転に応じた発射位置を計算
		D3DXVECTOR3 offset = { 0.f, 0.3f, 1.5f }; // 砲塔の先端に合わせる
		D3DXMATRIX mRotationY;
		D3DXMatrixRotationY(&mRotationY, RotY);

		D3DXVECTOR3 rotatedOffset;
		D3DXVec3TransformCoord(&rotatedOffset, &offset, &mRotationY);

		// プレイヤー位置に加算して弾の位置決定
		m_vPosition = Pos + rotatedOffset;

		// 弾の回転（見た目用）
		m_vRotation.y = RotY;

		m_Shot.m_Display = true;
		m_Shot.m_Velocity = 0.5f;
		m_Shot.m_DisplayTime = FPS * 3;

		// 弾の進行方向（Z軸を回転させる）
		m_Shot.m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);
		D3DXVec3TransformCoord(&m_Shot.m_MoveDirection, &m_Shot.m_MoveDirection, &mRotationY);
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
