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
	: m_pBody		( nullptr )
	, m_pCannon		( nullptr )
	, m_PlayerID	()
	, m_HasControl	( false )
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

CPlayer::~CPlayer()
{
}

void CPlayer::Initialize(int id)
{
	// プレイヤーIDにそれぞれのID番号を入れる
	m_PlayerID = id;

	//インスタンスを生成
	m_pBody = std::make_shared<CBody>(id);
	m_pCannon = std::make_shared<CCannon>(id);
	m_pCannon->Initialize(id);
	m_pBody->Initialize(id);

	auto im = std::make_shared<CInputManager>();	
	SetInputManagerShared(im);

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
}

//共有
void CPlayer::SetInputManagerShared(const std::shared_ptr<CInputManager>& im)
{
	m_Input = im;
	if (m_pBody)
	{
		m_pBody->SetInputManager(m_Input);
	}
	if (m_pCannon)
	{
		m_pCannon->SetInputManager(m_Input);
	}
}

void CPlayer::SetKeyboardEnabled(bool on)
{
	if (m_Input)
	{
		m_Input->SetUseKeyboard(on);
	}
}


void CPlayer::AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon)
{
	m_pBody->AttachMesh(pBody);
	m_pCannon->AttachMesh(pCannon);
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

void CPlayer::SetTankScale(const float& sca)
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
	//操作権がない時は入力を読まない
	if (!m_HasControl)
	{
		if (m_pBody)   m_pBody->CCharacter::Update();
		if (m_pCannon) m_pCannon->CCharacter::Update();
		return;
	}

	// ダメージ処理の更新
	PlayerDamage();
	// 死亡処理の更新
	PlayerDeath();

	//移動とか適用
	UpdateHumanInputAndMove();

}

void CPlayer::SetTune(const TankTuning& info)
{
	m_pBody->SetTuning(info);
	m_pCannon->SetTuning(info);
}

void CPlayer::UpdateHumanInputAndMove()
{
#if 1
	//pad入力
	if (!m_pBody || !m_pCannon) return;

	float move = 0.f, turn = 0.f, aim = 0.f;

	//if (m_pPad && m_pPad->IsConnect())
	//{
	//	const float lx = ToStick01(m_pPad->GetLThumbX());
	//	const float ly = ToStick01(m_pPad->GetLThumbY());
	//	const float rx = ToStick01(m_pPad->GetRThumbX());

		move = Deadzone(ly, 0.15f);
		turn = Deadzone(lx, 0.15f);
		aim = Deadzone(rx, 0.15f);
	}
	else
	{
	}
#endif

	const float dt = 1.f / FPS;	
	//const auto& tuning = GetTuning();

	D3DXVECTOR3 pos = m_pBody->GetPosition();
	D3DXVECTOR3 bodyrot = m_pBody->GetRotation();
	D3DXVECTOR3 cannonrot = m_pCannon->GetRotation();

	bodyrot.y += turn * (m_Tune.bodyTurnSpeed * dt);
	D3DXVECTOR3 fwd(std::sinf(bodyrot.y), 0.f, std::cosf(bodyrot.y));
	pos += fwd * (move * m_Tune.moveSpeed * dt);
	cannonrot.y += aim * (m_Tune.turretTurnSpeed * dt);

	m_pBody->SetRotation(bodyrot);
	m_pBody->SetPosition(pos.x, pos.y = 0, pos.z);
	m_pBody->Update();

	//砲塔と車体を同期する.
	SyncCannonToBody();
	{
		//この下のコメントを外したら、車体と砲塔が別々に動く.
		//D3DXVECTOR3 cannonpos = pos;
		//cannonpos.y += tuning.cannonHeight;
		//m_pCannon->SetPosition(cannonpos);
	}
	m_pCannon->SetRotation(cannonrot);
	m_pCannon->Update();
}

//砲塔と車体を同期する
void CPlayer::SyncCannonToBody()
{
	//auto tuning = GetTuning();
	if (!Body() || !Cannon()) return;

	D3DXVECTOR3 pos = Body()->GetPosition();
	pos.y += m_Tune.cannonHeight;		// 砲塔の高さオフセット
	Cannon()->SetPosition(pos);			// 位置を同期
}

void CPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if (m_Player.m_Draw == true)
	{
		m_pBody->Draw(View, Proj, Light, Camera);
		m_pCannon->Draw(View, Proj, Light, Camera);
	}
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

D3DXVECTOR3 CPlayer::GetCannonPosition() const
{
	if (m_pCannon)
	{
		return m_pCannon->GetPosition();
	}
	else
	{
		return GetPosition();
	}
}

float CPlayer::GetCannonYaw() const
{
	if (m_pCannon)
	{
		return m_pCannon->GetRotation().y;
	}
	else
	{
		return GetRotation().y;
	}
}

D3DXVECTOR3 CPlayer::GetPosition() const
{
	if (m_pBody)return m_pBody->GetPosition();	//常にbodyの実位置を返す
	return CCharacter::GetPosition();
}

D3DXVECTOR3 CPlayer::GetRotation() const
{
	if (m_pBody)return m_pBody->GetRotation();
	return CCharacter::GetRotation();
}


