#pragma once
//-----ライブラリ-----
#include <iostream>
#include <vector>
#include <array>

//-----必要クラス-----
#include "Assets/Sprite/Sprite2D/CSprite2D.h"


//================================================================
//			設定画面でのUIマネージャー.
//================================================================
class SettingUIObjectManager
{
public:
	SettingUIObjectManager();
	~SettingUIObjectManager();

	//更新処理.
	void Update();
	//描画処理.
	void Draw();

	//インスタンス生成.
	void Create();

	//スプライトの保存関数.
	void SetSettingUIImg(SettingKinds kind, std::shared_ptr<CSprite2D> sprite);

private:
	//スプライトの情報を持つ変数(11コ).
	std::array<std::shared_ptr<CSprite2D>, IMAGE> m_Sprites;



};