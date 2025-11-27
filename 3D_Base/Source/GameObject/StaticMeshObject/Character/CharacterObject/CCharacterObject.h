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

	//パラメータの設定.
	virtual void SetTuning(const TankTuning& tuning) override{ m_Tuning = tuning; }
	//パラメータの取得.
	virtual const TankTuning& GetTuning() const override{ return m_Tuning; }

	//プレイヤーかCOMを識別.
	virtual bool IsPlayer() const = 0;

	//当たった時の処理.
	virtual void OnHit(CCharacterObjectBase* other) {};

	//Body.Cannonのオーバロード
	virtual std::shared_ptr<CBody> GetBody() const { return m_pBody; }
	virtual std::shared_ptr<CCannon> GetCannon() const { return m_pCannon; }

	//戦車のTransform
	virtual void SetTankPosition(const D3DXVECTOR3& pos) {};
	virtual void SetTankRotation(const D3DXVECTOR3& rot) {};
	virtual void SetTankScale   (const D3DXVECTOR3& sca) {};

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
	virtual void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }

	//キャノンの設定
	virtual void SetCannon(std::shared_ptr<CCannon> pCannon) { m_pCannon = pCannon; }

	//キャノンポジション
	virtual D3DXVECTOR3 GetCannonPosition() const;

	virtual void HitPlayer();

	virtual bool HasControl() const { return m_HasControl; }

	virtual void SyncCannonToBody();

	int GetPlayerID() { return m_PlayerID; }
protected:

	
	//車体クラス.
	std::shared_ptr<CBody>		m_pBody;
	//砲塔クラス.
	std::shared_ptr<CCannon>	m_pCannon;

	//キャラクターの状態.
	bool m_IsActive;

	//キャラクターの生存フラグ.
	bool m_IsAlive;

	//戦車の情報.
	TankTuning m_Tuning;

	//ここから下はいらなかったら消してください
	//キャラクターの体力.
	int m_HP;
	//最大体力.
	int m_MaxHP;

	//描画フラグ.
	bool m_Drawflag;

	//リスポーンフラグ
	bool m_Respawn;
	
	//リスポーンタイム
	float m_RespawnTimer;

	//操作権があるか
	bool m_HasControl;	

	//死亡しているか
	bool m_Death;

	//ダメージフラグ
	bool m_Damage;
	
	//無敵時間
	float m_MutekiTimer;

	//無敵状態カウント
	float m_MutekiCut;

	//プレイヤーID
	int m_PlayerID;

};