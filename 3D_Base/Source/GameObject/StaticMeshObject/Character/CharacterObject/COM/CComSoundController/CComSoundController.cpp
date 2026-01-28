#include "CComSoundController.h"
#include "Assets/Sound/CSoundManager.h"

CComSoundController::CComSoundController()
	: m_PlayerID (0)
	, m_WasMoving(false)
{
}

void CComSoundController::UpdateMoveSound(bool IsMoving)
{
	//ó‘Ô‚ª•Ï‚í‚Á‚½‚¾‚¯ˆ—
	if (IsMoving == m_WasMoving)
	{
		return;
	}

	if (IsMoving)
	{
		switch (m_PlayerID)
		{
		case 0: CSoundManager::PlayLoop(CSoundManager::SE_Move1); break;
		case 1: CSoundManager::PlayLoop(CSoundManager::SE_Move2); break;
		case 2: CSoundManager::PlayLoop(CSoundManager::SE_Move3); break;
		case 3: CSoundManager::PlayLoop(CSoundManager::SE_Move4); break;
		}
	}
	else
	{
		switch (m_PlayerID)
		{
		case 0: CSoundManager::Stop(CSoundManager::SE_Move1); break;
		case 1: CSoundManager::Stop(CSoundManager::SE_Move2); break;
		case 2: CSoundManager::Stop(CSoundManager::SE_Move3); break;
		case 3: CSoundManager::Stop(CSoundManager::SE_Move4); break;
		}
	}
	//¡‚Ìó‘Ô‚ğ‘OƒtƒŒ[ƒ€ó‘Ô‚Æ‚µ‚Ä•Û‘¶
	m_WasMoving = IsMoving;
}