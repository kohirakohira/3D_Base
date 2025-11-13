#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//CharacterObject//CCharacterObject.h" // キャラクターオブジェクトクラス

//-----外部クラス-----
#include "Camera//CCamera.h" //カメラクラス
#include "InputDevice//Input//CInputManager.h" // 入力受付クラス
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h"

//================================================================
//	砲塔クラス
//================================================================
class CCannon
	: public CCharacter
{
public:
	CCannon(int inputID);
	~CCannon() override;

	// 更新関数
	void Update() override;
	// 描画関数
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
#if 0
	// 初期化関数
	void Init() override;

	// インスタンス生成
	void Create(int index)  override = 0;

	// 位置の設定
	void SetPosition(D3DXVECTOR3 pos) override { m_vPosition = pos; }
	// 回転の設定
	void SetRotation(D3DXVECTOR3 rot) override { m_vRotation = rot; }
	// 拡縮の設定
	void SetScale(D3DXVECTOR3 sca)	  override { m_vScale	 = sca; }

	// 位置の取得
	const D3DXVECTOR3 GetPosition() override { return m_vPosition; }
	// 回転の取得
	const D3DXVECTOR3 GetRotation() override { return m_vRotation; }
	// 拡縮の取得
	const D3DXVECTOR3 GetScale()	override { return m_vScale;	   }

	// パラメータの設定
	void SetTuning(const TankTuning& tuning) override = 0;
	// パラメータの取得
	virtual const TankTuning& GetTuning() const override = 0;

	//　プレイヤーかCOMを識別
	virtual bool IsPlayer() const override = 0;
	// 当たった時の処理
	void OnHit(CCharacterObject* other) override = 0;
#endif
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