#pragma once
#include <iostream>

#include "GameObject//UI//CUIObject//CUIObject.h"		// UIオブジェクトクラス

class CSelectIcon
	:public CUIObject
{
public:
	CSelectIcon();
	virtual ~CSelectIcon() override;

	//動作関数.
	virtual void Update() override;

	//描画関数.
	virtual void Draw() override;




public:


};