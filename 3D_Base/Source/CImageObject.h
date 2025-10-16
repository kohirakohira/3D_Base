#pragma once
#include <iostream>

#include "CGameObject.h"
#include "CSprite2D.h"

//-----画像クラス.-----
//ゲームオブジェクトを継承.
class CImageObject
	:public CGameObject
{
public:
	CImageObject();
	~CImageObject();

	//動作関数.
	void Update() override;
	//描画関数.
	void Draw();

	//スプライトを接続する.
	void AttachSprite(std::shared_ptr<CSprite2D> pSprite) {
		m_pSprite = pSprite;
	}
	//スプライトを切り離す.
	void DetachSprite() {
		m_pSprite = nullptr;
	}

	//画像の位置を設定する.
	void SetPosition(float x, float y, float z)
	{
		//位置を設定.
		m_vPosition = D3DXVECTOR3(x, y, z);
	}

	//画像の動作.
	void ImageMove();


protected:
	//描画関数※finalでこれ以上継承するのを防ぐ.
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override final;




public:
	//スプライト変数の宣言.
	std::shared_ptr<CSprite2D>	 m_pSprite;

};