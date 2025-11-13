#include "CCharacterObject.h"

CCharacterObject::CCharacterObject(
	int							hp,
	const TankTuning&			tuning/*,
	std::shared_ptr<CBody>		body,
	std::shared_ptr<CCannon>	cannon*/
)
	: m_HP					( hp )
	, m_MaxHP				( hp )
	, m_Tuning				( tuning )
	//, m_Body				( body )
	//, m_Cannon				( cannon )
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_Drawflag			( false )
{
}

CCharacterObject::~CCharacterObject()
{
}