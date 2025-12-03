#pragma once
//----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // プレイヤークラス

//-----ライブラリ-----
#include <iostream>
#include <vector>
#include <array>

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/CPlayer.h" // プレイヤークラス
#include "GameObject//StaticMeshObject//Shot//CShot.h" // ショットクラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/COM/CComPlayer.h" // COMプレイヤークラス
#include "Global.h"

//↓松岡.
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankBody/CBody.h" // 戦車ボディクラス
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h" // 戦車キャノンクラス

#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//ショットマネージャー

//コントローラークラス.
#include "InputDevice/Input/Controller\ControllerManager/CControllerManager.h"

class CCharacterManager
	: public CStaticMeshObject
{
public:
	CCharacterManager();
	~CCharacterManager() override;

	//=======初期化・更新・描画=======
	void Init(); // 初期化
	void Update() override;// 更新関数	
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;// 描画関数
	//==============================

	//=======メッシュをアタッチ=======
	void AttachMeshesToPlayer(int index, std::shared_ptr<CStaticMesh> body, std::shared_ptr<CStaticMesh> cannon);
	//==============================

	//=======座標・回転・拡縮を設定=======
	void SetPlayerPosition(int index, const D3DXVECTOR3& pos); // 各プレイヤーに座標設定
	void SetPlayerRotation(int index, const D3DXVECTOR3& rad); // 各プレイヤーに回転設定
	void SetPlayerScale	  (int index, const D3DXVECTOR3& xyz); // 各プレイヤーに拡縮設定
	//==================================

	//=======オブジェクトと当たった時の押し返し=======
	void SetPushBackPosision(int index, const D3DXVECTOR3& push);
	//=============================================

	//=======バウンディング・コライダーの作成=======
	void CreateBounding(int index, const std::shared_ptr<CStaticMesh>& body, const std::shared_ptr<CStaticMesh>& cannon);
	void CreateCollider(int index);	// コライダーの作成
	//===========================================
	
	//=======プレイヤーのリスポーン=======	
	void PlayerRespawn(int index);		// プレイヤーのリスポーン
	void SetRespawnArea(int index);		// リスポーン可能エリアにプレイヤーの座標を指定する		
	int GetAreaIndex(float x, float z); // マップの中央を跨がないように計算する
	//==================================

	//=======プレイヤーのゲーム開始時の座標設定=======	
	void SetStartPosition();	// ゲームの開始座標設定
	//=============================================

	//=======プレイヤーを取得=======	
	std::shared_ptr<CCharacterObjectBase> GetControlPlayer(int index); // 引数あり
	//============================

	void SwitchActivePlayer(); // 旧コード：動かせるプレイヤーを変更できた

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

	//プレイヤーとCOMの切り替え.
	void SwitchControl();

	//=======パラメータ設定用=======
	void SetPlayerTuningAll(const TankTuning& t);
	void SetPlayerTuning(int idx, const TankTuning& t);
	//============================

	//=======パラメータの設定・取得=======
	void SetTuning(const TankTuning& tuning, int index);
	const TankTuning& GetTuning(int index) const;
	//==================================

	//COMに障害物を配る
	void SetComObstacleRef(const std::vector<CComPlayer::SimpleObstacle>* obstacles);

private:

	struct Area {
		bool Taken = false;
		D3DXVECTOR3 RespawnPos; // リスポーン位置
		D3DXVECTOR3 RespawnRot; // リスポーン向き
	};

	// プレイヤーのリスポン位置オフセット
	float offset;

	// プレイヤーの向き
	float AngleY;

	// エリア4つを定義（マップの座標系に合わせて調整）
	Area areas[4];

	//↓松岡.
	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;
	std::shared_ptr<CShotManager>			m_ShotManager; //弾マネージャー
	std::shared_ptr<CComPlayer>				m_pCom;
	//*仮追加
	//std::shared_ptr<CComPlayer>				m_pCom;

	//CPlayerとCComPlayerを同一コンテナで管理するための基底に統一
	std::vector<std::shared_ptr<CCharacterObjectBase>> m_pCharacter;

	int m_ActivePlayerIndex;	// 現在操作中のプレイヤー(デバッグ用)

	int m_LockTargetIndex = -1;	//ロック無効(デバック用)

	bool m_OnePPadEnabled = true; // 1PパッドON/OFF
	int m_keyboardPlayer = 0;
};