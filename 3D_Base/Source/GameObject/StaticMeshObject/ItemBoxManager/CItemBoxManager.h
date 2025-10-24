#pragma once
#include <iostream>
#include <vector>
#include <random>

#include "GameObject//StaticMeshObject//ItemBoxManager//ItemBox//CItemBox.h"		// アイテムボックス
#include "GameObject//StaticMeshObject//ItemBoxManager//ItemBoxType/ItemType.h"		//アイテムの種類
#include "Global.h"

//アイテムボックスマネージャー.
class CItemBoxManager
{
public:
	CItemBoxManager();
	~CItemBoxManager();

	//動作関数.
	void Update();
	//描画関数.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera);
	//インスタンス生成(複製).
	void Create();
	//メッシュの設定.
	void AttachMesh(std::shared_ptr<CStaticMesh> pMesh);

	// バウンディングオブジェクトの作成
	void CreateBounding(std::shared_ptr<CStaticMesh>& pItem);

	// コライダーの作成
	void CreateCollider();

	//位置設定.
	void SetPosition(float x, float y, float z);
	//回転設定.
	void SetRotation(float x, float y, float z);
	//大きさ設定.
	void SetScale(float x, float y, float z);

	//重力の有無を設定.
	void SetGravity(bool flg);
	//アイテムの中身を設定してあげる.
	void SetItemInfo();

	//アイテムの中身をランダム化.
	CItemType ItemRandom();

	//外部のクラスから情報取得.
	void SetCItemBox(std::vector<std::shared_ptr<CItemBox>> pItem) { m_Item = pItem; }

	// 外部のクラスに情報を渡す
	std::vector<std::shared_ptr<CItemBox>>	GetItem() const { return m_Item; }

public:
	//アイテムボックス.
	std::vector<std::shared_ptr<CItemBox>>	m_Item;
	//アイテムメッシュ.
	std::shared_ptr<CStaticMesh>			m_ItemMesh;
	
	//アイテムの種類.
	CItemType m_ItemInfo;
};