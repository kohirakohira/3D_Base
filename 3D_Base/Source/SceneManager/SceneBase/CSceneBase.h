#pragma once
//-----���C�u����-----
#include <iostream>

//-----�O���N���X-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9�N���X
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11�N���X

#include "SceneManager//SceneType//CSceneType.h" // �V�[���^�C�v
class CSceneBase
{
public:
	CSceneBase() {};
	virtual~CSceneBase() {};

	//����֐�.
	virtual void Update()	= 0;
	//�`��֐�.
	virtual void Draw()		= 0;
	//�������֐�.
	virtual void Init()		= 0;
	//����֐�.
	virtual void Destroy()	= 0;
	//�C���X�^���X�쐬�֐�.
	virtual void Create()	= 0;
	//�f�[�^�̓ǂݍ���.
	virtual HRESULT LoadData() = 0;

	//�Ȃ�̃V�[���Ȃ̂��𔻒肷��֐�.
	virtual CSceneType GetSceneType() const = 0;
}; 

