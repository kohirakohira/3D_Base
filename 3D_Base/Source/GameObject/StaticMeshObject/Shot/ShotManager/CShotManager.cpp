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
	m_pShots.clear();
	m_pShots.resize(playerCount);

	for (int i = 0; i < playerCount; ++i)
	{
		for (int j = 0; j < ShotMax; ++j)
		{
			auto shot = std::make_shared<CShot>();
			shot->Initialize(j);
			m_pShots[i].push_back(shot);
		}
	}
}

void CShotManager::AttachMeshToPlayerShot(int playerIndex, std::shared_ptr<CStaticMesh> mesh)
{
	if (playerIndex < m_pShots.size())
	{
		for (auto& shot : m_pShots[playerIndex])
		{
			shot->AttachMesh(mesh);
		}
	}
}

void CShotManager::SetReload(int playerIndex, const D3DXVECTOR3& pos, float rotY)
{
	if (playerIndex < m_pShots.size())
	{
		for (auto& shot : m_pShots[playerIndex])
		{
			if (!shot->IsActive())  // 新しくこの関数を CShot に追加してください
			{
				shot->Reload(pos, rotY);
				break;
			}
		}
	}
}

void CShotManager::Update()
{
	for (auto& playerShots : m_pShots)
	{
		for (auto& shot : playerShots)
		{
			shot->Update();
		}
	}
}

void CShotManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& playerShots : m_pShots)
	{
		for (auto& shot : playerShots)
		{
			shot->Draw(View, Proj, Light, Camera);
		}
	}
}
