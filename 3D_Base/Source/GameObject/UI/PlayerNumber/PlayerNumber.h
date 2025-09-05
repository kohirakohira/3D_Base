#pragma once
//-----継承するクラス-----
#include "GameObject\\UI\\CUIObject\\CUIObject.h"	//UIオブジェクトクラス.

//-----ライブラリ-----
#include <iostream>

//-----外部クラス-----
#include "Assets\\DirectX\\DirectX11\\CDirectX11.h"		//DirectX11クラス.

//------------------------------------------------------------
//	プレイヤー番号画像クラス(CUIObjectを継承).
//------------------------------------------------------------
class CPlayerNumber
	:public CUIObject
{
public:
	CPlayerNumber();
	~CPlayerNumber();

	//更新.
	void Update() override;
	//描画.
	void Draw() override;



private:

};