#pragma once
//-----ライブラリ-----
#include <iostream>

//-----外部クラス-----
#include "GameObject//UI//CUIObject//CUIObject.h"		// UIオブジェクトクラス
#include "SceneManager//SceneType//CSceneType.h"		// シーンタイプ
#include "InputDevice/Input/Controller/ControllerManager/CControllerManager.h"	//コントローラー.
#include "InputDevice/Key/CMultiInputManager/CMultiInputKeyManager.h"			//キー.

//----------------選択肢用クラス.----------------
//CUIObjectを継承している.
class CChoiceImage
	: public CUIObject
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

	//コントローラー入出力.
	void ControllerUpdate();
	//キー入出力.
	void KeyUpdate();

public:
	//シーンタイプ変数.
	CSceneType		m_SceneType;

	//選択しているものを識別するためのモノ.
	bool 			m_IsSelected;

	//定数宣言.
	//位置の調整用.
	const float posAdjustment_1 = 1.5f;
	const float posAdjustment_2 = 1.2f;
	const float posAdjustment_3 = 1.37f;
	const float posAdjustment_4 = 2.75f;
private:
	//キークラスの宣言.
	std::unique_ptr<CMultiInputKeyManager>	m_Key;

};