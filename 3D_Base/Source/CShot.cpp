#include "CShot.h"
#include <cmath>

static constexpr int kDefaultShotLifeFrames = 180; //3秒間

//yowから前方向の単位ベクトルをつくる
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

	//既定は非表示・寿命0
	m_Shot = Shot{};
	m_Shot.m_Display = false;
	m_Shot.m_DisplayTime = 0;
}

void CShot::Update()
{
	if (!m_Shot.m_Display) return;

	//平面移動
	D3DXVECTOR3 pos = GetPosition();
	pos += m_Shot.m_MoveDirection * m_Shot.m_MoveSpeed;

	//重力
	if (m_Shot.m_Gravity != 0.0f)
	{
		//下向きに落とす設計
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

	//ワールド座標
	SetPosition(Pos);
	SetRotation(D3DXVECTOR3(0.0f, RotY, 0.0f));

	//進行方向と各種リセット
	m_Shot.m_MoveDirection = ForwardFromYaw(RotY);
	m_Shot.m_Velocity = 0.0f;
	m_Shot.m_DisplayTime = (m_Shot.m_DisplayTime > 0) ? m_Shot.m_DisplayTime : kDefaultShotLifeFrames;
	m_Shot.m_Display = true;

#if 0
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
#endif
}

bool CShot::IsActive() const
{
	return m_Shot.m_Display && (m_Shot.m_DisplayTime > 0);
}
