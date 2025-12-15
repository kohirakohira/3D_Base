#pragma once
//-----ライブラリ-----
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>

//-----継承-----
#include "GameObject//UI/CUIObject/CUIObject.h"


//=========================================================
//				数字画像を管理するクラス.
//=========================================================
class NumberImage
	: public CUIObject
{
public:
	NumberImage();
	~NumberImage() override;

	//更新処理.
	void Update() override;
	//描画処理.
	void Draw() override;
	//初期化.
	void Init();

	//数字の設定.
	void SetNumber(int num, int digitsNum);

	//画像の表示幅の設定.
	void SetDigitWidth(float w) { m_DigitWidth = w; }

	//画像の表示基準位置の設定.
	void SetBasePosition(D3DXVECTOR2 pos) { m_BasePos = pos; }

public:
	//桁ごとのUI.
	std::vector<std::shared_ptr<CUIObject>>	m_DigitObject;
	//UV設定用.
	std::unordered_map<int, POINT>	m_DigitPattern_Map;
	//桁ごとに分解した数字.
	std::vector<int>				m_Digits;
	//画像の横幅.
	float							m_DigitWidth;
	//基準位置.
	D3DXVECTOR2						m_BasePos;

};