#include "CCharacterObject.h"

CCharacterObjectBase::CCharacterObjectBase(
	int							hp
)

	: m_HP					( hp )
	, m_MaxHP				( hp )
	, m_pBody				()
	, m_pCannon				()
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_Drawflag			( true  )
	, m_Respawn				( false )
	, m_HasControl			( false )
	, m_Death				( false )
	, m_Damage				( false )
	, m_Muteki				( true )
{
}

CCharacterObjectBase::~CCharacterObjectBase() = default;

void CCharacterObjectBase::Update()
{
	if (m_pBody)
	{
		m_pBody->Update();
	}

	if (m_pCannon)
	{
		m_pCannon->Update();
	}

}

void CCharacterObjectBase::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	// ¶‘¶ƒtƒ‰ƒO‚â•`‰æƒtƒ‰ƒO‚Å‘Šú return
	if (!m_IsAlive || !m_Drawflag) {
		return;
	}

#if 1
	// ŽÔ‘Ì‚Æ–C“ƒ‚ð‚Ü‚Æ‚ß‚Ä•`‰æ
	if (m_pBody) {
		m_pBody->Draw(View, Proj, Light, Camera);
	}
	if (m_pCannon) {
		m_pCannon->Draw(View, Proj, Light, Camera);
	}
#endif
}