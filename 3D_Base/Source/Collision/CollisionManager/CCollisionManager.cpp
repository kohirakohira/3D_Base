#include "CCollisionManager.h"
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX11クラス

CCollisionManager::CCollisionManager()
{

}

CCollisionManager::~CCollisionManager()
{
	m_pStaticMeshBSphere = std::make_shared<CStaticMesh>();
}

void CCollisionManager::Draw()
{

	m_pStaticMeshBSphere->Render(m_pCamera->m_mView , m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera.vPosition);
	//m_pBBox->Draw(CDirectX9::GetInstance().GetDevice(), D3DCOLOR_XRGB(0, 255, 0));
}

void CCollisionManager::Create()
{
	m_pBBox = std::make_shared<CBoundingBox>();
	m_pCamera = std::make_shared<CCamera>();
}

void CCollisionManager::CreateTank(int id)
{
	m_pBody = std::make_shared<CBody>(id);
}

HRESULT CCollisionManager::LoadData()
{
	////バウンディングスフィア(当たり判定用).
	//m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Sphere.x"));

	//// バウンディングボックス(当たり判定用)
	//m_pStaticMeshBBox->Init(_T("Data\\Collision\\Box.x"));

	//バウンディングスフィアの作成.
	m_pBody->CreateBBoxForMesh(*m_pStaticMeshBBox);

	return S_OK;
}

void CCollisionManager::UpdateBounding()
{
	m_pBody->UpdateBBoxPos();
	//m_pBody->UpdateBSpherePos();
}

void CCollisionManager::CheckAllCollisions()
{
}

