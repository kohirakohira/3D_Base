#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h" // ショットマネージャークラス

CShotManager::CShotManager()
	: m_pShots()
{
	
}

CShotManager::~CShotManager()
{
}

void CShotManager::Initialize(int playerCount)
{
	for (int i = 0; i < ShotMax; ++i)
	{
		m_pShots.push_back(std::make_shared<CShot>());
	}
}

void CShotManager::AttachMeshToPlayerShot(int playerIndex, std::shared_ptr<CStaticMesh> mesh)
{
	//中身が無かったら返す.
	if (mesh == nullptr)
	{
		return;
	}
	m_pShots[playerIndex]->AttachMesh(mesh);
}

void CShotManager::SetReload(int playerIndex, const D3DXVECTOR3& pos, float rotY)
{
	for (auto& shot : m_pShots)
	{
		if (!shot->IsActive())  // 新しくこの関数を CShot に追加してください
		{
			shot->Reload(pos, rotY);
			break;
		}
	}
}

void CShotManager::Update()
{
	for (auto& shot : m_pShots)
	{
		shot->Update();
	}
}

void CShotManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& shot : m_pShots)
	{
		shot->Draw(View, Proj, Light, Camera);
	}
}

void CShotManager::CreateBounding(std::shared_ptr<CStaticMesh>& pShot)
{
	for (auto& shot : m_pShots)
	{
		shot->CreateBSphereForMesh(*pShot);
	}
}

void CShotManager::CreateCollider()
{
	for (auto& shot : m_pShots)
	{
		shot->CreateSpehreCollider(shot->GetRadius());
	}
}

//位置の取得.
const D3DXVECTOR3& CShotManager::GetPosition()
{
	for (auto& shot : m_pShots)
	{
		return shot->GetPosition();
	}
}