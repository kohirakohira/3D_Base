#pragma once
//-----���C�u����-----
#include <iostream>
#include <windows.h>

//-----�O���N���X-----
#include "SceneManager//SceneBase//CSceneBase.h"				// �V�[�����N���X
#include "SceneManager//SceneBase//GameMain//CGameMain.h"		// �Q�[�����C���N���X
#include "SceneManager//SceneBase//GameTitle//CGameTitle.h"		// �^�C�g���N���X


class CSceneManager
{
public:
	CSceneManager(HWND hWnd );
	~CSceneManager();

	//����֐�.
	void Update();
	//�`��֐�.
	void Draw();

	//�ǂݍ���.
	HRESULT LoadData();

	//�C���X�^���X����.
	void Create();

	//�V�[���J�ڊ֐�.
	void Change(std::shared_ptr<CSceneBase> InScene);
	
	
public:
	//�m�F�p�̕ϐ�.
	bool			m_Flag;

	//�E�B���h�E�n���h��.
	HWND			m_hWnd;

private:
	//�V�[����ۑ�����悤�̕ϐ�.
	std::shared_ptr<CSceneBase> m_pScene;

	//�V�[����ۑ����Ă������m.
	CSceneType					m_CurrentSceneType;
	//���̃V�[��.
	CSceneType					m_NextType;

};