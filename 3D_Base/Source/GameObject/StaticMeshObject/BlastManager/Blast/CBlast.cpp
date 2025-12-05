#include "CBlast.h"

CBlast::CBlast()
	: m_MaxSize				( 4.0f )
	, m_MinimumSize			( 0.0f )
	, m_Radius				( 0.0f )
	, m_BomStart			( false )
	, m_BomFinish			( false )
	, m_Speed				( 0.0f )
{
	//インスタンスの生成.
	m_pCollider = std::make_shared<CBoxCollider>();
}

CBlast::~CBlast()
{
}

//更新処理.
void CBlast::Update()
{
	//デルタタイム.
	constexpr float dt = 1.0f / FPS;
	//半径の拡大速度(dt * 秒数).
	const float RAD_SPEED = dt * m_Speed;

	if (m_BomStart == true)
	{
#if 1
		//半径を拡大.
		m_Radius += RAD_SPEED;

		//半径の上限を設定.
		m_Radius = std::clamp(m_Radius, m_MinimumSize, m_MaxSize);
#else
		m_Radius = m_MaxSize;
#endif
		//メッシュの半径を設定.
		CStaticMeshObject::SetScale(m_Radius);
		//当たり判定の半径を設定.
		m_pCollider->SetRadius(m_Radius);
		if (m_Radius >= m_MaxSize)
		{
			m_BomStart = false;
			m_BomFinish = true;
		}
	}
}

//描画処理.
void CBlast::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}

//半径の最大サイズを設定.
void CBlast::SetMaxRadius(float rad)
{
	m_MaxSize = rad;
}

//秒数の設定.
void CBlast::SetSpeed(float s)
{
	m_Speed = s;
}

//爆発フラグの設定.
void CBlast::SetBomStart(bool f)
{
	m_BomStart = f;
}

//爆発の開始フラグを取得.
bool CBlast::IsBomStart() const
{
	return m_BomStart;
}

//爆発の終了フラグを取得.
bool CBlast::IsBomFinish() const
{
	return m_BomFinish;
}
