#include "CBlastCollision.h"

//’è”éŒ¾.
const float MAX_RADIUS = 2.5f;		//”¼Œa‚ÌÅ‘å’l.
const float MIN_RADIUS = 0.0f;		//”¼Œa‚ÌÅ¬’l.

CBlastCollision::CBlastCollision()
	: m_Radius			( 0.0f )
	, m_Bom				( false )
{
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
	const float GROWTH_SPEED = 6.0f;	//‘å‚«‚³‚Ìã‚ª‚è•.
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

		//“–‚½‚è”»’èİ’è.
		SetRadius(m_Radius);
		//”¼Œa‚ğİ’è‚µ‚Ä‚ ‚°‚é.
		std::shared_ptr<CSphereCollider> m_ShereCollider = std::dynamic_pointer_cast<CSphereCollider>(m_pCollider);
		m_ShereCollider->SetRadius(m_Radius);
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

//“–‚½‚Á‚½‚ÌŠÖ”.
void CBlastCollision::HitBlast()
{
	//‰Šú‰».
	m_Radius = 0.0f;
	m_Bom = false;
}