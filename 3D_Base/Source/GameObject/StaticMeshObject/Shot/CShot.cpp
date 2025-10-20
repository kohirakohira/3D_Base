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
			m_Display = false;
		}
	}

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
		//弾が発射されていたら戻す.
		if (m_Shot.m_Display == true) return;

		// 回転に応じた発射位置を計算
		D3DXVECTOR3 offset = { 0.f, 0.3f, 1.5f }; // 砲塔の先端に合わせる
		D3DXMATRIX mRotationY;
		// Y軸回転行列を作成
		D3DXMatrixRotationY(
			&mRotationY,	// (out)行列
			m_vRotation.y);	// プレイヤーのY方向の回転値

		// 弾の進行方向（Z軸を回転させる）
		m_Shot.m_MoveDirection = D3DXVECTOR3(0.f, 0.f, 1.f);
		D3DXVec3TransformCoord(&m_Shot.m_MoveDirection, &m_Shot.m_MoveDirection, &mRotationY);
	}
#endif
}

