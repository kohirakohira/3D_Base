#pragma once
//-----ライブラリ-----
#include <iostream>

//-----必要クラス-----
#include "../CSingleton.h"
#include "../../GameObject/UI/CUIObject/CUIObject.h"

//=================================================================
//		フェイクロードクラス※シングルトン化.
//=================================================================
class FakeLoad
	: public CSingleton<FakeLoad>
{
public:
	//更新処理.
	void Update();
	//描画処理.
	void Draw();
	//初期化.
	void Init();
	//素材の読み込み.
	HRESULT LoadData();
	//描画の設定.
	void SetIsDraw(bool f) { m_IsDraw = f; }

private:
	//friend：クラスや関数に「private/protectedメンバへのアクセス権」を与えることができる.
	friend class CSingleton<FakeLoad>;
	FakeLoad();
	~FakeLoad();

private:
	//表示・非表示フラグ.
	bool m_IsDraw;
	//α値.
	float m_Alpha;
	//スプライト2D.
	std::shared_ptr<CSprite2D> m_Sprite2DFakeLoad;
	//スプライトオブジェクトクラス.
	std::unique_ptr<CUIObject> m_SpriteObj;
};