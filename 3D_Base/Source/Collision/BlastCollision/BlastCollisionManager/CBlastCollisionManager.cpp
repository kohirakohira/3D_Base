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
		blast->Update();
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
void CBlastCollisionManager::Create()
{
	for (int i = 0; i < ShotMax; i++)
	{
		//インスタンス生成.
		m_pBlastCollision.push_back(std::make_shared<CBlastCollision>());
	}
}

//当たった時の関数.
void CBlastCollisionManager::HitBlast()
{
	for (auto& blast : m_pBlastCollision)
	{
		blast->HitBlast();
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
	for (auto& blast : m_pBlastCollision)
	{
		blast->SetScale(xyz);
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
