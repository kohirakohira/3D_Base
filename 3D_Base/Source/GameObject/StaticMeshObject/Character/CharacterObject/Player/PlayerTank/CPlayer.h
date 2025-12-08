#pragma once
#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h" // 継承 || キャラクタークラス

//-----ライブラリ-----
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <Windows.h>

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//CharacterObject\\Player//PlayerTank//TankBody//CBody.h"		// 戦車：車体クラス
#include "GameObject//StaticMeshObject//Character//CharacterObject\\Player//PlayerTank//TankCannon//CCannon.h"	// 戦車：砲塔クラス

//コントローラークラス.
#include "InputDevice/Input/Controller/ControllerManager/CControllerManager.h"
//キークラス.
#include "InputDevice/Key/CMultiInputManager/CMultiInputKeyManager.h"

class CPlayer
	: public CCharacterObjectBase
{
public:

	//入力処理構造体.
	struct PlayerInput
	{
		CController::Direction moveDir;		//左スティック・WASDの方向.
		CController::Direction turretDir;	//右スティックの方向.
		bool shot;							//発射ボタン.
	};

public:
	//内部でBody・Cannonをまとめる関数.
	void Move(const PlayerInput& input);
	void Reload(const D3DXVECTOR3& pos, float y);

public:
	CPlayer();
	~CPlayer() override;

	void Init(int id);

	// 更新関数
	void Update() override;
	// 描画関数
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	// 外部のクラスに情報を渡す
	std::shared_ptr<CBody> GetBody() const override { return m_pBody; }
	std::shared_ptr<CCannon> GetCannon() const override { return m_pCannon; }
	std::shared_ptr<CShotManager>GetShotManager() const override { return ; }

	//砲塔の位置取得.
	D3DXVECTOR3 GetCannonPosition() const { return m_pCannon->GetPosition(); }
	float GetCannonYaw() const { return m_pCannon->GetRotation().y; }

	//当たった時用.
	void OnHit(CCharacterObjectBase* other);

	// プレイヤーかCOMを識別
	bool IsPlayer() const override { return true; }

	//操作可能かどうか.
	void SetHasControl(bool enable) { m_HasControl = enable; }
	bool HasControl() const { return m_HasControl; }

	// インスタンス生成関数
	void Create();

	//プレイヤーが壁に当たる処理をまとめる.
	void SetPushBack(const D3DXVECTOR3& push);

	// コライダーの作成
	void CreateCollider();

	// PlayerIDをCOMに渡す
	int GetPlayerID() const { return m_PlayerID; }

	//リスポーンフラグの設定継承版
	void SetRespawnFlag(bool flg) override { m_Chara.m_Respawn = flg; }
	// リスポーンフラグの取得
	bool GetRespawnFlag() const override { return m_Chara.m_Respawn; }

	// プレイヤーのコントローラー設定・取得
	void SetControllerIndex(int index);
	int GetControllerIndex() const { return m_ControllerIndex; }

	static void DebugLog(const char* msg){OutputDebugStringA(msg);}

	void RotateTurretByPad();

	void UpdateHumanInputAndMove(PlayerInput input);	//プレイヤー処理をいれておく

	// 砲塔と車体の同期
	void SyncCannonToBody();

	//弾マネージャーの設定.
	void SetShotManager(std::shared_ptr<CShotManager> shot) override ;

	//=====ヒット関数=====
	virtual void Hit() override;
	//===================

	//=====ダメージ関数=====
	void Damage() override;
	//=====================
	
	//=====死亡関数=====
	void Death() override;
	//=================

	//===ダメージの設定・取得===
	virtual void SetDamage(bool flg) override { m_Chara.m_Damage = flg; }
	virtual bool GetDamage() const override { return m_Chara.m_Damage; }
	//========================

	//=====死亡の設定・取得=====
	virtual void SetDeath(bool flg) override { m_Chara.m_Death = flg; }
	virtual bool GetDeath() const override { return m_Chara.m_Death; }
	//========================

	//=====無敵の設定・取得=====
	void SetMuteki(bool flg) override { m_Chara.m_Muteki = flg; }
	bool GetMuteki() const override { return m_Chara.m_Muteki; }
	//========================

	//============アイテムの設定・取得============
	void SetItemFlag(bool item) override { m_Chara.m_ItemFlag = item; }
	bool GetItemFlag() const override { return m_Chara.m_ItemFlag; }
	//============================================

protected:

	CController* 	m_Controller;
	int				m_PlayerID;
	bool			m_HasControl;	//操作権があるか

	//コントローラーを識別する変数.
	int				m_ControllerIndex;

private:
	///TankTuning m_Tune{};
	std::shared_ptr<CInputManager> m_pInput;
	std::unique_ptr<CMultiInputKeyManager> m_Key;
};
