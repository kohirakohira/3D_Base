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
	, m_Body				(std::move(body))
	, m_Cannon				(std::move(cannon))
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_Drawflag			( false )
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
	if (m_Body)
	{
		m_Body->Update();
	}

	if (m_Cannon)
	{
		m_Cannon->Update();
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
	if (m_Body) {
		m_Body->Draw(View, Proj, Light, Camera);
	}
	if (m_Cannon) {
		m_Cannon->Draw(View, Proj, Light, Camera);
	}
#endif
}

void CCharacterObjectBase::AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_Body->AttachMesh(pBody);
	m_Cannon->AttachMesh(pCannon);
}

void CCharacterObjectBase::SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_Body->CreateBounding(pBody);
	m_Cannon->CreateBounding(pCannon);
}

void CCharacterObjectBase::CreateCollider()
{
	m_Body->CreateBoxCollider(m_Body->GetMinPos(), m_Body->GetMaxPos());
	m_Cannon->CreateBoxCollider(m_Cannon->GetMinPos(), m_Cannon->GetMaxPos());
}

D3DXVECTOR3 CCharacterObjectBase::GetCannonPosition() const
{
	if (m_Cannon)
	{
		return m_Cannon->GetPosition();
	}
}

float CCharacterObjectBase::GetCannonYaw() const
{
	if (m_Cannon)
	{
		return m_Cannon->GetRotation().y;
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

