#pragma once
//STL.
#include <iostream>
#include <memory>

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
#if 0
	void CPlayer::Create(int id)
{
	m_Body = std::make_shared<CBody>();
	m_Cannon = std::make_shared<CCannon>();

	//マネージャーセットキャノンボディ
	actor = player;
	m_pPlayers.push_back(actor);

	// Body / Cannon を渡すのは「push したあと」に行う
	SetBodyAndCannon(player->GetBody(), player->GetCannon());

}

#endif
{
public:
	CCharacterObjectBase(
		int hp = 2,
		//const TankTuning& tuning = {},
		std::shared_ptr<CBody> body = nullptr, 
		std::shared_ptr<CCannon> cannon = nullptr
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
	virtual void SetTuning(const TankTuning& tuning) override{ m_Tuning = tuning; }
	//パラメータの取得.
	virtual const TankTuning& GetTuning() const override{ return m_Tuning; }

	//プレイヤーかCOMを識別.
	virtual bool IsPlayer() const = 0;

	//当たった時の処理.
	virtual void OnHit(CCharacterObjectBase* other) {};

	//車体・砲塔を取得.
	virtual std::shared_ptr<CBody> const GetBody() { return m_Body; }
	virtual std::shared_ptr<CCannon> const GetCannon() { return m_Cannon; }

	//Body.Cannonのオーバロード
	virtual std::shared_ptr<CBody> GetBody() const { return m_Body; }
	virtual std::shared_ptr<CCannon> GetCannon() const { return m_Cannon; }

	//戦車のTransform
	virtual void SetTankPosition(const D3DXVECTOR3& pos) {};
	virtual void SetTankRotation(const D3DXVECTOR3& rot) {};
	virtual void SetTankScale(float sca) {};

	virtual const D3DXVECTOR3 GetPosition() = 0;
	virtual const D3DXVECTOR3 GetRotation() = 0;

	//メッシュアタッチ.CPlayerManagerで使う用
	virtual void AttachMeshse(std::shared_ptr<CStaticMesh>pBody, std::shared_ptr<CStaticMesh>pCannon) ;
	
	//バウンディングオブジェクト設定
	virtual void SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon);

	//コライダー作成
	void CreateCollider();

	//リスポーンフラグの取得
	virtual bool GetRespawnFlag() { return m_Respawn; }

	//リスポーンフラグの設定
	virtual void SetRespawnFlag(bool flg) { m_Respawn = flg; }

	//操作権の設定
	virtual void SetHasControl(bool control) { m_HasControl = control; }

	//ボディの設定
	virtual void SetCBody(std::shared_ptr<CBody> pBody) { m_Body = pBody; }

	//キャノンの設定
	virtual void SetCannon(std::shared_ptr<CCannon> pCannon) { m_Cannon = pCannon; }

	//キャノンポジション
	virtual D3DXVECTOR3 GetCannonPosition() const;

	virtual float GetCannonYaw() const;

	virtual void HitPlayer();

protected:
	//車体クラス.
	std::shared_ptr<CBody>		m_Body;
	//砲塔クラス.
	std::shared_ptr<CCannon>	m_Cannon;

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

	//リスポーンフラグ
	bool m_Respawn;

	//操作権があるか
	bool m_HasControl;	

	//死亡しているか
	bool m_Death;

	//ダメージフラグ
	bool m_Damage;
};