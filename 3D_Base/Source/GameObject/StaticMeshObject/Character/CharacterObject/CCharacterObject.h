#pragma once
//STL.
#include <iostream>

//継承するクラス.
#include "GameObject/StaticMeshObject/Character/CCharacter.h"

//車体と砲塔クラス.
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h"

//================================================================
//	キャラクターオブジェクト※基底クラス(キャラクターを継承).
//================================================================
class CCharacterObjectBase
	: public CCharacter
{
public:
	CCharacterObjectBase(
		int hp = 2,
		const TankTuning& tuning = {},
		std::shared_ptr<CBody> body = nullptr, 
		std::shared_ptr<CCannon> cannon = nullptr
	);
	virtual ~CCharacterObjectBase();

	//動作関数.
	virtual void Update() = 0;
	//描画関数.
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) = 0;
	//インスタンス生成関数.
	virtual void Create(int index) = 0;

	//位置の設定.
	virtual void SetPosition(D3DXVECTOR3 pos) = 0;
	//位置を取得.
	virtual const D3DXVECTOR3 GetPosition() = 0;

	//回転の設定.
	virtual void SetRotation(D3DXVECTOR3 rot) = 0;
	//回転を取得.
	virtual const D3DXVECTOR3 GetRotation() = 0;

	//拡縮の設定.
	virtual void SetScale(D3DXVECTOR3 sca) = 0;
	//拡縮を取得.
	virtual const D3DXVECTOR3 GetScale() = 0;

	//プレイヤーかCOMを識別.
	virtual bool IsPlayer() const = 0;

	//当たった時の処理.
	virtual void OnHit(CCharacterObjectBase* other) = 0;

	//車体・砲塔を取得.
	virtual std::shared_ptr<CBody> const GetBody() { return m_pBody; }
	virtual std::shared_ptr<CCannon> const GetCannon() { return m_pCannon; }

	//パラメータの設定.
	virtual void SetTuning(const TankTuning& tuning) override;
	//パラメータの取得.
	virtual const TankTuning& GetTuning() const override { return m_Tuning; }

protected:
	//車体クラス.
	std::shared_ptr<CBody>		m_pBody;
	//砲塔クラス.
	std::shared_ptr<CCannon>	m_pCannon;

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
};