#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // 継承 || キャラクタークラス

//-----外部クラス-----
#include "Camera//CCamera.h" //カメラクラス
#include "InputDevice//Input//CInputManager.h" // 入力受付クラス
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h"

/**************************************************
*	砲塔クラス.
**/
class CCannon
	: public CCharacter	//キャラクタークラスを継承.
{
public:
	CCannon(int inputID);
	virtual ~CCannon() override;

	virtual void Initialize(int id);

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	void SetCannonPosition(const D3DXVECTOR3& Pos);

	D3DXVECTOR3 GetCannonPosition() const { return m_vPosition; }

	// 入力クラスを設定
	void SetInputManager(const std::shared_ptr<CInputManager>& input);

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
	std::shared_ptr<CCamera> m_pCamera;

private:
	// プレイヤーの番号を管理するための変数
	int 			m_PlayerID;				// 入力ID

	std::shared_ptr<CInputManager>		m_pInput;

	// 弾クラス
	std::shared_ptr<CShotManager>		m_pShot;
};