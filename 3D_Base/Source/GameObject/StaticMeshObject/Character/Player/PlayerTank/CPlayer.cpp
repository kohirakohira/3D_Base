#include "CPlayer.h"

CPlayer::CPlayer()
	: m_pBody	( nullptr )
	, m_pCannon	( nullptr )
	, m_Hp		( 2 )
	, m_PlayerID()
	, m_pPad()
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

void CPlayer::UpdateHumanInputAndMove()
{
	if (!m_pBody || !m_pCannon) return;	//ボディとキャノンのポインタがなければなにもしない

	//キーが押されたかチェック
	auto isPressed = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };

	//移動・旋回・砲塔旋回の入力
	const float moveSpeed = 0.001f;		//移動速度
	const float turnSpeed = 0.001f;		//車体旋回速度
	const float aimSpeed = 0.001f;		//砲塔旋回速度

	float move = (isPressed('W') ? +1.f : 0.f) + (isPressed('S') ? -1.f : 0.f);
	float turn = (isPressed('D') ? +1.f : 0.f) + (isPressed('A') ? -1.f : 0.f);
	float aim = (GetAsyncKeyState(VK_RIGHT) & 0x8000 ? +1.f : 0.f)
		+ (GetAsyncKeyState(VK_LEFT) & 0x8000 ? -1.f : 0.f);

#if 1
	//現在値
	D3DXVECTOR3 pos = m_pBody->GetPosition();
	D3DXVECTOR3 brot = m_pBody->GetRotation();
	D3DXVECTOR3 crot = m_pCannon->GetRotation();

	//車体の旋回
	brot.y += turn * turnSpeed;

	//前進/後退
	D3DXVECTOR3 fwd(std::sinf(brot.y), 0.f, std::cosf(brot.y));
	pos += fwd * (move * moveSpeed);

	//砲塔
	crot.y += aim * aimSpeed;	

	// 反映
	m_pBody->SetRotation(brot);
	m_pBody->SetPosition(pos);
	m_pBody->Update();

	//砲塔の位置を車体上に合わせる
	D3DXVECTOR3 cpos = pos; cpos.y += 0.3f;
	m_pCannon->SetPosition(cpos);
	m_pCannon->SetRotation(crot);
	m_pCannon->Update();
#endif


#if 0
	m_pBody->Update();

	// 砲塔の位置を更新
	D3DXVECTOR3 pos = m_pBody->GetPosition();
	pos.y += 0.3f; // 砲塔の座標を合わせる
	m_pCannon->SetPosition(pos); // 砲塔座標更新

	m_pCannon->Update();
#endif



#if 0
	//コントローラーの入力
	if (!m_pBody || !m_pCannon) return;
	float padmove = 0.f;
	float padturn = 0.f;
	float padaim = 0.f;

	if (m_pPad && m_pPad->IsConnect())
	{
		m_pPad->GetLThumbX();
		m_pPad->GetLThumbY();
		m_pPad->GetRThumbX();

	}
#endif

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


