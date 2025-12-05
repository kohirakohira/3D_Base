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
	void Create(const D3DXVECTOR3& pos, float maxRad, std::shared_ptr<CStaticMesh> mesh, float s);

	//更新処理.
	void Update();

	//描画処理.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera);
	
private:
	std::vector <std::shared_ptr<CBlast>>		m_Blast;
};