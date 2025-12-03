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
	for (auto& blast : m_pBlastCollision)
	{
		if (blast->GetBlastFlag() == true)
		{
			blast->Update();
		}
	}
}

//描画処理.
void CBlastCollisionManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->Draw(View, Proj, Light, Camera);
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
	//当たり判定の位置設定.
	blastColl->GetCollider()->SetPosition(pos);

	//情報の保存.
	m_pBlastCollision.push_back(std::move(blastColl));
}

//当たった時の関数.
void CBlastCollisionManager::HitBlast(int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->HitBlast();
		////先頭(一番古いモノ)を削除.
		//m_pBlastCollision.erase(m_pBlastCollision.begin());
	}
}

//爆風メッシュのアタッチ.
void CBlastCollisionManager::AttachMesh(std::shared_ptr<CStaticMesh> mesh, int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->AttachMesh(mesh);
	}
}

//モデルに合わせたバウンディングスフィア作成のラッパー関数
void CBlastCollisionManager::CreateBSphereForMesh(std::shared_ptr<CStaticMesh> mesh, int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->CreateBSphereForMesh(*mesh);
	}
}

//スフィアのコライダーの生成.
void CBlastCollisionManager::CreateSpehreCollider(float rad, int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->CreateSpehreCollider(rad);
	}
}

//位置の設定.
void CBlastCollisionManager::SetPosition(D3DXVECTOR3 pos, int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->SetPosition(pos);
	}
}

//回転の設定.
void CBlastCollisionManager::SetRotation(D3DXVECTOR3 rot, int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->SetRotation(rot);
	}
}

//大きさの設定.
void CBlastCollisionManager::SetScale(float xyz, int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->SetScale(D3DXVECTOR3(xyz, xyz, xyz));
	}
}

//爆発フラグ設定.
void CBlastCollisionManager::SetBlastFlag(bool flg, int index)
{
	if (index >= 0 && index < m_pBlastCollision.size())
	{
		m_pBlastCollision[index]->SetBlastFlag(flg);
	}
}

//爆発の最大半径設定.
void CBlastCollisionManager::SetBlastRadiusMax( float rad)
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->SetBlastRadius(rad);
	}
}

//爆発フラグの取得.
bool CBlastCollisionManager::GetBlastFlag()
{
	for (int i = 0; i < m_pBlastCollision.size(); i++)
	{
		return m_pBlastCollision[i]->GetBlastFlag();
	}
}

//半径の取得.
float CBlastCollisionManager::GetBlastRadius()
{
	for(int i = 0; i < m_pBlastCollision.size(); i++)
	{
		return m_pBlastCollision[i]->GetBlastRadius();
	}
}

//コライダーの取得.
std::shared_ptr<CCollider> CBlastCollisionManager::GetCollider()
{
	for (int i = 0; i < m_pBlastCollision.size(); i++)
	{
		return m_pBlastCollision[i]->GetCollider();
	}
}
