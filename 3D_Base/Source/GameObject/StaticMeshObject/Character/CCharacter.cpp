#include "CCharacter.h"

CCharacter::CCharacter()
	: m_Shot		(false)

	, m_pRayY		( nullptr )
{
	m_pRayY = std::make_shared<RAY>();
	m_pRayY->Axis = D3DXVECTOR3(0.f, -1.f, 0.f);	//‰ºŒü‚«‚ÌŽ².
	m_pRayY->Length = 10.f;		//‚Æ‚è‚ ‚¦‚¸10‚É‚µ‚Ä‚¨‚­.
}

CCharacter::~CCharacter()
{
	//SAFE_DELETE( m_pRayY );
}

void CCharacter::Update()
{
	CStaticMeshObject::Update();
}

void CCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}
