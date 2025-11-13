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
class CCharacterObject
	: public CCharacter
{
public:
	CCharacterObject(
		int hp = 2,
		const TankTuning& tuning = {}/*,*/
		//std::shared_ptr<CBody> body = nullptr, 
		//std::shared_ptr<CCannon> cannon = nullptr
	);
	virtual ~CCharacterObjectBase();

	//動作関数.
	virtual void Update() override;
	//描画関数.
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	//インスタンス生成関数.
	virtual void Create(int index);

#if 0
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
#endif

	//パラメータの設定.
	virtual void SetTuning(const TankTuning& tuning) { m_Tuning = tuning; }
	//パラメータの取得.
	virtual const TankTuning& GetTuning() const { return m_Tuning; }

	//プレイヤーかCOMを識別.
	virtual bool IsPlayer() const = 0;

	//当たった時の処理.
	virtual void OnHit(CCharacterObjectBase* other) {};

	//車体・砲塔を取得.
	virtual std::shared_ptr<CBody> const GetBody();
	virtual std::shared_ptr<CCannon> const GetCannon();

	//Body.Cannonのオーバロード
	virtual std::shared_ptr<CBody> GetBody() const;
	virtual std::shared_ptr<CCannon> GetCannon() const;

	//戦車のTransform
	void SetTankPosition(const D3DXVECTOR3& pos) {};
	void SetTankRotation(const D3DXVECTOR3& rot) {};
	void SetTankScale(float sca) {};

	virtual const D3DXVECTOR3 GetPosition() = 0;
	virtual const D3DXVECTOR3 GetRotation() = 0;

protected:
	////車体クラス.
	//std::shared_ptr<CBody>		m_Body;
	////砲塔クラス.
	//std::shared_ptr<CCannon>	m_Cannon;

	//キャラクターの状態.
	bool m_IsActive;

	//キャラクターの生存フラグ.
	bool m_IsAlive;

	//戦車の情報.
	TankTuning m_Tuning;

	//キャラクターの体力.
	int m_HP;
	//最大体力.
	int m_MaxHP;

	//描画フラグ.
	bool m_Drawflag;
};