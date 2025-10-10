#include "CCollisionManager.h"

CCollisionManager::CCollisionManager()
	:	m_KeyInput		( nullptr )
	,	m_Draw			( false )
{

}

CCollisionManager::~CCollisionManager()
{

}

void CCollisionManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{	
	////描画フラグ切り替え
	//if (m_KeyInput->ReleaseInputKey('P') == true)
	//{
	//	if (m_Draw == true)
	//	{
	//		m_Draw = false;
	//	}
	//	else
	//	{
	//		m_Draw = true;
	//	}
	//}
	m_pStaticMeshBSphere->SetRotation(D3DXVECTOR3(0.f, 0.f, 0.f)); // ← 追加

	m_Draw = true;
	// フラグがtureの時だけ描画
	if (m_Draw == true)
	{
		//// 戦車の車体描画
		//for (int i = 0; i < PLAYER_MAX; ++i)
		//{
		//	if (auto player = m_pPlayerManager->GetControlPlayer(i))
		//	{
		//		m_pStaticMeshBSphere->SetPosition(player->GetBodyPosition());
		//	}
		//	m_pStaticMeshBSphere->Render(View, Proj, Light, Camera.vPosition);
		//}

		// 戦車の砲塔の描画
		for (int i = 0; i < PLAYER_MAX; ++i)
		{
			if (auto player = m_pPlayerManager->GetControlPlayer(i))
			{
				m_pStaticMeshBSphere->SetPosition(player->GetCannonPosition());
			}
			m_pStaticMeshBSphere->Render(View, Proj, Light, Camera.vPosition);
		}
	}
}

void CCollisionManager::Create()
{
	m_pStaticMeshBSphere = std::make_shared<CStaticMesh>();
	//m_pStaticMeshBBox = std::make_shared<CStaticMesh>();

	// キー入力.
	m_KeyInput = std::make_shared<CMultiInputKeyManager>();

	//キー入力の初期化.
	m_KeyInput->Init();
	m_KeyInput->SetKey({ 'P' });
}

HRESULT CCollisionManager::LoadData()
{
	//バウンディングスフィア(当たり判定用).
	m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Body.x"));

	// バウンディングボックス(当たり判定用)
	//m_pStaticMeshBBox->Init(_T("Data\\Collision\\OtamesiBox.x"));

	//バウンディングスフィアの作成.
	//m_pBody->CreateBBoxForMesh(*m_pStaticMeshBBox);
	//m_pBody->CreateBSphereForMesh(*m_pStaticMeshBSphere);

		// プレイヤーごとにバウンディング作成
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		m_pPlayerManager->CreateBounding(index, m_pStaticMeshBSphere, m_pStaticMeshBSphere);
	}

	//m_pStaticMeshBSphere->SetRotation();

	//m_pBody->CreateBounding(m_pStaticMeshBBox);

	return S_OK;
}

void CCollisionManager::UpdateBounding()
{
	//キー入力受付.
	m_KeyInput->Update();

	// プレイヤーごとにバウンディング座標を更新
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		m_pPlayerManager->UpdateBounding(index);
	}
}

void CCollisionManager::CheckAllCollisions()
{
}

void CCollisionManager::SetPlayerManager(std::shared_ptr<CPlayerManager> pPlayerMgr)
{
	m_pPlayerManager = pPlayerMgr;
}

