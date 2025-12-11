#pragma once
//-----ライブラリ-----
#include <iostream>
#include <vector>

//-----子クラス-----
#include "Blast/CBlast.h"

//==================================================================================
//			爆風マネージャークラス.
//==================================================================================
class CBlastManager
{
public:
	CBlastManager();
	~CBlastManager();

	//インスタンス生成.
	void Create(const D3DXVECTOR3& pos, std::shared_ptr<CStaticMesh> mesh, float s, const float playerID);

	//更新処理.
	void Update();

	//描画処理.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera);
	
	//爆風一つを取得.
	std::shared_ptr<CBlast> GetBlast(int index);

	//爆風をある分取得.
	std::vector <std::shared_ptr<CBlast>> GetAllBlast();

public:
	//通常の大きさ.
	float m_NormalRadius;
	//アイテム取得時の大きさ.
	float m_MaxRadius;

private:
	std::vector <std::shared_ptr<CBlast>>		m_Blast;
};