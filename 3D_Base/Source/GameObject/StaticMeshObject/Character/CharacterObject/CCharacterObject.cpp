#include "CCharacterObject.h"

CCharacterObjectBase::CCharacterObjectBase(
	int							hp
)
	: m_pBody				()
	, m_pCannon				()
	, m_PlayerID			( -1 )
{
	//キャラクター共通処理初期値
	m_Character.m_HP			= hp;
	m_Character.m_MaxHP			= hp;
	m_Character.m_IsActive		= false;
	m_Character.m_IsAlive		= false;
	m_Character.m_Drawflag		= true;
	m_Character.m_Respawn		= false;
	m_Character.m_HasControl	= false;
	m_Character.m_Death			= false;
	m_Character.m_Damage		= false;
	m_Character.m_MutekiTimer	= 0.2f;
	m_Character.m_Muteki		= false;
	m_Character.m_MutekiCnt		= 0.f;
	m_Character.m_RespawnTimer	= 0.f;
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
	// 生存フラグや描画フラグで早期 return
	if (!m_Character.m_IsAlive || m_Character.m_Drawflag == false) {
		return;
	}

#if 1
	// 車体と砲塔をまとめて描画
	if (m_pBody) {
		m_pBody->Draw(View, Proj, Light, Camera);
	}
	if (m_pCannon) {
		m_pCannon->Draw(View, Proj, Light, Camera);
	}
#endif
}