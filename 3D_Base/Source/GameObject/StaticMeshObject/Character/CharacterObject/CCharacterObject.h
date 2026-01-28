#pragma once
//STL.
#include <iostream>
#include <memory>

//-----継承するクラス-----
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

//車体と砲塔クラス.
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"

//弾マネージャークラス.
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"

//ショットクラス
#include "GameObject/StaticMeshObject/Shot/CShot.h"

//================================================================
//	キャラクターオブジェクト※基底クラス(キャラクターを継承).
//================================================================
class CCharacterObjectBase
	: public CStaticMeshObject // スタティックメッシュオブジェクトクラスを継承.
{
public:

	// キャラクター構造体を作成
	struct Character
	{
		int		m_Hp;			// プレイヤーのHP
		int		m_MaxHp;		// プレイヤーの最大HP

		float   m_MoveSpeed;	// 移動速度

		int		m_MutekiCnt;	// 無敵カウント
		float	m_MutekiTimer;	// 無敵時間

		float	m_RespawnTimer;	// リスポーン時間

		bool	m_Drawflag;		// 描画フラグ
		bool	m_Damage;		// ダメージフラグ
		bool	m_Death;		// 死亡フラグ		
		bool	m_Kill;			// すでにキル処理したか		
		bool	m_Muteki;		// 無敵フラグ
		bool	m_Respawn;		// リスポーンフラグ

		bool	m_HitWall;		// サウンドフラグ：壁に当たっている時
		bool	m_HitBox;		// サウンドフラグ：箱に当たっている時
		bool	m_HitBlast;		// サウンドフラグ：爆風に当たっている時
		bool	m_HitCharacter;	// サウンドフラグ：キャラクターに当たっている時
		bool	m_Move;			// サウンドフラグ：キャラクターが移動している時
				
		float Soundcount = 0.0f;// サウンドカウント

	} m_Chara;

public:
	CCharacterObjectBase();
	virtual ~CCharacterObjectBase();

	//動作関数.
	virtual void Update() override = 0;
	//描画関数.
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override = 0;

	// 車体の取得
	virtual std::shared_ptr<CBody> GetBody() const { return m_pBody; }
	// 砲塔の取得
	virtual std::shared_ptr<CCannon> GetCannon() const { return m_pCannon; }

	// 移動速度の設定・取得
	virtual void SetMoveSpeed(float speed) { m_Chara.m_MoveSpeed = speed; }
	virtual float GetMoveSpeed() { return m_Chara.m_MoveSpeed; }

	// サウンドクールダウン
	virtual void SoundCoolDown();
	// サウンドカウントの設定
	virtual void SetSoundCount(float soundcount) { m_Chara.Soundcount = soundcount; }
	// サウンドカウントの取得
	virtual float GetSoundCount() const { return m_Chara.Soundcount; }

	//プレイヤーかCOMを判定する用.
	virtual bool IsPlayer() const = 0;

	//操作可能かどうか.
	virtual void SetHasControl(bool enable) = 0;
	virtual bool HasControl() const = 0;

	// リスポーンフラグの設定
	virtual void SetRespawnFlag(bool flg) { m_Chara.m_Respawn = flg; }
	// リスポーンフラグの取得
	virtual bool GetRespawnFlag() const { return m_Chara.m_Respawn; }

	// 弾マネージャーのインスタンス設定.
	virtual void SetShotManager(std::shared_ptr<CShotManager> shot);

	//=====ダメージ関数=====
	virtual void Damage();
	//=====================
	
	//=====無敵関数=====
	virtual void Muteki();
	//=================

	//=====死亡関数=====
	virtual void Death();
	//=================

	//===ダメージの設定・取得===
	virtual void SetDamage(bool flg) { m_Chara.m_Damage = flg; }
	virtual bool GetDamage() const { return m_Chara.m_Damage; }
	//========================

	//=====死亡の設定・取得=====
	virtual void SetDeath(bool flg) { m_Chara.m_Death = flg; }
	virtual bool GetDeath() const { return m_Chara.m_Death; }
	//========================

	//=====キル処理の設定・取得=====
	virtual void SetKill(bool flg) { m_Chara.m_Kill = flg; }
	virtual bool GetKill() const { return m_Chara.m_Kill; }
	//========================

	//=====無敵の設定・取得=====
	virtual void SetMuteki(bool flg) { m_Chara.m_Muteki = flg; }
	virtual bool GetMuteki() const { return m_Chara.m_Muteki; }
	//========================

	//=====サウンドフラグの設定・取得=====
	virtual void SetHitWall (bool flg)	{ m_Chara.m_HitWall = flg; }
	virtual void SetHitBox  (bool flg)	{ m_Chara.m_HitBox = flg; }
	virtual void SetHitBlast(bool flg)	{ m_Chara.m_HitBlast = flg; }
	virtual void SetHitCharacter(bool flg)	{ m_Chara.m_HitCharacter = flg; }
	virtual void SetMove(bool flg)	{ m_Chara.m_Move = flg; }

	virtual bool GetHitWall () const { return m_Chara.m_HitWall; }
	virtual bool GetHitBox  () const { return m_Chara.m_HitBox; }
	virtual bool GetHitBlast() const { return m_Chara.m_HitBlast; }
	virtual bool GetHitCharacter() const { return m_Chara.m_HitCharacter; }
	virtual bool GetMove() const { return m_Chara.m_Move; }
	//========================

	//=====位置の設定・取得=====
	virtual void SetPosition(const D3DXVECTOR3& pos);
	virtual D3DXVECTOR3 GetPosition() const;
	//========================

	//=====回転の設定・取得=====
	virtual void SetRotation(const D3DXVECTOR3& rot);
	virtual D3DXVECTOR3 GetRotation() const;
	//========================

	//プレイヤーのインデックス番号の取得.
	virtual int GetPlayerID() = 0;

	//体力取得
	virtual int GetHP() const{ return m_Chara.m_Hp; }

	//体力最大値取得.
	virtual int GetMaxHP() const { return m_Chara.m_MaxHp; }

	// サウンドフラグ初期化
	virtual void ResetSoundFlg();

	// 当たり判定の切り替え関数
	virtual void ChangeCollision(const D3DXVECTOR3& rot);

	// 当たり判定フラグの取得
	virtual bool GetCollisionFlg() const { return m_CollisionFlg; }

protected:
	//車体クラス.
	std::shared_ptr<CBody>			m_pBody;
	//砲塔クラス.
	std::shared_ptr<CCannon>		m_pCannon;
	//弾マネージャークラス.
	std::shared_ptr<CShotManager>	m_pShotManager;

	//キャラクターの状態.
	bool m_IsActive;

	//キャラクターの生存フラグ.
	bool m_IsAlive;

	//操作権があるか
	bool m_HasControl;	

	//プレイヤーID
	int m_PlayerID;

	// サウンドタイマー
	float m_SoundTimer;

	// 当たり判定用フラグ
	bool m_CollisionFlg;

};