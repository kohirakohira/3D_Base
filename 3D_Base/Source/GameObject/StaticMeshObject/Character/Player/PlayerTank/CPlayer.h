#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // 継承 || キャラクタークラス

//-----ライブラリ-----
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h"		// 戦車：車体クラス
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h"	// 戦車：砲塔クラス

class CXInput;	//前方宣言

class CPlayer
	: public CCharacter
{
public:
	CPlayer();
	virtual ~CPlayer() override;

	virtual void Initialize(int id);

	void AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon);
	void SetTankPosition(const D3DXVECTOR3& pos);
	void SetTankRotation(const D3DXVECTOR3& pos);

	//プレイヤーが壁に当たる処理をまとめる.
	void SetPushBack(const D3DXVECTOR3& push);

	//Body優先でワールド座標と回転を返す
	virtual D3DXVECTOR3 GetPosition() const;
	virtual D3DXVECTOR3 GetRotation() const;

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	// バウンディングオブジェクトを設定
	void SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon);

	// コライダーの作成
	void CreateCollider();

	//外部のクラスから情報取得.
	void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
	void SetCCannon(std::shared_ptr<CCannon> pCannon) { m_pCannon = pCannon; }

	// 外部のクラスに情報を渡す
	std::shared_ptr<CCannon> GetCannon() const { return m_pCannon; }
	std::shared_ptr<CCannon> GetCannon() { return m_pCannon; }
	std::shared_ptr<CBody>	 GetBody()   const { return m_pBody; }

	float GetCannonYaw() const;
	D3DXVECTOR3 GetCannonPosition() const;

	//操作権関連の外部関数
	void SetHasControl(bool control) { m_HasControl = control; }
	void SetKeyBoadEnble(bool control) { m_HasControl = control; }
	bool HasControl() const { return m_HasControl; }

	//パッド用の外部関数
	void SetPadRef(CXInput* pad) { m_pPad = pad; }
	CXInput* GetPadRef() const { return m_pPad; }

	//マネージャーセット
	void SetInputManagerShared(const std::shared_ptr<CInputManager>& im);
	void SetKeyboardEnabled(bool on);
	std::shared_ptr<CInputManager>& GetInputManager() { return m_Input; }

	//PlayerIDをCOMに渡す
	int GetPlayerID() const { return m_PlayerID; }


protected:
	std::shared_ptr<CBody> Body() const { return m_pBody; }
	std::shared_ptr<CCannon> Cannon() const { return m_pCannon; }
	void UpdateHumanInputAndMove();	//プレイヤー処理をいれておく

	//砲塔と車体の同期
	void SyncCannonToBody();

protected:

	std::shared_ptr<CBody>		m_pBody;
	std::shared_ptr<CCannon>	m_pCannon;
	int			m_Hp;
	int			m_PlayerID;
	bool m_HasControl;			//操作権があるか
	CXInput* m_pPad;			//コントローラー

private:
	//TankTuning m_Tune{};
	std::shared_ptr<CInputManager> m_pInput;

};
