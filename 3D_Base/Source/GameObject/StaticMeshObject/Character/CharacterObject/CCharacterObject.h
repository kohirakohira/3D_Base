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

		int		m_MutekiCnt;	// 無敵カウント
		float	m_MutekiTimer;	// 無敵時間

		float	m_RespawnTimer;	// リスポーン時間

		bool	m_Drawflag;		// 描画フラグ
		bool	m_Damage;		// ダメージフラグ
		bool	m_Death;		// 死亡フラグ		
		bool	m_Muteki;		// 無敵フラグ
		bool	m_Respawn;		// リスポーンフラグ
	} m_Chara;

public:
	CCharacterObjectBase();
	virtual ~CCharacterObjectBase();

	//動作関数.
	virtual void Update() override = 0;
	//描画関数.
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override = 0;

	//車体の取得.
	virtual std::shared_ptr<CBody> GetBody() const = 0;
	//砲塔の取得.
	virtual std::shared_ptr<CCannon> GetCannon() const = 0;
	//弾マネージャーの取得.
	virtual std::shared_ptr<CShotManager> GetShotManager() const = 0;

	//砲塔の位置取得.
	virtual D3DXVECTOR3 GetCannonPosition() const = 0;
	virtual float GetCannonYaw() const = 0;

	//プレイヤーかCOMを判定する用.
	virtual bool IsPlayer() const = 0;

	//操作可能かどうか.
	virtual void SetHasControl(bool enable) = 0;
	virtual bool HasControl() const = 0;

	//リスポーンフラグの取得
	virtual bool GetRespawnFlag() const = 0;
	//リスポーンフラグの設定
	virtual void SetRespawnFlag(bool flg) = 0;

	//弾マネージャーのインスタンス設定.
	virtual void SetShotManager(std::shared_ptr<CShotManager> shot) = 0;

	//=====ヒット関数=====
	virtual void Hit() = 0;
	//===================

	//=====ダメージ関数=====
	virtual void Damage() = 0;
	//=====================

	//=====死亡関数=====
	virtual void Death() = 0;
	//=================

	//===ダメージの設定・取得===
	virtual void SetDamage(bool flg) = 0;
	virtual bool GetDamage() const = 0;
	//========================

	//=====死亡の設定・取得=====
	virtual void SetDeath(bool flg) = 0;
	virtual bool GetDeath() const = 0;
	//========================

	//=====無敵の設定・取得=====
	virtual void SetMuteki(bool flg) = 0;
	virtual bool GetMuteki() const = 0;
	//========================

protected:
	//車体クラス.
	std::shared_ptr<CBody>			m_pBody;
	//砲塔クラス.
	std::shared_ptr<CCannon>		m_pCannon;
	//弾マネージャークラス.
	//生成や設定はPlayer/COM側で行う.
	std::shared_ptr<CShotManager>	m_pShotManager;

	//キャラクターの状態.
	bool m_IsActive;

	//キャラクターの生存フラグ.
	bool m_IsAlive;

	//操作権があるか
	bool m_HasControl;	

	//プレイヤーID
	int m_PlayerID;
};