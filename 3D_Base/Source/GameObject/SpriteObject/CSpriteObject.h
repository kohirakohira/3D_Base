#pragma once
#include "GameObject//CGameObject.h" // �Q�[���I�u�W�F�N�g�N���X

#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3D�X�v���C�g�N���X.

/************************************************************
*	�X�v���C�g�I�u�W�F�N�g�N���X.
**/
class CSpriteObject
	: public CGameObject // �Q�[���I�u�W�F�N�g�N���X���p��
{
public:
	CSpriteObject();
	virtual ~CSpriteObject() override;

	//CGameObject�ŏ������z�֐��̐錾������Ă�̂ł�����Œ�`������.
	virtual void Update() override;
	//CSpriteObject�Ő錾�����֐��ŁA�ȍ~�͂����override������
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj );

	//�X�v���C�g��ڑ�����.
	void AttachSprite( CSprite3D& pSprite ){
		m_pSprite = &pSprite;
	}
	//�X�v���C�g��؂藣��.
	void DetachSprite(){
		m_pSprite = nullptr;
	}

protected:
	void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override final;

protected:
	CSprite3D*	m_pSprite;
};
