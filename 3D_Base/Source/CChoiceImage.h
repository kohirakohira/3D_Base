#pragma once
#include <iostream>

#include "CUIObject.h"
#include "CSceneType.h"

//----------------選択肢用クラス.----------------
//CUIObjectを継承している.
class CChoiceImage
	:public CUIObject
{
public:
	CChoiceImage(CSceneType typ);
	~CChoiceImage() override;

	//動作関数.
	void Update() override;
	//描画関数.
	void Draw() override;

	//選択肢の移動関数.
	void MoveChoiceImg();

	//選択しているかどうかを取得する関数.
	bool GetSelectedFlag() const { return m_IsSelected; }


public:
	//シーンタイプ変数.
	CSceneType		m_SceneType;

	//選択しているものを識別するためのモノ.
	bool 			m_IsSelected;


private:

};