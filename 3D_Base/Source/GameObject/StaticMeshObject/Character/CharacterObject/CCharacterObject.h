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

	//キャラクターの体力.
	int m_HP;
	//最大体力.
	int m_MaxHP;

	//描画フラグ.
	bool m_Drawflag;

	//リスポーンフラグ
	bool m_Respawn;

	//操作権があるか
	bool m_HasControl;	

	//死亡しているか
	bool m_Death;

	//ダメージフラグ
	bool m_Damage;


};