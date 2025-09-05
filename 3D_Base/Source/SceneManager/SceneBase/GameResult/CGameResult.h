#pragma once
//-----�p������N���X-----
#include "SceneManager//SceneBase//CSceneBase.h" // �V�[�����N���X

//-----���C�u����-----
#include <iostream>
#include <windows.h>

#include "Global.h"

//-----DirectX.-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9�N���X
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11�N���X

//���o�p.
#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3D�X�v���C�g�N���X.

//-----���b�V��-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X

#include "Camera//CCamera.h" //�J�����N���X

#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h" // �v���C���[�}�l�[�W���[�N���X
#include "GameObject//StaticMeshObject//Ground//CGround.h" // �n�ʃN���X

//�L�[�o�C���h.
#include "InputDevice//Matuoka//CMultiInputKeyManager.h" // �L�[���͗p.

//--------------------------------
// UI
//-------------------------------
#include "GameObject//UI//CUIObject//CUIObject.h"		// UI�I�u�W�F�N�g�N���X

//�I���摜.
#include "GameObject//UI//SelectIcon//CSelectIcon.h"	// �I���A�C�R���N���X
//�I����.
#include "GameObject//UI//ChoiceImage//CChoiceImage.h"  // �I�����N���X




//�O���錾.
class CSprite2D;

class CGameResult
	:public CSceneBase
{
public:
	CGameResult(HWND hWnd);
	~CGameResult()override;


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

	//���������������̔���N���X.
	CSceneType WinOrDrawFunction();

	//�V�[���̎��.
	CSceneType GetSceneType() const override { return m_SceneType; }

public:
	//�E�B���h�E�n���h��.
	HWND		m_hWnd;

	//�^�C�g���摜.
	std::shared_ptr<CSprite2D>			m_pSpriteResult;
	std::shared_ptr<CSprite2D>			m_pSpriteSelect;
	//�I�����摜.
	std::shared_ptr<CSprite2D>			m_SpriteChoice;

	//�X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X.
	std::shared_ptr<CUIObject>			m_pSpriteObj;
	//�X�^�e�B�b�N���b�V��(�g���܂킷����)
	std::shared_ptr<CStaticMesh>		m_pStaticMeshFighter;		//���@
	std::shared_ptr<CStaticMesh>		m_pStaticMeshGround;		//�n��
	std::shared_ptr<CStaticMesh>		m_pStaticMeshCloud;			//�_

	//�V�[���^�C�v�ϐ�.
	CSceneType m_SceneType;


	//���o.
	std::shared_ptr<CPlayerManager>		m_pPlayerManager;
	std::shared_ptr<CGround>			m_pGround;
	//�J����.
	std::shared_ptr<CCamera>			m_pCamera;

	//�I���A�C�R��.
	std::shared_ptr<CSelectIcon>		m_pSelectIcon;
	//�I����.
	std::shared_ptr<CChoiceImage>		m_pChoiceIcon;

	//�`��t���O.
	bool DrawFlag;

private:
	//�m�F�p.
	int NoMajic = 0;
	//�L�[�o�C���h.
	std::shared_ptr<CMultiInputKeyManager>	m_Key;
};