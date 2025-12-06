#include "CPlayer.h"
#include "InputDevice/Input/XInput/CXInput.h"

//指定した区間から外れていたら、近い端点に合わせる
template <class T>	//比較演算子が使える型に対応させる
static inline T clamp(T v, T lo, T hi)
{
	if (v < lo)	//下限に丸める
	{
		return lo;
	}
	if (v > hi)	//上限に丸める
	{
		return hi;
	}
	return v;
}

//XInputのSHORTを-1,1に正規化
static inline float ToStick01(short raw)
{
	float denom;	//分母
	if (raw >= 0)
	{
		denom = 32767.0f;
	}
	else
	{
		denom = 32768.0f;	//32767で割ると少しはみ出す
	}

	float v = static_cast<float>(raw) / denom;
	//-1, +1にクランプ
	if (v < -1.0f)
	{
		v = -1.0f;
	}
	else if (v > 1.0f)
	{
		v = 1.0f;
	}
	return v;
}

//デッドゾーン
static inline float Deadzone(float v, float z)
{
	return (std::fabs(v) ? 0.0f : v);
}


CPlayer::CPlayer()
	: m_Controller( nullptr )

	, m_HasControl		( false )
	, m_KeyBoad			( true )
	, m_ControllerIndex	()
{
	//プレイヤー初期値.
	m_Player = {
		0,		// プレイヤーの体力
		2,		// プレイヤーの最大体力
		0,		// 無敵カウント
		0.3f,	// 無敵時間
		3.0f,	// リスポーン時間
		true,	// 描画するかどうか
		false,	// ダメージを受けたか
		false,	// 死亡しているか
		false,	// リスポーン
	};
}

CPlayer::~CPlayer() = default;

void CPlayer::Init(int id)
{
	// プレイヤーIDにそれぞれのID番号を入れる
	m_PlayerID = id;
	CCharacterObjectBase::m_PlayerID = id;	//基底クラスにも反映

	m_Controller = CControllerManager::GetInstance().GetController(m_PlayerID);

	//インスタンスを生成
	m_pBody = std::make_shared<CBody>(id);
	m_pCannon = std::make_shared<CCannon>(id);

	// プレイヤーの体力に最大体力を入れる
	m_Player.m_Hp = m_Player.m_MaxHp;
	// プレイヤーの無敵時間を初期化
	m_Player.m_MutekiCnt = 0;
	m_Player.m_MutekiTimer = 0.3;
	// プレイヤーのフラグを初期化
	m_Player.m_Draw	   = true;
	m_Player.m_Damage  = false;
	m_Player.m_Death   = false;
	m_Player.m_Respawn = false;

	//継承したものも初期化
	m_Character.m_Drawflag = true;
	m_Character.m_Damage = false;
	m_Character.m_Death = false;
	m_Character.m_Respawn = false;
	m_Character.m_IsActive = true;
	m_Character.m_IsAlive = true;
}

void CPlayer::SetPushBack(const D3DXVECTOR3& push)
{
	m_pBody->PushBack(push);
	m_pCannon->PushBack(push);
}

void CPlayer::Update()
{
	//プレイヤー入力.
	PlayerInput m_CurrentInput{};

	if (m_Controller != nullptr)
	{
		m_CurrentInput.moveDir		= m_Controller->GetLeftStickDirection(0.5f);
		m_CurrentInput.turretDir	= m_Controller->GetRightStickDirection(0.5f);
		m_CurrentInput.shot			= m_Controller->Down(CXInput::RB, true);
	}

	//操作権がない時は入力を読まない
	if (!m_HasControl)
	{
		if (m_pBody)   m_pBody->Update();
		if (m_pCannon) m_pCannon->Update();
		return;
	}

	// ダメージ処理の更新
	PlayerDamage();
	// 死亡処理の更新
	PlayerDeath();

	//移動とか適用
	UpdateHumanInputAndMove(m_CurrentInput);

	//車体の位置が決まったあとで砲塔の位置を同期
	SyncCannonToBody();
}

void CPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (m_Player.m_Draw == true)
	{
		m_pBody->Draw(View, Proj, Light, Camera);
		m_pCannon->Draw(View, Proj, Light, Camera);
	}
}

void CPlayer::OnHit(CCharacterObjectBase* other)
{
}

void CPlayer::Create()
{
	//インスタンス生成(BodyとCannon).
	m_pBody = std::make_shared<CBody>(m_PlayerID);
	m_pCannon = std::make_shared<CCannon>(m_PlayerID);
	//弾マネージャー.
	m_pShotManager = std::make_shared<CShotManager>();
}

#if 1
//移動.
void CPlayer::Move(const PlayerInput& input)
{
	//左スティックで移動.
	auto dir = m_Controller->GetLeftStickDirection(0.5f);
	//デフォは停止.
	m_pBody->SetMoveState(CBody::Stop);

	switch (dir)
	{
	case CController::Direction::Up:
		//前進.
		m_pBody->SetMoveState(CBody::Forward);
		break;
	case CController::Direction::Down:
		//後退.
		m_pBody->SetMoveState(CBody::Backward);
		break;
	case CController::Direction::Left:
		m_pBody->AddRotationY(-m_Tuning.turretTurnSpeed);
		break;
	case CController::Direction::Right:
		m_pBody->AddRotationY(m_Tuning.turretTurnSpeed);
		break;
	case CController::Direction::UpLeft:
		m_pBody->AddRotationY(-m_Tuning.turretTurnSpeed);
		m_pBody->SetMoveState(CBody::Forward);
		break;
	case CController::Direction::UpRight:
		m_pBody->AddRotationY(m_Tuning.turretTurnSpeed);
		m_pBody->SetMoveState(CBody::Forward);
		break;
	case CController::Direction::DownLeft:
		m_pBody->AddRotationY(-m_Tuning.turretTurnSpeed);
		m_pBody->SetMoveState(CBody::Backward);
		break;
	case CController::Direction::DownRight:
		m_pBody->AddRotationY(m_Tuning.turretTurnSpeed);
		m_pBody->SetMoveState(CBody::Backward);
		break;
	case CController::Direction::None:
		//何も入力が無いので停止しておく.
		m_pBody->SetMoveState(CBody::Stop);
		break;
	default:
		break;
	}
	//移動.
	m_pBody->RadioControl();
}
#endif

//弾発射.
void CPlayer::Reload(const D3DXVECTOR3& pos, float y)
{
	PlayerInput input{};

	//弾の発射. 
	input.shot = true;

	m_pCannon->Reload(pos, y, input.shot, m_PlayerID);
}

//プレイヤーのコントローラー設定.
void CPlayer::SetControllerIndex(int index)
{
	m_ControllerIndex = index;
}

//砲塔だけを回転させる
void CPlayer::RotateTurretByPad()
{
	if (!m_pCannon || !m_Controller)return;
	if (!m_Controller->CheckConnected()) return;

	auto dir = m_Controller->GetRightStickDirection(0.5f);

	D3DXVECTOR3 rot = m_pCannon->GetRotation();

	switch (dir)
	{
	case CController::Direction::Left:
	case CController::Direction::UpLeft:
	case CController::Direction::DownLeft:
		rot.y -= m_Tuning.turretTurnSpeed;
		break;

	case CController::Direction::Right:
	case CController::Direction::UpRight:
	case CController::Direction::DownRight:
		rot.y += m_Tuning.turretTurnSpeed;
		break;

	default:
		break;
	}

	m_pCannon->SetRotation(rot);
	
	
}

