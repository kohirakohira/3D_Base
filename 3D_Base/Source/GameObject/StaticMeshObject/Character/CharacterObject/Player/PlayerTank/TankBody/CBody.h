#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//CharacterObject//CCharacterObject.h" // キャラクターオブジェクトクラス

//-----外部クラス-----
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/PlayerTank/TankCannon/CCannon.h" // 戦車：砲塔クラス
#include "InputDevice//Input//CInputManager.h" // 入力受付クラス

//================================================================
//	車体クラス.
//================================================================

// 前方宣言
//class CInputManager;

class CBody
	: public CCharacterObject
{
public:
	// 移動状態
	enum enMoveState
	{
		Stop,		// 停止
		Forward,	// 前進
		Backward,	// 後退
		TurnLeft,	// 左回転
		TurnRight,	// 右回転
	};
public:
	CBody(int inputID);
	// 更新関数
	void Update() override;
	// 描画関数
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
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

	// ラジコン操作
	void RadioControl();

	// 入力クラスを設定
	void SetInputManager(const std::shared_ptr<CInputManager>& input);

	//プレイヤーが壁に当たると戻す.
	void PushBack(const D3DXVECTOR3& push);

	// バウンディングボックスを作成
	void CreateBounding(std::shared_ptr<CStaticMesh> pBody);

	// リスポーンエリア設定
	void SetRespawnArea(int Area);

	// リスポーン
	void Respawn();

	// 死亡確認
	void Death();

	//Bodyの向きを変更できる関数.
	void AddRotationY(float value);

	//移動列挙型の設定.
	void SetMoveState(enMoveState state) { m_MoveState = state; }

private:
	// キー入力受付.
	void KeyInput();

protected:
	float		m_TurnSpeed;	// 回転速度
	float		m_MoveSpeed;	// 移動速度
	enMoveState m_MoveState;	// 移動状態

private:
	bool		m_Death;			// 死亡
	int			m_RespawnCoolTime;  // リスポーンクールタイム
	int			m_RespawnTime;		// リスポーン時間測定

	std::shared_ptr<CInputManager> m_pInput;
};
