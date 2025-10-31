#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" /* 継承クラス || キャラクタークラス */

//-----ライブラリ-----
#include <iostream>
#include <vector>
#include <array>

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // プレイヤークラス
#include "GameObject//StaticMeshObject//Shot//CShot.h" // ショットクラス
#include "gameObject//StaticMeshObject//Character//COM//CComPlayer.h" // COMプレイヤークラス
#include "Global.h"

//↓松岡.
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h" // 戦車ボディクラス
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h" // 戦車キャノンクラス

#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//ショットマネージャー

class CXInput;	//前方宣言

class CPlayerManager
	: public CCharacter
{
public:
	CPlayerManager();
	~CPlayerManager();

	void Initialize();
	void AttachMeshesToPlayer(int index, std::shared_ptr<CStaticMesh> body, std::shared_ptr<CStaticMesh> cannon);
	void SetPlayerPosition(int index, const D3DXVECTOR3& pos);
	void SetPushBackPosision(int index, const D3DXVECTOR3& push);
	void SetPlayerScale(int index, const float& xyz);

	// バウンディングオブジェクトの作成
	void CreateBounding(int index, const std::shared_ptr<CStaticMesh>& body, const std::shared_ptr<CStaticMesh>& cannon);
	
	// コライダーの作成
	void CreateCollider(int index);

	//↓松岡.
	void SetPlayerRotation(int index, const D3DXVECTOR3& rad);
	void Update() override;
	void Draw (D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	
	D3DXVECTOR3 GetPosition();
	std::shared_ptr<CPlayer> GetControlPlayer(int index);

	void SwitchActivePlayer();

	//プレイヤーの位置と回転を取得.引数には各プレイヤーを入れる
	D3DXVECTOR3 GetPosition(int index)const;
	D3DXVECTOR3 GetRotation(int index)const;

	//追尾のデバック
	//ロック対象のインデックス指定
	void SetLockTargetIndex(int index) { m_LockTargetIndex = index; }

	//ショット用
	void SetShotManager(std::shared_ptr<CShotManager>& mgr);

	//↓松岡
	//子オブジェクトに各BodyとCannonを設定してあげる関数.
	void SetBodyAndCannon(std::shared_ptr<CBody> body, std::shared_ptr<CCannon> cannon);

private:
	//↓松岡.
	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;
    std::vector<std::shared_ptr<CPlayer>>	m_pPlayers;
	std::shared_ptr<CShotManager> m_ShotManager; //弾マネージャー

	std::array<std::unique_ptr<CXInput>, 4>	m_Pads;	//Pad0から3
	std::array<bool, 4> m_PadConnected{};			//前フレーム接続状態
	std::array<int, 4>m_PadIndex;					//padId->playerIdx (-1=未割当)
	std::vector<int> m_PlayerPad;					//playerIdx->padId (-1=未割当)

	int m_ActivePlayerIndex;	// 現在操作中のプレイヤー(デバッグ用)

	int m_LockTargetIndex = -1;	//ロック無効(デバック用)

	bool m_OnePPadEnabled = true; // 1PパッドON/OFF
	int m_keyboardPlayer = 0;
private:
	void InitPads();
	void SyncByPadConnection();   //接続状況で割当/COM切替
	int  FindFirstComPlayer() const;
	int FindFirstPadReceiver(int StartIndex = 1)const;

	//パラメータ設定用
	void SetPlayerTuningAll(const TankTuning& t);
	void SetPlayerTuning(int idx, const TankTuning& t);

};


