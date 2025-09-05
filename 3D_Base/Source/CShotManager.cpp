#include "CShotManager.h"

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

	for (int i = 0; i < playerCount; ++i)
	{
		std::vector<std::unique_ptr<CShot>> playerShots;
		for (int j = 0; j < ShotMax; ++j)
		{
			auto shot = std::make_unique<CShot>();
			shot->Initialize(j);
			playerShots.push_back(std::move(shot));
		}
		m_pShots.push_back(std::move(playerShots));
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
			if (!shot->IsActive())  // V‚µ‚­‚±‚ÌŠÖ”‚ð CShot ‚É’Ç‰Á‚µ‚Ä‚­‚¾‚³‚¢
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