void CPlayer::UpdateHumanInputAndMove(PlayerInput input)
{

	//松岡.
	// コントローラー番号を取得
	const int index = GetControllerIndex();

	// そのコントローラーの入力を取得
	CController* controller = CControllerManager::GetInstance().GetController(index);

	// 左スティックで車体の移動／回転
	Move(input);

	//右スティックで砲塔回転
	RotateTurretByPad();
	//Rotate(input);

	// RB入力があった時 → リロード
	if (controller->Down(CXInput::RB, true))
	{
		Reload(m_pCannon->GetCannonPosition(), m_pCannon->GetRotation().y);
	}

	// 砲塔を車体位置に追従させる
	SyncCannonToBody();

	// 見た目の更新
	if (m_pBody)   m_pBody->Update();
	if (m_pCannon) m_pCannon->Update();
}

//砲塔と車体を同期する
void CPlayer::SyncCannonToBody()
{
	if (!GetBody() || !GetCannon()) return;

	D3DXVECTOR3 pos = GetBody()->GetPosition();
	pos.y += m_Tuning.cannonHeight;			// 砲塔の高さオフセット
	GetCannon()->SetPosition(pos);			// 位置を同期
}

// プレイヤーのダメージ処理
void CPlayer::PlayerDamage()
{
	//時間定数宣言.
	const float TIME = 1.0f / FPS;

	if (m_Player.m_Damage == true)
	{
		// 無敵タイマーを減少
		m_Player.m_MutekiTimer -= TIME;

		if (m_Player.m_MutekiTimer <= 0.0f)
		{
			// 描画フラグがtrueの時はfalseに
			// falseの時はtrueにする
			if (m_Player.m_Draw == true)
			{
				m_Player.m_Draw = false;
			}
			else
			{
				m_Player.m_Draw = true;
			}

			// 無敵カウントを1つ増やす
			m_Player.m_MutekiCnt++;

			// 無敵タイマーを初期化
			m_Player.m_MutekiTimer = 0.2f;
		}

		if (m_Player.m_MutekiCnt >= 10)
		{
			// 描画フラグ有効化
			m_Player.m_Draw = true;

			// ダメージフラグを無効化
			m_Player.m_Damage = false;
		}
	}
	else
	{
		// 念のためここでも無敵を初期化する
		m_Player.m_MutekiCnt = 0;
		m_Player.m_MutekiTimer = 0.2;
	}
}

void CPlayer::PlayerDeath()
{
	//時間定数宣言.
	const float TIME = 1.0f / FPS;

	if (m_Player.m_Death == true)
	{
		// リスポーンタイムを減少
		m_Player.m_RespawnTimer -= TIME;
		
		// 描画フラグを無効化
		m_Player.m_Draw = false;

		if (m_Player.m_RespawnTimer <= 0.0f)
		{		
			// Hpを初期化
			m_Player.m_Hp = m_Player.m_MaxHp;

			// 描画フラグを有効化
			m_Player.m_Draw = true;

			// リスポーンタイマーを初期化
			m_Player.m_RespawnTimer = 3.0f;

			// リスポーンフラグ有効化
			m_Player.m_Respawn = true;

			// 死亡フラグを無効化
			m_Player.m_Death = false;
		}
	}
}

//// プレイヤーが爆風と当たった時の処理
//void CPlayer::HitPlayer()
//{
//	// プレイヤーの体力を引く
//	m_Player.m_Hp--;
//	if (m_Player.m_Hp <= 0)
//	{
//		// 死亡フラグ有効化
//		m_Player.m_Death = true;
//	}
//	else
//	{
//		// ダメージフラグ有効化
//		m_Player.m_Damage = true;
//	}
//}
//
//// バウンディングオブジェクトを設定
//void CPlayer::SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
//{
//	m_pBody->CreateBounding(pBody);
//	m_pCannon->CreateBounding(pCannon);
//}

// コライダーの作成
void CPlayer::CreateCollider()
{
	m_pBody->CreateBoxCollider(m_pBody->GetMinPos(), m_pBody->GetMaxPos());
	m_pCannon->CreateBoxCollider(m_pCannon->GetMinPos(), m_pCannon->GetMaxPos());
}