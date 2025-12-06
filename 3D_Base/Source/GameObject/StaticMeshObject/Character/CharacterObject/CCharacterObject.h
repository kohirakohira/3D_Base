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

	//キャラクター共通処理
	struct Character
	{
								
		bool	m_IsActive;		//キャラクターの状態.
		bool	m_IsAlive;		//キャラクターが生存しているか.
		int		m_HP;			//キャラクターの体力.
		int		m_MaxHP;		//キャラクターの最大体力.
		bool	m_Drawflag;		//描画ができるか.
		bool	m_Respawn;		//リスポーンするか.
		float	m_RespawnTimer;	//リスポーンするまでの時間.
		bool	m_HasControl;	//操作権があるか.
		bool	m_Death;		//キャラクターが死亡しているか.
		bool	m_Damage;		//ダメージを受けたかどうか.	
		float	m_MutekiTimer;	//無敵時間.
		bool	m_Muteki;		//無敵かどうか.
		float	m_MutekiCnt;	//無敵時間のカウント.

	};

	CCharacterObjectBase(
		int hp = 2
	);
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

	//当たった時用.
	virtual void OnHit(CCharacterObjectBase* other) = 0;

	//プレイヤーかCOMを判定する用.
	virtual bool IsPlayer() const = 0;

	//操作可能かどうか.
	virtual void SetHasControl(bool enable) = 0;
	virtual bool HasControl() const = 0;

	//リスポーンフラグの取得
	virtual bool GetRespawnFlag() const = 0;
	//リスポーンフラグの設定
	virtual void SetRespawnFlag(bool flg) = 0;

	//プレイヤー取得
	int GetPlayerID() { return m_PlayerID; }

	//位置取得
	virtual D3DXVECTOR3 GetPosition() const
	{
		if (m_pBody) return m_pBody->GetPosition();
		return m_vPosition;  // フォールバック（CGameObjectのメンバ）
	}

	//位置設定
	virtual void SetPosition(const D3DXVECTOR3& pos)
	{
		if (m_pBody)   m_pBody->SetPosition(pos);
		if (m_pCannon) m_pCannon->SetPosition(pos);
		m_vPosition = pos;  // CGameObjectのメンバにも設定
	}

	//回転取得
	virtual D3DXVECTOR3 GetRotation() const
	{
		if (m_pBody) return m_pBody->GetRotation();
		return m_vRotation;  // フォールバック
	}

	//追加: 回転設定
	virtual void SetRotation(const D3DXVECTOR3& rot)
	{
		if (m_pBody) m_pBody->SetRotation(rot);
		m_vRotation = rot;
	}

protected:
	//車体クラス.
	std::shared_ptr<CBody>			m_pBody;
	//砲塔クラス.
	std::shared_ptr<CCannon>		m_pCannon;
	//弾マネージャークラス.
	//生成や設定はPlayer/COM側で行う.
	std::shared_ptr<CShotManager>	m_pShotManager;

	//キャラクター共通処理
	Character m_Character;

	//プレイヤーID.デフォルト-1
	int m_PlayerID;
};