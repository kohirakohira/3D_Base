#pragma once
//-----�p������N���X-----
#include "SceneManager//SceneBase//CSceneBase.h" // �V�[�����N���X

//-----���C�u����-----
#include <iostream>
#include <windows.h>

//-----�O���[�o��-----
#include "Global.h"

//-----�O���N���X-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9�N���X
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11�N���X

#include "GameObject//UI//CUIObject//CUIObject.h"		// UI�I�u�W�F�N�g�N���X
#include "GameObject//UI//ChoiceImage//CChoiceImage.h"  // �I�����N���X

//�L�[�̓���.
#include "InputDevice//Matuoka//CMultiInputKeyManager.h" // �L�[���͗p

//���o�p.
#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3D�X�v���C�g�N���X.

#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X
#include "Camera//CCamera.h" //�J�����N���X

//�n�ʃN���X.
#include "GameObject//StaticMeshObject//Ground//CGround.h" // �n�ʃN���X


//�O���錾.
class CSprite2D;

class CGameTitle
	:public CSceneBase
{
public:
	CGameTitle(HWND hWnd);
	~CGameTitle()override;


	//����֐�.
	void Update()override;
	//�`��֐�.
	void Draw()override;
	//�������֐�.
	void Init()override;
	//����֐�.
	void Destroy()override;
	//�C���X�^���X�쐬�֐�.
	void Create()override;
	//�f�[�^�̓ǂݍ���.
	HRESULT LoadData()override;


	//�V�[���̎��.
	CSceneType GetSceneType() const override { return m_SceneType; }
public:
	//�E�B���h�E�n���h��.
	HWND		m_hWnd;

	//�^�C�g���摜.
	std::shared_ptr<CSprite2D>			m_pSpriteTitle;
	//�I�����摜.
	std::shared_ptr<CSprite2D>			m_SpriteChoice;
	//�I���摜(�v���C&�G���h).
	std::shared_ptr<CSprite2D>			m_SpritePlaySelect;
	std::shared_ptr<CSprite2D>			m_SpriteEndSelect;

	//�^�C�g���摜.
	std::shared_ptr<CUIObject>			m_pSpriteTitleImg;
	//�I�����摜.
	std::shared_ptr<CChoiceImage>		m_pSpriteChoiceImg;	
	//�I���摜(�v���C&�G���h).
	std::shared_ptr<CUIObject>			m_pSpritePlaySelectImg;
	std::shared_ptr<CUIObject>			m_pSpriteEndSelectImg;


	//�V�[���^�C�v�ϐ�.
	CSceneType m_SceneType;

	//�L�[����.
	std::shared_ptr<CMultiInputKeyManager>	m_KeyInput;

	//�`��t���O.
	bool DrawFlag;



//------------------------------------------------------------------------------------------
//	���o.
//------------------------------------------------------------------------------------------
public:
	//�J�����N���X.
	std::shared_ptr<CCamera>			m_pCamera;

	//�n�ʃN���X.
	std::shared_ptr<CGround>			m_pGround;
	//�n�ʂ̃��b�V��.
	std::shared_ptr<CStaticMesh>		m_StaticMeshGround;

	
	
	float y = 0.f;

	float rad = 0.f;



};