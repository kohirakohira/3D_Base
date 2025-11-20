#pragma once
//-----継承するクラス-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // キャラクタークラス

//-----外部クラス-----
#include "GameObject//StaticMeshObject//Character//CharacterObject//Player//PlayerTank//TankCannon//CCannon.h" // 戦車：砲塔クラス
#include "InputDevice//Input//Controller//CController.h" // 入力受付クラス

#include <memory>

/**************************************************
*	車体クラス.
**/

// 前方宣言
class CInputManager;

class CBody
	: public CCharacter	// キャラクタークラスを継承
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
	~CBody();

	// 更新関数
	void Update() override;
	// 描画関数
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	// 初期化関数
	void Init();

	// パラメータの設定
	void SetTuning(const TankTuning& tuning) override { m_Tuning = tuning; };
	// パラメータの取得
	const TankTuning& GetTuning() const override { return m_Tuning; }

	// ラジコン操作
	void RadioControl();

	//プレイヤーが壁に当たると戻す.
	void PushBack(const D3DXVECTOR3& push);

	// バウンディングボックスを作成
	void CreateBounding(std::shared_ptr<CStaticMesh> pBody);

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

	// コントローラー変数
	CController* m_pController;
};
