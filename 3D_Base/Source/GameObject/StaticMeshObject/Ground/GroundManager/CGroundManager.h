#pragma once
//-----STL-----.
#include <iostream>
#include <vector>

//-----クラス-----.
#include "../CGround.h"			//地面クラス.

//============================================================================
//		地面クラスマネージャー※演出に使用.
//============================================================================
class CGroundManager
{
public:
	CGroundManager();
	~CGroundManager();

	//更新関数.
	void Update();
	//描画関数.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera);
	//インスタンス生成.
	void Create();
	//初期化.
	void Init();

	//メッシュのアタッチ.
	void AttachMesh(std::shared_ptr<CStaticMesh> mesh);

	//位置設定.
	void SetPosition(float x, float y, float z);
	//回転設定.
	void SetRotato(float x, float y, float z);

public:
	//地面クラス※複数.
	std::vector<std::shared_ptr<CGround>>		m_Ground;

};