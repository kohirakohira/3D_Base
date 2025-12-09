#pragma once
//-----ライブラリ-----
#include <iostream>
#include <unordered_map>

//-----継承クラス-----
#include "GameObject//UI/CUIObject/CUIObject.h"

//=================================================================
//			タイマー用のUI.
//=================================================================
class TimerImage
	: public CUIObject
{
public:
	TimerImage();
	~TimerImage() override final;

	//更新処理.
	void Update() override final;
	//描画処理.
	void Draw() override final;

	//時間を設定.
	void SetTimeLimit(float t) { m_Timer = t; }

	//画像の位置(時間の計算).
	void ImgLoadPosition();

public:
	//.
	std::unordered_map<> m_TimeLimitImage;

	//読み込み位置.
	float m_LoadPosX;

	//時間.
	float m_Timer;

};