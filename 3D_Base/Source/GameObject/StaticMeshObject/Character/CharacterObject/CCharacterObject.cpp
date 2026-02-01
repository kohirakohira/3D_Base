#include "CCharacterObject.h"

CCharacterObjectBase::CCharacterObjectBase()
	: m_pBody				()
	, m_pCannon				()
	, m_IsActive			( false )
	, m_IsAlive				( false )
	, m_HasControl			( false )
	, m_PlayerID			( -1 )
	, m_SoundTimer			( 30.0f )
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
		false,	// すでにキル処理したか	
		false,	// 無敵フラグ
		false,	// リスポーンフラグ

		false,	// サウンドフラグ：壁に当たっている時	
		false,	// サウンドフラグ：箱に当たっている時
		false,	// サウンドフラグ：爆風に当たっている時

		//0.0f,	// サウンドカウント
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
	if (!m_Chara.m_Drawflag) {
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

// サウンドクールダウン
void CCharacterObjectBase::SoundCoolDown()
{
	m_Chara.Soundcount--;
	if (m_Chara.Soundcount <= 0.0f && m_Chara.m_HitBox == true)
	{
		m_Chara.Soundcount = 90.f;
	}
}

// 弾マネージャーのインスタンス設定
void CCharacterObjectBase::SetShotManager(std::shared_ptr<CShotManager> shot)
{
	m_pShotManager = shot;
	m_pCannon->SetShotManager(m_pShotManager);
}

//=====ダメージ関数=====
void CCharacterObjectBase::Damage()
{
	if (m_Chara.m_Muteki == false &&m_Chara.m_Damage == false)
	{
		// プレイヤーの体力を引く
		m_Chara.m_Hp--;
		if (m_Chara.m_Hp <= 0)
		{
			// 死亡フラグ有効化
			m_Chara.m_Death = true;
			m_Chara.m_Kill = false;  // 死亡直後なのでまだ未処理
		}
		else
		{
			// ダメージフラグ有効化
			m_Chara.m_Damage = true;
			// 無敵フラグ有効化
			m_Chara.m_Muteki = true;
		}
	}
}
//=====================

//=====無敵関数=====
void CCharacterObjectBase::Muteki()
{
	//時間定数宣言.
	const float TIME = 1.0f / FPS;

	if (m_Chara.m_Muteki == true)
	{
		// 無敵タイマーを減少
		m_Chara.m_MutekiTimer -= TIME;

		if (m_Chara.m_MutekiTimer <= 0.0f)
		{
			// 描画フラグがtrueの時はfalseに
			// falseの時はtrueにする
			if (m_Chara.m_Drawflag == true)
			{
				m_Chara.m_Drawflag = false;
			}
			else
			{
				m_Chara.m_Drawflag = true;
			}

			// 無敵カウントを1つ増やす
			m_Chara.m_MutekiCnt++;

			// 無敵タイマーを初期化
			m_Chara.m_MutekiTimer = 0.1f;
		}

		if (m_Chara.m_MutekiCnt >= 20)
		{
			// 描画フラグ有効化
			m_Chara.m_Drawflag = true;

			// 無敵フラグを無効化
			m_Chara.m_Muteki = false;

			// ダメージフラグを無効化
			m_Chara.m_Damage = false;
		}
	}
	else
	{
		// 当たるようになったらサウンドフラグ無効化
		m_Chara.m_HitBlast = false;

		// 念のためここでも無敵を初期化する
		m_Chara.m_MutekiCnt = 0;
		m_Chara.m_MutekiTimer = 0.2;
		m_Chara.m_Muteki = false;
	}
}
//=================


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

			// 無敵フラグ有効化
			m_Chara.m_Muteki = true;
		}
	}
}
//=================

//=====位置の設定・取得=====
void CCharacterObjectBase::SetPosition(const D3DXVECTOR3& pos)
{
	if (m_pBody)   m_pBody->SetPosition(pos);
	if (m_pCannon) m_pCannon->SetPosition(pos);
}

D3DXVECTOR3 CCharacterObjectBase::GetPosition() const
{
	if (m_pBody) return m_pBody->GetPosition();
	return D3DXVECTOR3(0, 0, 0);
}
//========================

//=====回転の設定・取得=====
void CCharacterObjectBase::SetRotation(const D3DXVECTOR3& rot)
{
	if (m_pBody) m_pBody->SetRotation(rot);
}

D3DXVECTOR3 CCharacterObjectBase::GetRotation() const
{
	if (m_pBody) return m_pBody->GetRotation();
	return D3DXVECTOR3(0, 0, 0);
}
//========================

// サウンドフラグのリセット
void CCharacterObjectBase::ResetSoundFlg()
{
	if (m_Chara.m_HitWall == true || m_Chara.m_HitBox == true)
	{
		//m_SoundTimer--;
		//if (m_SoundTimer <= 0.0f)
		//{
		//	m_Chara.m_HitWall = false;
		//	m_Chara.m_HitBox = false;

		//	m_SoundTimer = 30.f;
		//}
	}
}
