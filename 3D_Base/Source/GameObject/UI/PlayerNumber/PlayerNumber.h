#pragma once
//-----�p������N���X-----
#include "GameObject\\UI\\CUIObject\\CUIObject.h"	//UI�I�u�W�F�N�g�N���X.

//-----���C�u����-----
#include <iostream>

//-----�O���N���X-----
#include "Assets\\DirectX\\DirectX11\\CDirectX11.h"		//DirectX11�N���X.

//------------------------------------------------------------
//	�v���C���[�ԍ��摜�N���X(CUIObject���p��).
//------------------------------------------------------------
class CPlayerNumber
	:public CUIObject
{
public:
	CPlayerNumber();
	~CPlayerNumber();

	//�X�V.
	void Update() override;
	//�`��.
	void Draw() override;



private:

};