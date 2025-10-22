#include "CBlastCollision.h"

CBlastCollision::CBlastCollision()
	: m_Angle			( 0.0f )
	, m_Radius			( 0.0f )
{
	//‹…‚Ì“–‚½‚è”»’è.
	m_pCollider = std::make_shared<CSphereCollider>();
	//”¼Œa‚ğİ’è‚µ‚Ä‚©‚ç“–‚½‚è”»’è‚ğì¬.
	CreateSpehreCollider(m_Radius);
}

CBlastCollision::~CBlastCollision()
{
}

//“®ìˆ—.
void CBlastCollision::Update()
{
	//’è”éŒ¾.
	const float PI = 3.141592f;

	//”¼Œa‚ğ™X‚É‘å‚«‚­‚·‚é.
	m_Angle += 1.0f;

	//”¼Œa‚ğƒ‰ƒWƒAƒ“’l‚É•ÏŠ·‚·‚é.
	m_Radius = m_Angle * PI / 180.0f;

	//”¼Œa‚ğí‚Éİ’è‚µ‘±‚¯‚é.
	CStaticMeshObject::SetRadius(m_Radius);
}

//•`‰æˆ—.
void CBlastCollision::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}
