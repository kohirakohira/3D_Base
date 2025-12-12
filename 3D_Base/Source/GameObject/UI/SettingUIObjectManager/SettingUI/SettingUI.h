#pragma once
//-----ライブラリ-----
#include <iostream>

//-----継承クラス-----
#include "GameObject/CGameObject.h"

//-----必要クラス-----
#include "Assets/Sprite/Sprite2D/CSprite2D.h"


//==============================================================
//			設定画面でのUI※Ready等.
//==============================================================
class SettingUI
	:public CGameObject
{
public:
	SettingUI();
	~SettingUI();
	
	//更新処理.
	void Update() override final;
	//描画処理.
	void Draw();


protected:
	//描画処理.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)override final;

private:

};