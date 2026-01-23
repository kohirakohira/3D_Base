#include "CBlast.h"

CBlast::CBlast()
	: m_MaxSize				( 4.0f )
	, m_MinimumSize			( 0.0f )
	, m_Radius				( 0.0f )
	, m_BomStart			( false )
	, m_BomFinish			( false )
	, m_Speed				( 0.0f )
	, m_PlayerID			( 0.0f )
{
	//インスタンスの生成.
	m_pCollider = std::make_shared<CSphereCollider>();
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

		//エフェクトの再生.
		if (handle != -1)
		{
			D3DXVECTOR3 scale = { m_Radius * 0.09f, m_Radius * 0.09f, m_Radius * 0.09f };
			CEffect::GetInstance().SetScale(handle, scale);
		}

#else
		m_Radius = m_MaxSize;
#endif
		//メッシュの半径を設定.
		CStaticMeshObject::SetScale(m_Radius);
		if (m_Radius >= m_MaxSize)
		{
			if (handle != -1)
			{
				CEffect::GetInstance().Stop(handle);
				handle = -1;
			}
			m_BomStart = false;
			m_BomFinish = true;
		}
	}
	CStaticMeshObject::Update();
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

//プレイヤーID設定.
void CBlast::SetPlayerID(float id)
{
	m_PlayerID = id;
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

//エフェクトの初期位置を設定.
void CBlast::InitEffectPosition(const D3DXVECTOR3& pos)
{
	handle = CEffect::GetInstance().Play( CEffect::Blast, pos);
}