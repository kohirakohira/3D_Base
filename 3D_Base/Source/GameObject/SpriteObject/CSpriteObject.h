#pragma once
#include "GameObject//CGameObject.h" // ゲームオブジェクトクラス

#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3Dスプライトクラス.

/************************************************************
*	スプライトオブジェクトクラス.
**/
class CSpriteObject
	: public CGameObject // ゲームオブジェクトクラスを継承
{
public:
	CSpriteObject();
	virtual ~CSpriteObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	//CSpriteObjectで宣言した関数で、以降はこれをoverrideさせる
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//スプライトを接続する.
	void AttachSprite( CSprite3D& pSprite ){
		m_pSprite = &pSprite;
	}
	//スプライトを切り離す.
	void DetachSprite(){
		m_pSprite = nullptr;
	}

protected:
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override final;

protected:
	CSprite3D*	m_pSprite;
};
