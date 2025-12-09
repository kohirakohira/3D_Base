#include "TimerImage.h"

TimerImage::TimerImage()
	: m_LoadPosX				( 0.0f )
	, m_Timer					( 0.0f )
{
}

TimerImage::~TimerImage()
{
}

//更新処理.
void TimerImage::Update()
{

}

//描画処理.
void TimerImage::Draw()
{
	if (m_pSprite == nullptr) {
		return;
	}

	//描画直前で座標や回転情報などを更新.
	m_pSprite->SetPosition(m_vPosition);
	m_pSprite->SetRotation(m_vRotation);
	m_pSprite->SetScale(m_vScale);

	//レンダリング.
	m_pSprite->Render();
}

//画像の位置(時間の計算).
void TimerImage::ImgLoadPosition()
{
	SetPatternNo(m_LoadPosX, 0.0f);
}
