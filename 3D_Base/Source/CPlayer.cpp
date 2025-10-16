#include "CPlayer.h"

CPlayer::CPlayer()
	: m_pBody	( nullptr )
	, m_pCannon	( nullptr )
	, m_Hp		( 2 )
	, m_PlayerID()
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
   
void CPlayer::Update()
{
	m_pBody->Update();

	// 砲塔の位置を更新
	D3DXVECTOR3 pos = m_pBody->GetPosition();
	pos.y += 0.3f; // 砲塔の座標を合わせる
	m_pCannon->SetPosition(pos); // 砲塔座標更新

	m_pCannon->Update();
}

void CPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	m_pBody->Draw(View, Proj, Light, Camera);
	m_pCannon->Draw(View, Proj, Light, Camera);
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
	if (m_pBody)return m_pBody->GetPosition();
	return CCharacter::GetRotation();
}


