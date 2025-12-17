#pragma once
//-----ƒ‰ƒCƒuƒ‰ƒŠ-----
#include <iostream>

//-----Œp³ƒNƒ‰ƒX-----
#include "../../GameObject/UI/CUIObject/CUIObject.h"


//=====================================================================
//				”wŒi‚Ì•`‰æ(CUIObject‚ğŒp³).
//=====================================================================
class BackGround
	: public CUIObject
{
public:
	BackGround();
	~BackGround() override;

	//XVŠÖ”.
	void Update() override;

public:

};