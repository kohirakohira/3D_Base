#pragma once
#include <iostream>

#include "GameObject//UI//CUIObject//CUIObject.h"		// UI�I�u�W�F�N�g�N���X

class CSelectIcon
	:public CUIObject
{
public:
	CSelectIcon();
	virtual ~CSelectIcon() override;

	//����֐�.
	virtual void Update() override;

	//�`��֐�.
	virtual void Draw() override;




public:


};