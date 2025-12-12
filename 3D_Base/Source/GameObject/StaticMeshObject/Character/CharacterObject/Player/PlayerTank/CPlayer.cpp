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

	, m_PlayerID			()
	, m_HasControl			( false )
	, m_ControllerIndex		()

	, m_Key					()
{

	//キーの生成.
	m_Key = std::make_unique<CMultiInputKeyManager>();
	m_Key->SetKey({ 'W', 'A', 'S', 'D', 'Z', VK_LEFT, VK_RIGHT });
}

CPlayer::~CPlayer() = default;

void CPlayer::Init(int id)
{
	// プレイヤーIDにそれぞれのID番号を入れる
	m_PlayerID = id;

	m_Controller = CControllerManager::GetInstance().GetController(m_PlayerID);

	//インスタンスを生成
	m_pBody = std::make_shared<CBody>(id);
	m_pCannon = std::make_shared<CCannon>(id);

	// プレイヤーの体力に最大体力を入れる
	m_Chara.m_Hp = m_Chara.m_MaxHp;
	// プレイヤーの無敵時間を初期化
	m_Chara.m_MutekiCnt = 0;
	m_Chara.m_MutekiTimer = 0.3;

	// プレイヤーのフラグを初期化
	m_Chara.m_Drawflag = true;
	m_Chara.m_Damage  = false;
	m_Chara.m_Death   = false;
	m_Chara.m_Kill	  = false;
	m_Chara.m_Respawn = false;

	//継承したものも初期化
	m_IsActive = true;
	m_IsAlive = true;
}

void CPlayer::SetPushBack(const D3DXVECTOR3& push)
{
	m_pBody->PushBack(push);
	m_pCannon->PushBack(push);
}

void CPlayer::Update()
{
	//キーの更新.
	m_Key->Update();

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
	Damage();
	// 死亡処理の更新
	Death();

	//playerが死亡していたら処理をスキップ
	if (m_Chara.m_Death == true)
	{
		return;
	}

	//移動とか適用
	UpdateHumanInputAndMove(m_CurrentInput);

	//車体の位置が決まったあとで砲塔の位置を同期
	SyncCannonToBody();
}

void CPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (m_Chara.m_Drawflag == true)
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
}

#if 1
//移動.
void CPlayer::Move(const PlayerInput& input)
{
	//左スティックで移動.
	CController::Direction dir = CController::Direction::None;

	if (m_Controller != nullptr)
	{
		dir = m_Controller->GetLeftStickDirection(0.5f);
	}
	//デフォは停止.
	m_pBody->SetMoveState(CBody::Stop);

	if (dir != CController::Direction::None)
	{
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
	}
	else
	{
		if (m_Key->InputKey('W') == true)
		{
			m_pBody->SetMoveState(CBody::Forward);
		}

		if (m_Key->InputKey('S') == true)
		{
			m_pBody->SetMoveState(CBody::Backward);
		}

		if (m_Key->InputKey('A') == true)
		{
			m_pBody->AddRotationY(-m_Tuning.turretTurnSpeed);
		}

		if (m_Key->InputKey('D') == true)
		{
			m_pBody->AddRotationY(m_Tuning.turretTurnSpeed);
		}

		if (m_Key->InputKey('W') == true && m_Key->InputKey('A') == true)
		{
			m_pBody->AddRotationY(-m_Tuning.turretTurnSpeed);
			m_pBody->SetMoveState(CBody::Forward);
		}

		if (m_Key->InputKey('W') == true && m_Key->InputKey('D') == true)
		{
			m_pBody->AddRotationY(m_Tuning.turretTurnSpeed);
			m_pBody->SetMoveState(CBody::Forward);
		}

		if (m_Key->InputKey('S') == true && m_Key->InputKey('A') == true)
		{
			m_pBody->AddRotationY(-m_Tuning.turretTurnSpeed);
			m_pBody->SetMoveState(CBody::Backward);
		}

		if (m_Key->InputKey('S') == true && m_Key->InputKey('D') == true)
		{
			m_pBody->AddRotationY(m_Tuning.turretTurnSpeed);
			m_pBody->SetMoveState(CBody::Backward);
		}

		if (m_Key->NoInputKey() == false)
		{
			m_pBody->SetMoveState(CBody::Stop);
		}
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
	CController::Direction dir = CController::Direction::None;
	if (!m_pCannon)return;
	if (m_Controller != nullptr)
	{
		dir = m_Controller->GetRightStickDirection(0.5f);
	}

	D3DXVECTOR3 rot = m_pCannon->GetRotation();

	if (dir != CController::Direction::None)
	{
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
	}
	else
	{
		if (m_Key->InputKey(VK_LEFT) == true)
		{
			rot.y -= m_Tuning.turretTurnSpeed;
		}

		if (m_Key->InputKey(VK_RIGHT) == true)
		{
			rot.y += m_Tuning.turretTurnSpeed;
		}

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

	// RT入力があった時 → リロード
	//※押し込み具合：50(0～255).
	if (controller && controller->GetRightTrigger(50) == CController::Trigger::RightTrigger || m_Key->NowInputKey('Z'))
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

//// プレイヤーが爆風と当たった時の処理
//void CPlayer::HitPlayer()
//{
//	// プレイヤーの体力を引く
//	m_Chara.m_Hp--;
//	if (m_Chara.m_Hp <= 0)
//	{
//		// 死亡フラグ有効化
//		m_Chara.m_Death = true;
//	}
//	else
//	{
//		// ダメージフラグ有効化
//		m_Chara.m_Damage = true;
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
