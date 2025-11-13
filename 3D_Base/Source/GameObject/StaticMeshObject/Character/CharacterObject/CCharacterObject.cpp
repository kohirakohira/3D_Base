#include "CCharacterObject.h"

CCharacterObjectBase::CCharacterObjectBase(
	int							hp,
	const TankTuning&			tuning,
	std::shared_ptr<CBody>		body,
	std::shared_ptr<CCannon>	cannon)
	: m_HP					( hp )
	, m_MaxHP				( hp )
	, m_pBody				( body )
	, m_pCannon				( cannon )
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_Drawflag			( false )
{
}

CCharacterObjectBase::~CCharacterObjectBase()
{
}

void CCharacterObjectBase::SetTuning(const TankTuning& tuning)
{
}
