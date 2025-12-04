#include "CBlastCollision.h"

CBlastCollision::CBlastCollision()
	: m_Radius		( 0.0f )
	, m_Bom			( false )
{
	m_RadiusMax = MAX_RADIUS;
	//‹…‚Ì“–‚½‚è”»’è.
	m_pCollider = std::make_shared<CSphereCollider>();
}

CBlastCollision::~CBlastCollision()
{
}

//“®ìˆ—.
void CBlastCollision::Update()
{
	//’è”éŒ¾.
	const float GROWTH_SPEED = 10.0f;	//‘å‚«‚³‚Ìã‚ª‚è•.
	const float deltaTime = 1.0f / 60.0f;

#if 1
	//”¼Œa‚ğ™X‚É‘å‚«‚­‚·‚é.
	m_Radius += (GROWTH_SPEED * deltaTime);

	//Å¬’l‚©‚çÅ‘å’l‚Ü‚Å‚µ‚©”½‰f‚³‚ê‚È‚¢.
	m_Radius = std::clamp(m_Radius, MIN_RADIUS, m_RadiusMax);

	//”š”­‚ğ–ß‚·.
	if (m_Radius >= m_RadiusMax)
	{
		//‰Šú‰».
		m_Radius = MIN_RADIUS;
		m_Bom = false;
	}

#else
	//”¼Œa‚ğŒÅ’è.
	m_Radius = MAX_RADIUS;
#endif

	//“–‚½‚è”»’èİ’è.
	m_pCollider->SetRadius(m_Radius);
	m_pCollider->SetPosition(m_vPosition);

	//”¼Œa‚ğİ’è‚µ‚Ä‚ ‚°‚é.
	std::shared_ptr<CSphereCollider> m_ShereCollider = std::dynamic_pointer_cast<CSphereCollider>(m_pCollider);
	m_ShereCollider->SetRadius(m_Radius);

	CStaticMeshObject::Update();

}

//•`‰æˆ—.
void CBlastCollision::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (m_Bom == true)
	{
		CStaticMeshObject::Draw(View, Proj, Light, Camera);
	}
}

//“–‚½‚Á‚½‚ÌŠÖ”.
void CBlastCollision::HitBlast()
{
	//‰Šú‰».
	m_Radius = 0.0f;
	m_Bom = false;
}