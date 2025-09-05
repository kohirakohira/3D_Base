#pragma once
#include <iostream>
#include <vector>
#include <random>

#include "CItemBox.h"		//アイテムボックス.
#include "ItemType.h"		//アイテムの種類.
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


	//位置設定.
	void SetPosition(float x, float y, float z);
	//回転設定.
	void SetRotation(float x, float y, float z);
	//大きさ設定.
	void SetScale(float x, float y, float z);


	//アイテムの中身を設定してあげる.
	void SetItemInfo();

	//アイテムの中身をランダム化.
	CItemType ItemRandom();

public:
	//アイテムボックス.
	std::vector<std::shared_ptr<CItemBox>>	m_Item;
	//アイテムメッシュ.
	std::shared_ptr<CStaticMesh>			m_ItemMesh;

	//アイテムの種類.
	CItemType m_ItemInfo;

private:

};