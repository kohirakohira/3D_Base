#include "CCharacterObject.h"

CCharacterObjectBase::CCharacterObjectBase(
	int							hp,
	const TankTuning&			tuning,
	std::shared_ptr<CBody>		body,
	std::shared_ptr<CCannon>	cannon
)
	: m_HP					( hp )
	, m_MaxHP				( hp )
	, m_Tuning				( tuning )
	, m_Body				(std::move(body))
	, m_Cannon				(std::move(cannon))
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_Drawflag			( false )
{
}

CCharacterObjectBase::~CCharacterObjectBase() = default;


void CCharacterObjectBase::Create(int index)
{
	//Šî’ê‚Å‚Í‚È‚É‚à‚µ‚È‚¢.‹ïÛ‚Å¶¬Ó–±‚ðŽ‚½‚¹‚é
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
	if (!m_Drawflag) return;

	if (m_Body)
	{
		m_Body->Draw(View, Proj, Light, Camera);
	}

	if (m_Cannon)
	{
		m_Cannon->Draw(View, Proj, Light, Camera);
	}
}

