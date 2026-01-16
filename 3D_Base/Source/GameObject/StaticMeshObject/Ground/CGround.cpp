#include "CGround.h"

CGround::CGround()
	: m_pCharacterManager	()
	, m_Speed			( 0.0f )
{
}

CGround::~CGround()
{
}

void CGround::Update()
{
}

//‰Šú‰».
void CGround::Init()
{
	//‰Šú‰».
	m_pCharacterManager = nullptr;
	m_Speed = 0.0f;
}

//’n–Ê‚Ì“®‚«.
void CGround::Move(float speed)
{
	//ŠÔ’è”.
	const float dt = 1.0f / FPS;

	//‘¬“xİ’è.
	m_Speed = speed;

	//Z•ûŒü‚É“®‚©‚·.
	m_vPosition.z += m_Speed * dt;

}