#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // 継承 || キャラクタークラス

//-----外部クラス-----
#include "Camera//CCamera.h" //カメラクラス
#include "InputDevice//Input//CInputManager.h" // 入力受付クラス
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h"

#include <memory>

/**************************************************
*	砲塔クラス.
**/
class CCannon
	: public CCharacter	//キャラクタークラスを継承.
{
public:
	CCannon(int inputID);
	~CCannon();

	// 更新関数
	void Update() override;
	// 描画関数
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	// 初期化関数
	void Init();

	// パラメータの設定
	void SetTuning(const TankTuning& tuning) override { m_Tuning = tuning; };
	// パラメータの取得
	const TankTuning& GetTuning() const override { return m_Tuning; }

	// 砲塔座標設定
	void SetCannonPosition(const D3DXVECTOR3& Pos);

	// 砲塔の情報を渡す
	D3DXVECTOR3 GetCannonPosition() const { return m_vPosition; }

	//プレイヤーが壁に当たると戻す.
	void PushBack(const D3DXVECTOR3& push);

	// バウンディングボックスを作成
	void CreateBounding(std::shared_ptr<CStaticMesh> pCannon);

	// 弾クラスの設定
	void SetShotManager(const std::shared_ptr<CShotManager>& shot) { m_pShot = shot; }

	//弾を作成・リロード・発射関数.
	void Reload(D3DXVECTOR3 pos, float y, bool flag, int index);

private:
	// キー入力受付
	void KeyInput();

protected:
	float		m_TurnSpeed;			// 回転速度

	int			m_ShotCoolTime;			// ショットのクールタイム
	const int	m_ShotInterval;			// ショットのインターバル

private:
	// 弾クラス
	std::shared_ptr<CShotManager>		m_pShot;

	// プレイヤーの番号を管理するための変数
	int 			m_PlayerID;				// 入力ID

	// コントローラークラス
	CController* m_pController;
};