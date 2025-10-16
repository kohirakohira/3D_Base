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
	//中身を消す.
	m_Item.clear();

	//アイテムのインスタンス生成.
	for (int i = 0; i < ITEM_MAX; i++)
	{
		//アイテムボックスのインスタンス生成.
		m_Item.push_back(std::make_shared<CItemBox>());
	}
}

void CItemBoxManager::AttachMesh(std::shared_ptr<CStaticMesh> pMesh)
{
	//メッシュ設定.
	for (auto& item : m_Item)
	{
		item->AttachMesh(pMesh);
	}
}

void CItemBoxManager::SetPosition(float x, float y, float z)
{
	for (auto& item : m_Item)
	{
		//横にずらすだけ.
		x += 2;
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

void CItemBoxManager::SetScale(float x, float y, float z)
{
	for (auto& item : m_Item)
	{
		//大きさ設定.
		item->SetScale(x, y, z);
	}
}

void CItemBoxManager::SetItemInfo()
{
	for (auto& item : m_Item)
	{
		//中身を順番に設定.
		//ランダムで設定.
		m_ItemInfo = ItemRandom();

		item->SetItemInfo(m_ItemInfo);
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
		20,			// Shield      → よく出る.
		25,			// SpeedUp     → よく出る.
		25,			// PowerUp     → そこそこ.
		10,			// BlastUp     → ちょっと出にくい.
		8,			// Reflection  → 出にくい.
		12			// Reload      → 普通.
		});

	return static_cast<CItemType>(dist(gen));
}
