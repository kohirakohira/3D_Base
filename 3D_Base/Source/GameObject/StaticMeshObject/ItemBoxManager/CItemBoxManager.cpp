//全アイテムのリセット.
#include "CItemBoxManager.h"

CItemBoxManager::CItemBoxManager()
	: m_Item			()
	, m_ItemInfo		()
{
}

CItemBoxManager::~CItemBoxManager()
{
}

void CItemBoxManager::Update()
{
	for (auto& item : m_Item)
	{
		item->Update();
	}

	//無くなったアイテムを削除.
	m_Item.erase(
		std::remove_if(
			m_Item.begin(), m_Item.end(),
			[](const std::shared_ptr<CItemBox>& item)
			{
				return !item->IsActive();
			}),
		m_Item.end());
}

void CItemBoxManager::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	for (auto& item : m_Item)
	{
		item->Draw(View, Proj, Light, Camera);
	}
}

void CItemBoxManager::Create()
{
	//アイテムの最大数以下なら生成.
	if (m_Item.size() < ITEM_MAX)
	{
		//アイテムボックスのインスタンス生成.
		std::unique_ptr<CItemBox> item = std::make_unique<CItemBox>();
		//メッシュの設定.
		item->AttachMesh(m_ItemMesh);
		//各設定.
		D3DXVECTOR3 pos = ItemPositionRandom();
		item->SetPosition(pos.x, pos.y, pos.z);
		item->SetRotation(0.0f, 0.0f, 0.0f);
		item->SetScale(0.2f);
		//当たり判定の設定.
		item->CreateBBoxForMesh(*m_ItemMesh);
		item->CreateBoxCollider(item->GetMinPos(), item->GetMaxPos());
		//アイテムのインスタンスを移動.
		m_Item.push_back(std::move(item));
	}
}

void CItemBoxManager::Clear()
{
	//全アイテムのリセット.
	m_Item.clear();
}

void CItemBoxManager::AttachMesh(std::shared_ptr<CStaticMesh> pMesh)
{
	//メッシュ設定.
	if (pMesh == nullptr)
	{
		return;
	}
	m_ItemMesh = pMesh;
}

void CItemBoxManager::CreateBounding(std::shared_ptr<CStaticMesh>& pItem)
{
	// バウンディング設定.
	for (auto& item : m_Item)
	{
		item->CreateBounding(pItem);
	}
}

//void CItemBoxManager::CreateCollider()
//{
//	// コライダー設定.
//	for (auto& item : m_Item)
//	{
//		item->CreateBoxCollider(item->GetMinPos(), item->GetMaxPos());
//	}
//}

void CItemBoxManager::SetPosition(float x, float y, float z)
{
	for (auto& item : m_Item)
	{
		item->SetPosition(x, y, z);
	}
}

void CItemBoxManager::SetRotation(float x, float y, float z)
{
	for (auto& item : m_Item)
	{
		//回転設定.
		item->SetRotation(x, y, z);
	}
}
void CItemBoxManager::SetRotation(D3DXVECTOR3 xyz)
{
	for (auto& item : m_Item)
	{
		//回転設定.
		item->SetRotation(xyz);
	}
}

void CItemBoxManager::SetScale(float x, float y, float z)
{
	for (auto& item : m_Item)
	{
		//大きさ設定.
		item->SetScale(x, y, z);
	}
}

//重力の有無を設定.
void CItemBoxManager::SetGravity(bool flg)
{
	for (auto& item : m_Item)
	{
		//重力の有無を設定.
		item->SetGravity(flg);
	}
}

void CItemBoxManager::SetItemInfo(int index)
{	
	if (index >= 0 && index < m_Item.size())
	{
		//中身を順番に設定.
		//ランダムで設定.
		m_ItemInfo = ItemRandom();

		m_Item[index]->SetItemInfo(m_ItemInfo);
	}
}

CItemType CItemBoxManager::ItemRandom()
{
	//staticを付けるのは、毎回作っていると、処理速度が低下するから、一度だけ作成.
	
	//シード値を作成.
	//毎回違う値をくれる.
	static std::random_device rd;
	//メルセンヌ・ツイスタという乱数エンジン※高速.
	//genはインスタンス.
	//rd()は初期値に戻す.
	static std::mt19937 gen(rd());

	//出現しやすさを重みで設定.
	//(数が大きいほど出やすい).
	//各数字は重みであり、くじ引きで想像するとわかりやすいかも.
	//例：シールド30枚、リフレクション5枚のチケットがある、合計は35枚だが、
	//そのうち30枚はシールドで(30/35)、残りはリフレクション(5/35)ということ.
	std::discrete_distribution<> dist({
		0,			// Shield      → よく出る.
		0,			// SpeedUp     → よく出る.
		0,			// PowerUp     → そこそこ.
		100,		// BlastUp     → ちょっと出にくい.
		0,			// Reflection  → 出にくい.
		0			// Reload      → 普通.
		});

	return static_cast<CItemType>(dist(gen));
}

//位置をランダム化.
D3DXVECTOR3 CItemBoxManager::ItemPositionRandom()
{
	//staticを付けるのは、毎回作っていると、処理速度が低下するから、一度だけ作成.

	//シード値を作成.
	//毎回違う値をくれる.
	static std::random_device rd;
	//メルセンヌ・ツイスタという乱数エンジン※高速.
	//genはインスタンス.
	//rd()は初期値に戻す.
	static std::mt19937 gen(rd());

	//範囲設定.
	std::uniform_real_distribution < float > distX(-25.0f, 25.0f);
	std::uniform_real_distribution < float > distZ(-25.0f, 25.0f);

	//Y軸固定.
	const float fixedY = 20.0f;

	return D3DXVECTOR3(distX(gen), fixedY, distZ(gen));
}

//アイテムの情報を取得する.
ItemInfomation CItemBoxManager::GetItemInfo(int index)
{
	if (index >= 0 && index < m_Item.size())
	{
		return m_Item[index]->GetItem();
	}
}

std::vector<std::shared_ptr<CItemBox>> CItemBoxManager::GetItem() const
{
	return m_Item;
}

std::shared_ptr<CCollider> CItemBoxManager::GetCollider() const
{
	for (auto& item : m_Item)
	{
		return item->GetCollider();
	}
}
