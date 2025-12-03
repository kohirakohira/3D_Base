#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h" // ショットマネージャークラス

CShotManager::CShotManager()
	: m_pShots			()
	, m_Mesh			()
{
}

CShotManager::~CShotManager()
{
}

void CShotManager::Initialize()
{
	for (int i = 0; i < ShotMax; ++i)
	{
		m_pShots.push_back(std::make_unique<CShot>());
	}
	m_pShots.reserve(ShotMax);
}

void CShotManager::AttachMeshToPlayerShot(BulletKinds kind, std::shared_ptr<CStaticMesh> mesh)
{
	//中身が無かったら返す
	if (mesh == nullptr)
	{
		return;
	}
	m_Mesh[kind] = mesh;
}

// 動作処理
void CShotManager::Update()
{
	for (auto& shot : m_pShots)
	{
		shot->Update();
	}
}

// 描画処理
void CShotManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (int i = 0; i < m_pShots.size(); ++i)
	{
		m_pShots[i]->Draw(View, Proj, Light, Camera);
	}
}

void CShotManager::HitShot()
{
#ifdef _DEBUG
	std::cout << "削除前の弾数: " << m_pShots.size() << std::endl;
#endif

	// 1. 削除対象の要素を末尾に移動させ、新しい終端イテレータ(it)を取得
	auto it = std::remove_if(
		m_pShots.begin(),
		m_pShots.end(),
		// ラムダ式: Bulletを引数に取り、削除するならtrueを返す
		[](const std::shared_ptr<CShot>& b) {
			return b->HitShot();
		}
	);

	// 2. 新しい終端イテレータ(it)から元の終端までを削除
	m_pShots.erase(it, m_pShots.end());

#ifdef _DEBUG
	std::cout << "削除後の弾数: " << m_pShots.size() << std::endl;
#endif

}

void CShotManager::Create(const D3DXVECTOR3& pos, float rotY, bool shotFlg, int No)
{
	// インスタンス生成
	auto shot = std::make_unique<CShot>();

	// メッシュのアタッチ
	shot->AttachMesh(m_Mesh[No]);

	// 弾の向きの設定
	shot->Reload(pos, rotY);

	// 発射フラグの設定
	shot->SetShotFlag(shotFlg);

	// 当たり判定の生成
	shot->CreateBSphereForMesh(*m_Mesh[No]);

	// 当たり判定の設定
	shot->CreateSpehreCollider(shot->GetRadius());

	// 情報の保存
	m_pShots.push_back(std::move(shot));
}

// モデルに合わせたバウンディングスフィア作成のラッパー関数
void CShotManager::CreateBSphereForMesh(std::shared_ptr<CStaticMesh>& mesh, int index)
{
	if (index >= 0 && index < m_pShots.size())
	{
		m_pShots[index]->CreateBSphereForMesh(*mesh);
	}
}

// スフィアのコライダーの生成
void CShotManager::CreateCollider(int index)
{
	if (index >= 0 && index < m_pShots.size())
	{
		m_pShots[index]->CreateSpehreCollider(m_pShots[index]->GetRadius());
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