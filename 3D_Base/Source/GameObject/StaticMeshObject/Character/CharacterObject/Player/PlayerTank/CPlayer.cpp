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

	, m_PlayerID		()
	, m_HasControl		( false )
	, m_ControllerIndex	()
{
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

	m_Controller = CControllerManager::GetInstance().GetController(m_PlayerID);

	//インスタンスを生成
	m_pBody = std::make_shared<CBody>(id);
	m_pCannon = std::make_shared<CCannon>(id);
	m_pCannon->Init();
	m_pBody->Init();

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
	m_Drawflag = true;
	m_Damage = false;
	m_Death = false;
	m_Respawn = false;
	m_IsActive = true;
	m_IsAlive = true;
}

void CPlayer::SetTankPosition(const D3DXVECTOR3& pos)
{	

	m_pBody->SetPosition(pos);		// 車体座標指定
	m_pCannon->SetPosition(pos);	// 砲塔座標指定
}

void CPlayer::SetTankRotation(const D3DXVECTOR3& rot)
{
	m_pBody->SetRotation(rot);		// 車体回転指定
	m_pCannon->SetRotation(rot);	// 砲塔回転指定
}

void CPlayer::SetTankScale(const D3DXVECTOR3& sca)
{	
	m_pBody->SetScale(sca);			// 車体大きさ指定
	m_pCannon->SetScale(sca);		// 砲塔大きさ指定
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

void CPlayer::Create(int index)
{
}

void CPlayer::SetPosition(D3DXVECTOR3 pos)
{
}

const D3DXVECTOR3 CPlayer::GetPosition()
{
	if (m_pBody) return m_pBody->GetPosition(); //常にbodyの実位置を返す
	return CCharacter::GetPosition();
}


void CPlayer::SetRotation(D3DXVECTOR3 rot)
{
}

const D3DXVECTOR3 CPlayer::GetRotation()
{
	if (m_pBody) return m_pBody->GetRotation();;
	return CCharacter::GetRotation();
}

void CPlayer::SetScale(D3DXVECTOR3 sca)
{
}

const D3DXVECTOR3 CPlayer::GetScale()
{
	return D3DXVECTOR3();
}


#if 1
//移動.
void CPlayer::Move(const PlayerInput& input)
{
	//左スティックで移動.
	auto dir = m_Controller->GetLeftStickDirection(0.5f);
	//現在の情報を取得・保存.
	auto& tuning = GetTuning();
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
		m_pBody->AddRotationY(-tuning.turretTurnSpeed);
		break;
	case CController::Direction::Right:
		m_pBody->AddRotationY(tuning.turretTurnSpeed);
		break;
	case CController::Direction::UpLeft:
		m_pBody->AddRotationY(-tuning.turretTurnSpeed);
		m_pBody->SetMoveState(CBody::Forward);
		break;
	case CController::Direction::UpRight:
		m_pBody->AddRotationY(tuning.turretTurnSpeed);
		m_pBody->SetMoveState(CBody::Forward);
		break;
	case CController::Direction::DownLeft:
		m_pBody->AddRotationY(-tuning.turretTurnSpeed);
		m_pBody->SetMoveState(CBody::Backward);
		break;
	case CController::Direction::DownRight:
		m_pBody->AddRotationY(tuning.turretTurnSpeed);
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
//砲塔回転.
void CPlayer::Rotate(const PlayerInput& input)
{
	// 必要なポインタがなければ何もしない
	if (!m_pCannon || !m_Controller) return;
	if (!m_Controller->CheckConnected()) return;

	// ある程度スティックが倒れていないなら回転しない
	auto dir = m_Controller->GetRightStickDirection(0.5f);
	if (dir == CController::Direction::None) {
		return;
	}

	auto& tuning = GetTuning();

	// 右スティックの生のXY値を取得
	D3DXVECTOR2 stick = {
		m_Controller->GetRightStickX(),
		m_Controller->GetRightStickY()
	};

	// 完全にゼロなら何もしない（安全策）
	if (stick.x == 0.f && stick.y == 0.f) {
		return;
	}

	// スティック方向から「目標の角度」を求める
	// ※座標系の都合で x,y の順にしている
	const float targetAngle = std::atan2f(stick.x, stick.y);

	// いまの砲塔の角度
	D3DXVECTOR3 rot = m_pCannon->GetRotation();

	// 差分を求める
	float diff = targetAngle - rot.y;
	const float twoPi = D3DX_PI * 2.0f;

	// 差分を [-π, π] に正規化
	if (diff > D3DX_PI)       diff -= twoPi;
	else if (diff < -D3DX_PI) diff += twoPi;

	const float step = tuning.turretTurnSpeed;

	// 一歩で追いつけるなら一気に合わせる
	if (std::fabs(diff) <= step)
	{
		rot.y = targetAngle;
	}
	else if (diff > 0.f)
	{
		// 目標が正方向にある → 正方向に少し回す
		rot.y += step;
	}
	else
	{
		// 目標が負方向にある → 負方向に少し回す
		rot.y -= step;
	}

	// rot.y を [0, 2π] に正規化
	if (rot.y > twoPi)
		rot.y -= twoPi;
	else if (rot.y < 0.f)
		rot.y += twoPi;

	// 砲塔に反映
	m_pCannon->SetRotation(rot);
}

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
	auto& tuning = GetTuning();

	D3DXVECTOR3 rot = m_pCannon->GetRotation();

	switch (dir)
	{
	case CController::Direction::Left:
	case CController::Direction::UpLeft:
	case CController::Direction::DownLeft:
		rot.y -= tuning.turretTurnSpeed;
		break;

	case CController::Direction::Right:
	case CController::Direction::UpRight:
	case CController::Direction::DownRight:
		rot.y += tuning.turretTurnSpeed;
		break;

	default:
		break;
	}

	m_pCannon->SetRotation(rot);
	
	
}

void CPlayer::UpdateHumanInputAndMove(PlayerInput input)
{
#if 1
	//松岡.
	{
		//コントローラー番号を取得.
		int index = GetControllerIndex();

		//そのコントローラーの入力を取得.
		CController* m_Controller = CControllerManager::GetInstance().GetController(index);

		//コントローラーが接続されていなければ通らない.
		if (!m_Controller)return;

		//コントローラーがあり、ボタンを押されたか.
		if (m_Controller != nullptr && m_Controller->CheckConnected())
		{
			//LSHICK入力があった時.
			//移動.
			Move(input);
			//砲塔回転.
			Rotate(input);
			
			//RB入力があった時.
			if (m_Controller->Down(CXInput::RB, true))
			{
				//リロード.
				Reload(m_pCannon->GetCannonPosition(), m_pCannon->GetRotation().y);
			}
		}
		SyncCannonToBody();

		m_pBody->Update();
		m_pCannon->Update();
	}
#endif
}

//砲塔と車体を同期する
void CPlayer::SyncCannonToBody()
{
	auto tuning = GetTuning();
	if (!Body() || !Cannon()) return;

	D3DXVECTOR3 pos = Body()->GetPosition();
	pos.y += tuning.cannonHeight;		// 砲塔の高さオフセット
	Cannon()->SetPosition(pos);			// 位置を同期
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

// プレイヤーが爆風と当たった時の処理
void CPlayer::HitPlayer()
{
	// プレイヤーの体力を引く
	m_Player.m_Hp--;
	if (m_Player.m_Hp <= 0)
	{
		// 死亡フラグ有効化
		m_Player.m_Death = true;
	}
	else
	{
		// ダメージフラグ有効化
		m_Player.m_Damage = true;
	}
}

// バウンディングオブジェクトを設定
void CPlayer::SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_pBody->CreateBounding(pBody);
	m_pCannon->CreateBounding(pCannon);
}

// コライダーの作成
void CPlayer::CreateCollider()
{
	m_pBody->CreateBoxCollider(m_pBody->GetMinPos(), m_pBody->GetMaxPos());
	m_pCannon->CreateBoxCollider(m_pCannon->GetMinPos(), m_pCannon->GetMaxPos());
}

//アタッチメッシュ
void CPlayer::AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_pBody->AttachMesh(pBody);
	m_pCannon->AttachMesh(pCannon);
}
