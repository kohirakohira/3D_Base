#include "CCollisionManager.h"

CCollisionManager::CCollisionManager()
	:	m_KeyInput			( nullptr )
	,	m_Draw				( false )
	,	m_pCannonBBox		( nullptr )
	,	m_pBodyBBox			( nullptr )
	,	m_pPlayerManager	( nullptr )
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


	// 戦車の砲塔の描画
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(index))
		{
			m_pCannonBBox->SetPosition(player->GetCannon()->GetPosition());
			m_pCannonBBox->SetRotation(player->GetCannon()->GetRotation());
		}
		// フラグがtureの時だけ描画
		if (m_Draw == true)
		{
			m_pCannonBBox->Render(View, Proj, Light, Camera.vPosition);
		}
	}

	// 戦車の車体の描画
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(index))
		{
			m_pBodyBBox->SetPosition(player->GetCannonPosition());
			m_pBodyBBox->SetRotation(player->GetBody()->GetRotation());
		}
		// フラグがtureの時だけ描画
		if (m_Draw == true)
		{
			m_pBodyBBox->Render(View, Proj, Light, Camera.vPosition);
		}
	}
}

void CCollisionManager::Create()
{
	m_pStaticMeshBSphere = std::make_shared<CStaticMesh>();

	// バウンディングボックスのインスタンス生成
	m_pCannonBBox = std::make_shared<CStaticMesh>();
	m_pBodyBBox	  = std::make_shared<CStaticMesh>();

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
	m_pCannonBBox->Init(_T("Data\\Collision\\Body.x"));
	m_pBodyBBox  ->Init(_T("Data\\Collision\\Body.x"));

	// プレイヤーごとにバウンディング作成
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		m_pPlayerManager->CreateBounding(index, m_pBodyBBox, m_pCannonBBox);
	}

	//m_pStaticMeshBSphere->SetRotation();

	return S_OK;
}

void CCollisionManager::UpdateBounding()
{
	// キー入力受付
	m_KeyInput->Update();

	// プレイヤーごとにバウンディング座標を更新
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		m_pPlayerManager->UpdateBounding(index);
	}

	// 当たり判定の向き更新
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(index))
		{
			auto pbody = player->GetBody();
		}
	}
}

void CCollisionManager::CheckAllCollisions()
{
}

void CCollisionManager::SetPlayerManager(std::shared_ptr<CPlayerManager> pPlayerMgr)
{
	m_pPlayerManager = pPlayerMgr;
}

