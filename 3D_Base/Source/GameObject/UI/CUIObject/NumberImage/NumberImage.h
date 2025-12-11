#pragma once
//-----ライブラリ-----
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <string>

//-----継承-----
#include "GameObject//UI/CUIObject/CUIObject.h"

//-----必要クラス-----
#include "Assets//Sprite/Sprite2D/CSprite2D.h"

//=========================================================
//				数字画像を管理するクラス.
//=========================================================
class NumberImage
	: public CUIObject
{
public:
	//.


	//数字構造体.
	struct NUM
	{
		int Thousand;	//千の位.
		int Hundred;	//百の位.
		int Ten;		//十の位.
		int One;		//一の位.
	};
public:
	NumberImage();
	~NumberImage() override;

	//更新処理.
	void Update() override;
	//描画処理.
	void Draw() override;
	//初期化.
	void Init();

	//数字の計算.
	void NumberCalculations();

	//画像の設定.
	std::shared_ptr<CSprite2D> SetSpriteNumber(std::shared_ptr<CSprite2D> img) { m_NumImage = img; }

	//制限時間の設定.
	float SetTimer(float t) { m_Time = t; }

	//数字の設定.
	void SetNumber(int num)
	{
		if (num < 0 || num > 9)
		{
			num = 0;
		}
		else
		{
			m_PatternNo.x = num;
			m_PatternNo.y = 0;
		}
	}


public:
	//制限時間.
	int m_Time;
	//UV設定用.
	std::unordered_map<std::string, std::shared_ptr<NumberImage>>	m_NumBerMap;
	//画像.
	std::shared_ptr<CSprite2D>		m_NumImage;

private:
	//数字構造体の変数.
	NUM m_Num;
};