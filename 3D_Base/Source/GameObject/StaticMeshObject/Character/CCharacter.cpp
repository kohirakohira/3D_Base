#include "CCharacter.h"

CCharacter::CCharacter()
	: m_pRayY		( nullptr )
{
	//‰Šú’l‚Ìİ’è.
	SettingTune();
	m_pRayY = std::make_shared<RAY>();
	m_pRayY->Axis = D3DXVECTOR3(0.f, -1.f, 0.f);	//‰ºŒü‚«‚Ì².
	m_pRayY->Length = 10.f;		//‚Æ‚è‚ ‚¦‚¸10‚É‚µ‚Ä‚¨‚­.
}

CCharacter::~CCharacter()
{
}

void CCharacter::Update()
{
	CStaticMeshObject::Update();
}

void CCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}

#if 0
void CCharacter::SettingTune()
{
	m_Tune = { 0.08f, 0.03f, 0.03f, 0.3f };
}
#endif