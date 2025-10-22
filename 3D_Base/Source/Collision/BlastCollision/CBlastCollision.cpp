#include "CBlastCollision.h"

CBlastCollision::CBlastCollision()
	: m_Radius			( 0.0f )
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
	const float GROWTH_SPEED = 0.5f;	//‘å‚«‚³‚Ì•.
	const float MAX_RADIUS = 50.0f;	//”¼Œa‚ÌÅ‘å’l.
	const float MIN_RADIUS = 0.0f;	//”¼Œa‚ÌÅ¬’l.

	//”¼Œa‚ğ™X‚É‘å‚«‚­‚·‚é.
	m_Radius += GROWTH_SPEED;

	//Å¬’l‚©‚çÅ‘å’l‚Ü‚Å‚µ‚©”½‰f‚³‚ê‚È‚¢.
	m_Radius = std::clamp(m_Radius, MIN_RADIUS, MAX_RADIUS);

	//”¼Œa‚ğí‚Éİ’è‚µ‘±‚¯‚é.
	CStaticMeshObject::SetRadius(m_Radius);
}

//•`‰æˆ—.
void CBlastCollision::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}
