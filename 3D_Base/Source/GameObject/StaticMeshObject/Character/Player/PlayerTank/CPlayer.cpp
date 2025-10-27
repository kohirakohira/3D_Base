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
	, m_Hp			( 2 )
	, m_PlayerID	()
	, m_pPad		( nullptr )
	, m_HasControl	( false )
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Initialize(int id)
{
	m_PlayerID = id;

	//インスタンスを生成
	m_pBody = std::make_shared<CBody>(id);
	m_pCannon = std::make_shared<CCannon>(id);
	m_pCannon->Initialize(id);
	m_pBody->Initialize(id);

	auto im = std::make_shared<CInputManager>();	
	SetInputManagerShared(im);
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

void CPlayer::SetTankRotation(const D3DXVECTOR3& pos)
{
	m_pBody->SetRotation(pos);		// 車体回転指定
	m_pCannon->SetRotation(pos);	// 砲塔回転指定
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

	if (m_pPad && m_pPad->IsConnect())
	{
		const float lx = ToStick01(m_pPad->GetLThumbX());
		const float ly = ToStick01(m_pPad->GetLThumbY());
		const float rx = ToStick01(m_pPad->GetRThumbX());

		move = Deadzone(ly, 0.15f);
		turn = Deadzone(lx, 0.15f);
		aim = Deadzone(rx, 0.15f);
	}
	else
	{
	}
#endif

	const float dt = 1.f;	
	const auto& tuning = GetTuning();

	D3DXVECTOR3 pos = m_pBody->GetPosition();
	D3DXVECTOR3 bodyrot = m_pBody->GetRotation();
	D3DXVECTOR3 cannonrot = m_pCannon->GetRotation();

	bodyrot.y += turn * (tuning.bodyTurnSpeed * dt);
	D3DXVECTOR3 fwd(std::sinf(bodyrot.y), 0.f, std::cosf(bodyrot.y));
	pos += fwd * (move * tuning.moveSpeed * dt);
	cannonrot.y += aim * (tuning.turretTurnSpeed * dt);

	m_pBody->SetRotation(bodyrot);
	m_pBody->SetPosition(pos.x,pos.y = 0, pos.z);
	m_pBody->Update();

	SyncCannonToBody();
	D3DXVECTOR3 cannonpos = pos;
	cannonpos.y += tuning.cannonHeight;

	m_pCannon->SetPosition(cannonpos);
	m_pCannon->SetRotation(cannonrot);
	m_pCannon->Update();
}

//砲塔と車体を同期する
void CPlayer::SyncCannonToBody()
{
	auto tuning = GetTuning();
	auto body = Body();
	auto cannon = Cannon();
	if (!body || !cannon) return;

	D3DXVECTOR3 pos = body->GetPosition();
	pos.y += tuning.cannonHeight;   //砲塔の高さオフセット
	cannon->SetPosition(pos);       //位置を同期

}

void CPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	m_pBody->Draw(View, Proj, Light, Camera);
	m_pCannon->Draw(View, Proj, Light, Camera);
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


