#if 0

#include "CCollisionManager.h"
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX11�N���X

CCollisionManager::CCollisionManager()
{
	//m_pBody = std::make_shared<CBody>();
}

CCollisionManager::~CCollisionManager()
{
	m_pStaticMeshBSphere = std::make_shared<CStaticMesh>();
}

void CCollisionManager::Draw()
{

	//m_pBBox->Draw(CDirectX9::GetInstance().GetDevice(), D3DCOLOR_XRGB(0, 255, 0));
}

void CCollisionManager::Create()
{
	//�o�E���f�B���O�X�t�B�A�̍쐬.
	m_pBody->CreateBSphereForMesh(*m_pStaticMeshBSphere);

	//if (m_pBody)
	//m_pBody->CreateBox(m_pBody->GetPosition(), 10.f, 5.f, 20.f);
}

HRESULT CCollisionManager::LoadData()
{
	//�o�E���f�B���O�X�t�B�A(�����蔻��p).
	m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Sphere.x"));

	return S_OK;
}

void CCollisionManager::UpdateBounding()
{
	//m_pBody->UpdateBBoxPos();
	m_pBody->UpdateBSpherePos();
}

void CCollisionManager::CheckAllCollisions()
{
}

#endif 

