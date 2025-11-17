#include "CCharacterObject.h"

CCharacterObjectBase::CCharacterObjectBase(
	int							hp,
//	const TankTuning&			tuning,
	std::shared_ptr<CBody>		body,
	std::shared_ptr<CCannon>	cannon
)

	: m_HP					( hp )
	, m_MaxHP				( hp )
	//, m_Tuning			( tuning )
	, m_pBody				(std::move(body))
	, m_pCannon				(std::move(cannon))
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_Drawflag			( true  )
	, m_Respawn				( false )
	, m_HasControl			( false )
	, m_Death				( false )
	, m_Damage				( false )
	, m_Tuning				()
{
}

CCharacterObjectBase::~CCharacterObjectBase() = default;


void CCharacterObjectBase::Create(int index)
{
}

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

	CCharacter::Draw(View, Proj, Light, Camera);

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

void CCharacterObjectBase::AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	//m_Body->AttachMesh(pBody);
	//m_Cannon->AttachMesh(pCannon);
}

void CCharacterObjectBase::SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_pBody->CreateBounding(pBody);
	m_pCannon->CreateBounding(pCannon);
}

void CCharacterObjectBase::CreateCollider()
{
	m_pBody->CreateBoxCollider(m_pBody->GetMinPos(), m_pBody->GetMaxPos());
	m_pCannon->CreateBoxCollider(m_pCannon->GetMinPos(), m_pCannon->GetMaxPos());
}

D3DXVECTOR3 CCharacterObjectBase::GetCannonPosition() const
{
	if (m_pCannon)
	{
		return m_pCannon->GetPosition();
	}
}

float CCharacterObjectBase::GetCannonYaw() const
{
	if (m_pCannon)
	{
		return m_pCannon->GetRotation().y;
	}
}

void CCharacterObjectBase::HitPlayer()
{
	m_HP--;
	if (m_HP <= 0)
	{
		//Ž€–Sƒtƒ‰ƒO
		m_Death = true;
	}
	else
	{
		m_Damage = true;
	}
}

