#pragma once
//-----STL-----.
#include <iostream>

//-----必要クラス-----.
#include "../CCharacter.h"

//======================================================================
//		タイトル用プレイヤー.
//======================================================================
class CTitleOnlyPlayer
	: public CCharacter
{
public:
	CTitleOnlyPlayer();
	~CTitleOnlyPlayer() override final;

	//更新関数.
	void Update() override;
	//描画関数.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)override;


private:

};