#include "CBlastCollision.h"

//定数宣言.
const float MAX_RADIUS = 4.0f;		//半径の最大値.
const float MIN_RADIUS = 0.0f;		//半径の最小値.

CBlastCollision::CBlastCollision()
	: m_Radius			( 0.0f )
	, m_Bom				( false )
	, m_RadiusMax		( MAX_RADIUS )
{
	//球の当たり判定.
	m_pCollider = std::make_shared<CSphereCollider>();
}

CBlastCollision::~CBlastCollision()
{
}

//動作処理.
void CBlastCollision::Update()
{
	//定数宣言.
	const float GROWTH_SPEED = 10.0f;	//大きさの上がり幅.
	const float deltaTime = 1.0f / 60.0f;

	// コライダーの座標を更新
	m_pCollider->SetPosition(m_vPosition);

#if 1
	//半径を徐々に大きくする.
	m_Radius += (GROWTH_SPEED * deltaTime);

	//最小値から最大値までしか反映されない.
	m_Radius = std::clamp(m_Radius, MIN_RADIUS, m_RadiusMax);

	//爆発を戻す.
	if (m_Radius >= m_RadiusMax)
	{
		//初期化.
		m_Radius = MIN_RADIUS;
		m_Bom = false;
	}

#else
	//半径を固定.
	m_Radius = MAX_RADIUS;
#endif

	//当たり判定設定.
	SetRadius(m_Radius);
	//半径を設定してあげる.
	std::shared_ptr<CSphereCollider> m_ShereCollider = std::dynamic_pointer_cast<CSphereCollider>(m_pCollider);
	m_ShereCollider->SetRadius(m_Radius);

	std::cout << "半径" << m_Radius << std::endl;
	
}

//描画処理.
void CBlastCollision::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}

//当たった時の関数.
void CBlastCollision::HitBlast()
{
	//初期化.
	m_Radius = 0.0f;
	m_Bom = false;
}