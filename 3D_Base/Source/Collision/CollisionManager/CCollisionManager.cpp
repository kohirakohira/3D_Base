#include "CCollisionManager.h"
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX11ƒNƒ‰ƒX

CCollisionManager::CCollisionManager()
{
	//m_pBody = std::make_shared<CBody>();
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Draw()
{
	m_pBBox->Draw(CDirectX9::GetInstance().GetDevice(), D3DCOLOR_XRGB(0, 255, 0));
}

void CCollisionManager::CheckAllCollisions()
{
}

void CCollisionManager::CreateBounding()
{
	//if (m_pBody)
	//m_pBody->CreateBox(m_pBody->GetPosition(), 10.f, 5.f, 20.f);
}

void CCollisionManager::UpdateBounding()
{
	//m_pBody->UpdateBBoxPos();
}
