#pragma once
//----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//CharacterObject//CCharacterObject.h" // プレイヤークラス

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

class CPlayerManager
	: public CCharacterObjectBase
{
public:
	CPlayerManager();
	~CPlayerManager() override;

	void Initialize();
	void AttachMeshesToPlayer(int index, std::shared_ptr<CStaticMesh> body, std::shared_ptr<CStaticMesh> cannon);
	void SetPlayerPosition(int index, const D3DXVECTOR3& pos);
	void SetPushBackPosision(int index, const D3DXVECTOR3& push);
	void SetPlayerScale(int index, const float& xyz);

	// バウンディングオブジェクトの作成
	void CreateBounding(int index, const std::shared_ptr<CStaticMesh>& body, const std::shared_ptr<CStaticMesh>& cannon);
	
	// コライダーの作成
	void CreateCollider(int index);

	void PlayerRespawn(int index);

	// リスポーン可能エリアに
	// プレイヤーの座標を指定する
	void SetRespawnArea(int index);

	// マップの中央を跨がないように計算する
	int GetAreaIndex(float x, float z);

	// ゲームの開始座標設定
	void SetStartPosition();

	//↓松岡.
	void SetPlayerRotation(int index, const D3DXVECTOR3& rad);
	// 更新関数
	void Update() override;
	// 描画関数
	void Draw (D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	// インスタンスの生成
	void Create(int index) override;
	
	//位置の設定.
	void SetPosition(D3DXVECTOR3 pos) override;
	//位置を取得.
	const D3DXVECTOR3 GetPosition() override;

	//回転の設定.
	void SetRotation(D3DXVECTOR3 rot) override;
	//回転を取得.
	const D3DXVECTOR3 GetRotation() override;

	//拡縮の設定.
	void SetScale(D3DXVECTOR3 sca) override;
	//拡縮を取得.
	const D3DXVECTOR3 GetScale() override;

	//プレイヤーかCOMを識別.
	bool IsPlayer() const;

	//当たった時の処理.
	void OnHit(CCharacterObjectBase* other);

	//D3DXVECTOR3 GetPosition();
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

	//パラメータ設定用
	void SetPlayerTuningAll(const TankTuning& t);
	void SetPlayerTuning(int idx, const TankTuning& t);

	//プレイヤーとCOMの切り替え.
	void SwitchControl();

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
    std::vector<std::shared_ptr<CPlayer>>	m_pPlayers;
	std::shared_ptr<CShotManager>			m_ShotManager; //弾マネージャー

	int m_ActivePlayerIndex;	// 現在操作中のプレイヤー(デバッグ用)

	int m_LockTargetIndex = -1;	//ロック無効(デバック用)

	bool m_OnePPadEnabled = true; // 1PパッドON/OFF
	int m_keyboardPlayer = 0;
};