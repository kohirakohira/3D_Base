#include "CBlastCollisionManager.h"

CBlastCollisionManager::CBlastCollisionManager()
	: m_pBlastCollision				()
{
}

CBlastCollisionManager::~CBlastCollisionManager()
{
}

//動作処理.
void CBlastCollisionManager::Update()
{
	for (int i = 0; i < m_pBlastCollision.size(); i++)
	{
		m_pBlastCollision[i]->Update();
	}
}

//描画処理.
void CBlastCollisionManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (int i = 0; i < m_pBlastCollision.size(); i++)
	{
		m_pBlastCollision[i]->Draw(View, Proj, Light, Camera);
	}
}

//インスタンス生成.
void CBlastCollisionManager::Create(const D3DXVECTOR3& pos, bool blast, std::shared_ptr<CStaticMesh> mesh)
{
	//インスタンス生成.
	auto blastColl = std::make_unique<CBlastCollision>();

	//メッシュのアタッチ.
	blastColl->AttachMesh(mesh);
	//位置の設定.
	blastColl->SetPosition(pos);
	//爆発フラグの設定.
	blastColl->SetBlastFlag(blast);
	//当たり判定の生成.
	blastColl->CreateBSphereForMesh(*mesh);
	//当たり判定の設定.
	blastColl->CreateSpehreCollider(GetBlastRadius());

	//情報の保存.
	m_pBlastCollision.push_back(std::move(blastColl));
}

//当たった時の関数.
void CBlastCollisionManager::HitBlast()
{
	for (int i = 0; i < m_pBlastCollision.size(); i++)
	{
		m_pBlastCollision[i]->HitBlast();
		////先頭(一番古いモノ)を削除.
		//m_pBlastCollision.erase(m_pBlastCollision.begin());
	}
}

//爆風メッシュのアタッチ.
void CBlastCollisionManager::AttachMesh(std::shared_ptr<CStaticMesh> mesh)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->AttachMesh(mesh);
	}
}

//モデルに合わせたバウンディングスフィア作成のラッパー関数
void CBlastCollisionManager::CreateBSphereForMesh(std::shared_ptr<CStaticMesh> mesh)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->CreateBSphereForMesh(*mesh);
	}
}

//スフィアのコライダーの生成.
void CBlastCollisionManager::CreateSpehreCollider(float rad)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->CreateSpehreCollider(rad);
	}
}

//位置の設定.
void CBlastCollisionManager::SetPosition(D3DXVECTOR3 pos)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->SetPosition(pos);
	}
}

//回転の設定.
void CBlastCollisionManager::SetRotation(D3DXVECTOR3 rot)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->SetRotation(rot);
	}
}

//大きさの設定.
void CBlastCollisionManager::SetScale(float xyz)
{
	for (int i = 0; i < m_pBlastCollision.size(); i++)
	{
		m_pBlastCollision[i]->SetScale(xyz);
	}
}

//爆発フラグ設定.
void CBlastCollisionManager::SetBlastFlag(bool flg)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->SetBlastFlag(flg);
	}
}

//爆発フラグの取得.
bool CBlastCollisionManager::GetBlastFlag()
{
	for (auto& blast : m_pBlastCollision)
	{
		return blast->GetBlastFlag();
	}
}

//半径の取得.
float CBlastCollisionManager::GetBlastRadius()
{
	for (auto& blast : m_pBlastCollision)
	{
		return blast->GetBlastRadius();
	}
}

//コライダーの取得.
std::shared_ptr<CCollider> CBlastCollisionManager::GetCollider()
{
	for (auto& blast : m_pBlastCollision)
	{
		return blast->GetCollider();
	}
}
