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

	//砲塔の位置取得.
	D3DXVECTOR3 GetCannonPosition() const { return m_pCannon->GetPosition(); }
	float GetCannonYaw() const { return m_pCannon->GetRotation().y; }

	//当たった時用.
	void OnHit(CCharacterObjectBase* other);

	// インスタンス生成関数
	void Create();

	//プレイヤーが壁に当たる処理をまとめる.
	void SetPushBack(const D3DXVECTOR3& push);

	// コライダーの作成
	void CreateCollider();

	// PlayerIDをCOMに渡す
	int GetPlayerID() const { return m_PlayerID; }

	// プレイヤーのコントローラー設定・取得
	void SetControllerIndex(int index);
	int GetControllerIndex() const { return m_ControllerIndex; }

	static void DebugLog(const char* msg){OutputDebugStringA(msg);}

	void RotateTurretByPad();

	void UpdateHumanInputAndMove(PlayerInput input);	//プレイヤー処理をいれておく

	// 砲塔と車体の同期
	void SyncCannonToBody();

	//位置の取得.
	D3DXVECTOR3 GetPosition() const override
	{
		if (m_pBody) return m_pBody->GetPosition();
		return D3DXVECTOR3(0, 0, 0);
	}

	//位置設定.
	void SetPosition(const D3DXVECTOR3& pos) override
	{
		if (m_pBody)   m_pBody->SetPosition(pos);
		if (m_pCannon) m_pCannon->SetPosition(pos);
	}

	//回転取得
	D3DXVECTOR3 GetRotation() const override
	{
		if (m_pBody) return m_pBody->GetRotation();
		return D3DXVECTOR3(0, 0, 0);
	}

	//回転設定
	void SetRotation(const D3DXVECTOR3& rot) override
	{
		if (m_pBody) m_pBody->SetRotation(rot);
	}

	//プレイヤーのインデックス番号の取得.
	int GetPlayerID() { return m_PlayerID; }

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
