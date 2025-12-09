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
	if (!m_IsAlive || !m_Chara.m_Drawflag) 
	{
		return;
	}

#if 1
	// 車体と砲塔をまとめて描画
	if (m_pBody) 
	{
		m_pBody->Draw(View, Proj, Light, Camera);
	}
	if (m_pCannon)
	{
		m_pCannon->Draw(View, Proj, Light, Camera);
	}
#endif
}

void CCharacterObjectBase::SetShotManager(std::shared_ptr<CShotManager> shot)
{
	m_pShotManager = shot;
	m_pCannon->SetShotManager(m_pShotManager);
}

//=====ヒット関数=====
void CCharacterObjectBase::Hit()
{
	// プレイヤーの体力を引く
	m_Chara.m_Hp--;
	if (m_Chara.m_Hp < 0)
	{
		// 死亡フラグ有効化
		m_Chara.m_Death = true;
	}
	else
	{
		// ダメージフラグ有効化
		m_Chara.m_Damage = true;
	}
}
//===================

//=====ダメージ関数=====
void CCharacterObjectBase::Damage()
{
	const float TIME = 1.0f / FPS;

	if (m_Chara.m_Damage == true)
	{
		m_Chara.m_MutekiTimer -= TIME;

		if (m_Chara.m_MutekiTimer <= 0.0f)
		{
			if (m_Chara.m_Drawflag == true)
			{
				m_Chara.m_Drawflag = false;
			}
			else
			{
				m_Chara.m_Drawflag = true;
			}

			m_Chara.m_MutekiCnt++;
			m_Chara.m_MutekiTimer = 0.2f;
		}

		if (m_Chara.m_MutekiCnt >= 10)
		{
			m_Chara.m_Drawflag = true;
			m_Chara.m_Damage = false;
		}
	}
	else
	{
		m_Chara.m_MutekiCnt = 0;
		m_Chara.m_MutekiTimer = 0.2;
	}
}
//=====================

//=====死亡関数=====
void CCharacterObjectBase::Death()
{
	//時間定数宣言.
	const float TIME = 1.0f / FPS;

	if (m_Chara.m_Death == true)
	{
		// リスポーンタイムを減少
		m_Chara.m_RespawnTimer -= TIME;

		// 描画フラグを無効化
		m_Chara.m_Drawflag = false;

		if (m_Chara.m_RespawnTimer <= 0.0f)
		{
			// Hpを初期化
			m_Chara.m_Hp = m_Chara.m_MaxHp;

			// 描画フラグを有効化
			m_Chara.m_Drawflag = true;

			// リスポーンタイマーを初期化
			m_Chara.m_RespawnTimer = 3.0f;

			// リスポーンフラグ有効化
			m_Chara.m_Respawn = true;

			// 死亡フラグを無効化
			m_Chara.m_Death = false;
		}
	}
}
//=================
