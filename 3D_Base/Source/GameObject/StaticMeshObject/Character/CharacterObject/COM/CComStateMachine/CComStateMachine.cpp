#include "CComStateMachine.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/Util/Util.h"
#include <algorithm>

CComStateMachine::CComStateMachine()
	: m_State(State::Seek)
	, m_StateFrames(0)
{
}

void CComStateMachine::Update()
{
	++m_StateFrames;
}

void CComStateMachine::ChangeState(State newState)
{
	if (m_State != newState)
	{
		m_State = newState;
		m_StateFrames = 0;
	}
}

void CComStateMachine::EvaluateTransitions(float dist2, bool hasTarget, int lostSightFrames)
{
	//‚±‚Ì‹——£ˆÈ“à‚É“ü‚Á‚½‚çAttack‚É“ü‚Á‚Ä‚¢‚¢.max‚Å¬‚³‚­‚È‚è‚·‚¬‚½ê‡‚Ì‰ºŒÀ‚ğİ’è
	const float attackEnter2 = Util::Sqr(std::max(m_Config.keepDistance * m_Config.attackEnterMultiplier, 3.f));

	//Attackó‘Ô‚©‚ç”²‚¯‚é‚Æ‚«‚Ég‚¤‹——£‚Ìè‡’l‚Ì“ñæ
	const float attackExit2 = Util::Sqr(std::max(m_Config.keepDistance * m_Config.attackExitMultiplier, 5.f));

	//‹ß‚·‚¬‚é‚Æ‚İ‚È‚·‹——£
	const float evadeDist2 = Util::Sqr(m_Config.keepDistance * m_Config.evadeMultiplier);

	switch (m_State)
	{
	case State::Seek:
		if (hasTarget)
		{
			ChangeState(State::Chase);
		}
		break;

	case State::Chase:
		if (!hasTarget)
		{
			ChangeState(State::Seek);
			break;
		}

		if (dist2 <= evadeDist2)
		{
			ChangeState(State::Evade);
			break;
		}

		if (dist2 <= attackEnter2)
		{
			ChangeState(State::Attack);
			break;
		}
		break;

	case State::Attack:
		if (!hasTarget)
		{
			ChangeState(State::Seek);
			break;
		}

		if (dist2 < evadeDist2)
		{
			ChangeState(State::Evade);
			break;
		}

		if (dist2 > attackExit2)
		{
			ChangeState(State::Chase);
			break;
		}
		break;

	case State::Evade:
		if (!hasTarget)
		{
			ChangeState(State::Seek);
			break;
		}

		if (dist2 >= attackEnter2)
		{
			ChangeState(State::Chase);
			break;
		}
		else if (dist2 >= evadeDist2)
		{
			ChangeState(State::Attack);
			break;
		}

		if (lostSightFrames > m_Config.loseFrames)
		{
			ChangeState(State::Seek);
		}
		break;
	}
}

const char* CComStateMachine::GetStateName() const
{
	switch (m_State)
	{
	case State::Seek:
		return "Seek";
	case State::Chase:
		return "Chase";
	case State::Attack:
		return "Attack";
	case State::Evade:
		return "Evade";
	default:
		return "Unknown";
	}
}