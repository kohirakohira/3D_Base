#include "NumberImage.h"

NumberImage::NumberImage()
	: m_Num				(NUM{0, 0, 0, 0})
	, m_Number			( 0 )
{
}

NumberImage::~NumberImage()
{
}

//更新処理.
void NumberImage::Update()
{

}

//描画処理.
void NumberImage::Draw()
{
	CUIObject::Draw();
}

//初期化.
void NumberImage::Init()
{
	//2桁に分解.
	int left	= m_Number / 10;
	int right	= m_Number % 10;

	//オブジェクトに設定.
	m_NumBerMap["left"]->SetNumber(left);
	m_NumBerMap["right"]->SetNumber(right);
}

//数字の計算.
void NumberImage::NumberCalculations()
{

}
