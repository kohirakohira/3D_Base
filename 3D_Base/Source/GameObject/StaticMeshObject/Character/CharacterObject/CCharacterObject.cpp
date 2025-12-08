#include "CCharacterObject.h"

CCharacterObjectBase::CCharacterObjectBase()
	: m_pBody				()
	, m_pCannon				()
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_HasControl			( false )
	, m_PlayerID			( -1 )
{
	// キャラクター初期値.
	m_Chara = {
		2,		// キャラクターの体力
		2,		// キャラクターの最大体力
		0,		// 無敵カウント
		0.3f,	// 無敵時間
		3.0f,	// リスポーン時間
		true,	// 描画フラグ
		false,	// ダメージフラグ
		false,	// 死亡フラグ	
		false,	// 無敵フラグ
		false,	// リスポーンフラグ
	};
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
	if (!m_IsAlive || !m_Chara.m_Drawflag) {
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