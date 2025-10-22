#include "CBlastCollision.h"

CBlastCollision::CBlastCollision()
	: m_Radius			( 0.0f )
	, m_Bom				( false )
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
	const float GROWTH_SPEED = 10.0f;	//‘å‚«‚³‚Ìã‚ª‚è•.
	const float MAX_RADIUS = 5.0f;		//”¼Œa‚ÌÅ‘å’l.
	const float MIN_RADIUS = 0.0f;		//”¼Œa‚ÌÅ¬’l.
	const float deltaTime = 1.0f / FPS;

	if (m_Bom == true)
	{
#if 1
		//”¼Œa‚ğ™X‚É‘å‚«‚­‚·‚é.
		m_Radius += GROWTH_SPEED * deltaTime;

		//Å¬’l‚©‚çÅ‘å’l‚Ü‚Å‚µ‚©”½‰f‚³‚ê‚È‚¢.
		m_Radius = std::clamp(m_Radius, MIN_RADIUS, MAX_RADIUS);

		//”š”­‚ğ–ß‚·.
		if (m_Radius >= MAX_RADIUS)
		{
			//‰Šú‰».
			m_Radius = MIN_RADIUS;
			m_Bom = false;
		}
#else
		//”¼Œa‚ğŒÅ’è.
		m_Radius = MAX_RADIUS;
#endif
		//”¼Œa‚ğí‚Éİ’è‚µ‘±‚¯‚é.
		CStaticMeshObject::SetRadius(m_Radius);

	}
	else
	{
		return;
	}
}

//•`‰æˆ—.
void CBlastCollision::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}
