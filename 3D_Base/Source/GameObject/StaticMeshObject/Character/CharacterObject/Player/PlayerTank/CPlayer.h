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

class CPlayer
	: public CCharacterObjectBase
{
public:

	// 構造体を作成
	struct Player
	{
		int			m_Hp;			// プレイヤーのHP
		int			m_MaxHp;		// プレイヤーの最大HP

		int			m_MutekiCnt;	// 無敵カウント
		float		m_MutekiTimer;	// 無敵時間

		float		m_RespawnTimer;	// リスポーン時間
		
		bool		m_Draw;			// 描画するかどうか

		bool		m_Damage;		// ダメージを受けたか
		bool		m_Death;		// 死亡しているか
		bool		m_Respawn;		// リスポーン
	};
	Player m_Player;

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
	void Rotate(const PlayerInput& input);
	void Reload(const D3DXVECTOR3& pos, float y);

public:
	CPlayer();
	virtual ~CPlayer() override;

	void Init(int id);

	//void AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon) ;

	// 戦車の座標、回転、拡縮を設定
	void SetTankPosition(const D3DXVECTOR3& pos) override;
	void SetTankRotation(const D3DXVECTOR3& rot) override;
	void SetTankScale   (const D3DXVECTOR3& sca) override;

	//プレイヤーが壁に当たる処理をまとめる.
	void SetPushBack(const D3DXVECTOR3& push);

	// 更新関数
	virtual void Update() override;
	// 描画関数
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	// インスタンス生成関数
	virtual void Create(int index) override;

	// 位置の設定
	virtual void SetPosition(D3DXVECTOR3 pos) override;
	// 位置を取得
	virtual const D3DXVECTOR3 GetPosition() override;

	// 回転の設定
	virtual void SetRotation(D3DXVECTOR3 rot) override;
	// 回転を取得
	virtual const D3DXVECTOR3 GetRotation() override;

	// 拡縮の設定
	virtual void SetScale(D3DXVECTOR3 sca) override;
	// 拡縮を取得
	virtual const D3DXVECTOR3 GetScale() override;

	// プレイヤーかCOMを識別
	virtual bool IsPlayer() const override { return true; }

	// プレイヤーのダメージ処理
	void PlayerDamage();
	// プレイヤーの死亡処理
	void PlayerDeath();

	// プレイヤーが爆風と当たった時の処理
	void HitPlayer() override;

	// バウンディングオブジェクトを設定
	void SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)override;

	// コライダーの作成
	void CreateCollider();

	//外部のクラスから情報取得.
	void SetCBody(std::shared_ptr<CBody> pBody) override { m_pBody = pBody; }
	void SetCannon(std::shared_ptr<CCannon> pCannon) override { m_pCannon = pCannon; }

	void AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon) override;

	// 外部のクラスに情報を渡す
	std::shared_ptr<CBody> GetBody() const override { return m_pBody; }
	std::shared_ptr<CCannon> GetCannon() const override { return m_pCannon; }

	//操作権関連の外部関数
	void SetHasControl(bool control) override { m_HasControl = control; }
	void SetKeyBoadEnble(bool control) { m_HasControl = control; }
	bool HasControl() const { return m_HasControl; }

	// PlayerIDをCOMに渡す
	int GetPlayerID() const { return m_PlayerID; }

	// リスポーンフラグ設定
	//void SetRespawnFlag(bool flg) { m_Player.m_Respawn = flg; }

	//リスポーンフラグの設定継承版
	void SetRespawnFlag(bool flg) override { m_Respawn = flg; } 

	// リスポーンフラグの取得
	bool GetRespawnFlag() { return m_Player.m_Respawn; }

	// プレイヤーのコントローラー設定・取得
	void SetControllerIndex(int index);
	int GetControllerIndex() const { return m_ControllerIndex; }

	//パラメータの設定.
	virtual void SetTuning(const TankTuning& tuning) override {	m_Tuning = tuning; }
	//パラメータの取得.
	virtual const TankTuning& GetTuning() const override { return m_Tuning; }

	static void DebugLog(const char* msg){OutputDebugStringA(msg);}

	void RotateTurretByPad();

protected:
	std::shared_ptr<CBody> Body() const { return m_pBody; }
	std::shared_ptr<CCannon> Cannon() const { return m_pCannon; }
	void UpdateHumanInputAndMove(PlayerInput input);	//プレイヤー処理をいれておく

	// 砲塔と車体の同期
	void SyncCannonToBody();

protected:

	CController* 	m_Controller;
	int			m_PlayerID;
	bool		m_HasControl;	//操作権があるか

	//コントローラーを識別する変数.
	int			m_ControllerIndex;

private:
	///TankTuning m_Tune{};
	std::shared_ptr<CInputManager> m_pInput;

};
