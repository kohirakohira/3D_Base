#pragma once
//-----ƒ‰ƒCƒuƒ‰ƒŠ-----
#include <iostream>

//-----Œp³ƒNƒ‰ƒX-----
#include "../../GameObject/SpriteObject/CSpriteObject.h"


//=====================================================================
//				”wŒi‚Ì•`‰æ(CUIObject‚ğŒp³).
//=====================================================================
class BackGround
	: public CSpriteObject
{
public:
	BackGround();
	~BackGround() override;

	//XVŠÖ”.
	void Update() override;

public:

};