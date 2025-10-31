#include "CCollisionManager.h"

CCollisionManager::CCollisionManager()
	: m_pCollisionWallW		()
	, m_pCollisionWallH		()
	, m_pCollisionGround	()
	, m_pCollisionWoodBox	()
	, m_pCollisionItemBox	()
	, m_pCollisionShot		()
	, m_pCollisionCannon	()
	, m_pCollisionBody		()

	// 壁
	, m_pWallTop			()
	, m_pWallBottom			()
	, m_pWallLeft			()
	, m_pWallRight			()

	// 地面
	, m_pGround				()

	// 木箱
	, m_pWoodBoxTopLeft		()
	, m_pWoodBoxTopRight	()
	, m_pWoodBoxCenter		()
	, m_pWoodBoxBottomLeft	()
	, m_pWoodBoxBottomRight	()

	// 弾マネージャー
	, m_pShotManager		()

	// プレイヤーマネージャー
	, m_pPlayerManager		()
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::Update()
{
}

void CCollisionManager::Create()
{
}

HRESULT CCollisionManager::LoadData()
{
//--------------------------------------------------------------------------.
// 	   メッシュの読み込み
//--------------------------------------------------------------------------.
	//　スタティックメッシュの読み込み
	m_pCollisionWallW->Init(_T("Data\\Collision\\Wall1.x")); // 横向き壁
	m_pCollisionWallH->Init(_T("Data\\Collision\\Wall2.x")); // 縦向き壁

	return S_OK;
}

void CCollisionManager::WalltoPlayer()
{
}

void CCollisionManager::WalltoShot()
{
}

void CCollisionManager::PlayertoPlayer()
{
}

void CCollisionManager::PlayertoItemBox()
{
}

void CCollisionManager::PlayertoShot()
{
}

void CCollisionManager::ShottoShot()
{
}

void CCollisionManager::WoodBoxtoPlayer()
{
}

void CCollisionManager::WoodBoxtoShot()
{
}

void CCollisionManager::GroundtoShot()
{
}

void CCollisionManager::GroundtoItemBox()
{
}

void CCollisionManager::PlayertoBlast()
{
}
