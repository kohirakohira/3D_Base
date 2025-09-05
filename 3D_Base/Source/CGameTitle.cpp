#include "CGameTitle.h"
#include "CSoundManager.h"

//�O���錾.
#include "CSprite2D.h"

CGameTitle::CGameTitle(HWND hWnd)
	: m_hWnd(hWnd)

	, m_pSpriteTitle		( nullptr )
	, m_SpriteChoice		( nullptr )
	, m_SpritePlaySelect	( nullptr )
	, m_SpriteEndSelect		( nullptr )

	, m_pSpriteTitleImg		( nullptr )
	, m_pSpriteChoiceImg	( nullptr )
	, m_pSpritePlaySelectImg( nullptr )
	, m_pSpriteEndSelectImg	( nullptr )

	, m_KeyInput			( nullptr )

	, m_pCamera				( nullptr )

	, m_pPlayer				( nullptr )
	, m_pPlayerBody			( nullptr )
	, m_pPlayerCannon		( nullptr )

	, m_pCannon				( nullptr )
	, m_pBody				( nullptr )

	, m_Ground				( nullptr )
	, m_GroundMesh			( nullptr )

	, DrawFlag				( false )

{
	m_SceneType = CSceneType::Title;
}

CGameTitle::~CGameTitle()
{
	m_hWnd = nullptr;
}

void CGameTitle::Update()
{
	//BGM�̃��[�v�Đ�.
	CSoundManager::PlayLoop(CSoundManager::BGM_Title);







	//�J�����̈ʒu�����߂�p�ŉ��ǉ�.
	m_pCamera->FreeMove();








	//�L�[���͎�t.
	m_KeyInput->Update();

	//�I�����̈ړ�����.
	m_pSpriteChoiceImg->Update();
//��-----�^�C�g���ł̉��o-----��.

	m_Ground->Update();

//��-----�^�C�g���ł̉��o-----��.



	//�f�o�b�O�ɑJ��.
	if (m_KeyInput->ReleaseInputKey('D') == true)
	{
		//BGM�̃��[�v��~.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::Debug;

	}
	//���C���ɑJ��.
	if (m_KeyInput->ReleaseInputKey('M') == true)
	{
		//BGM�̃��[�v��~.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::Main;

	}
	//���U���gDraw�ɑJ��.
	if (m_KeyInput->ReleaseInputKey('L') == true)
	{
		//BGM�̃��[�v��~.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::ResultDraw;

	}
	//���U���gwin�ɑJ��.
	if (m_KeyInput->ReleaseInputKey('Y') == true)
	{
		//BGM�̃��[�v��~.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::ResultWin;

	}

	//�Q�[���ݒ�ɑJ��.
	if (m_KeyInput->ReleaseInputKey('Z') == true)
	{
		if (m_pSpriteChoiceImg->GetSelectedFlag() == false)
		{
			//BGM�̃��[�v��~.
			CSoundManager::Stop(CSoundManager::BGM_Title);

			//SE�̍Đ�.
			CSoundManager::PlaySE(CSoundManager::SE_Click);

			DrawFlag = true;
			
			//�Q�[���ݒ�ɑJ��.
			m_SceneType = CSceneType::Setting;
		}
		else
		{
			//BGM�̃��[�v��~.
			CSoundManager::Stop(CSoundManager::BGM_Title);

			//SE�̍Đ�.
			CSoundManager::PlaySE(CSoundManager::SE_Click);

			DrawFlag = true;

			//�Q�[�����I��������.
			PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		}
	}
}

void CGameTitle::Draw()
{
	if (DrawFlag == true) {
		return;
	}

	//���o�p.
	//�J�����̕`��.
	m_pCamera->Draw();

	//�v���C���[�̕`��.
	m_pPlayer->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);

	//�n�ʂ̕`��.
	m_Ground->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);



	//�O��֌W����.
	CDirectX11::GetInstance().SetDepth(false);
	//�^�C�g���̕`��.
	m_pSpriteTitleImg->Draw();
	//�I�����̕`��.
	m_pSpritePlaySelectImg->Draw();
	m_pSpriteEndSelectImg->Draw();
	CDirectX11::GetInstance().SetDepth(true);



	//�I�����̕`��.
	m_pSpriteChoiceImg->Draw();
}

void CGameTitle::Init()
{	
	//�萔�錾.
	const float POS_X		= WND_W / 8.f;
	const float POS_Y		= WND_H / 1.2f;
	const float SL_POS_X	= WND_W / 4.2f;
	const float SL_POS_Y	= WND_H / 1.5f;

	//�傫����ݒ�.
	m_pSpriteTitleImg->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTitleImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTitleImg->SetScale(0.5f, 0.5f, 0.f);
	//�I�����摜�̑傫����ݒ�.
	m_pSpriteChoiceImg->SetPosition(SL_POS_X, SL_POS_Y, 0.f);
	m_pSpriteChoiceImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteChoiceImg->SetScale(1.f, 1.f, 0.f);
	//�I�����摜�̑傫����ݒ�.
	m_pSpritePlaySelectImg->SetPosition(POS_X, SL_POS_Y, 0.f);
	m_pSpritePlaySelectImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpritePlaySelectImg->SetScale(1.f, 1.f, 0.f);
	//�I�����摜�̑傫����ݒ�.
	m_pSpriteEndSelectImg->SetPosition(POS_X, POS_Y, 0.f);
	m_pSpriteEndSelectImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteEndSelectImg->SetScale(1.f, 1.f, 0.f);
		
	//�L�[���͂̏�����.
	m_KeyInput->Init();
	m_KeyInput->SetKey({'D', 'Z', 'L', 'Y', 'M'});




	//���o�p.
	//�J�����̈ʒu�ݒ�.
	m_pCamera->SetCameraPos(-5.f, 2.f, 15.f);
	m_pCamera->SetLightRot(1.f, 5.f, 10.f);

	//�v���C���[�̐ݒ�.
	m_pPlayer->SetPosition(0.f, 0.f, 0.f);
	m_pPlayer->SetRotation(0.f, 0.f, 0.f);
	m_pPlayer->SetScale(1.f, 1.f, 1.f);

	//�n�ʂ̈ʒu�ݒ�.
	m_Ground->SetPosition(1.f, 0.f, 1.f);
	m_Ground->SetRotation(0.f, 0.f, 0.f);
	m_Ground->SetScale(100.f);
}

void CGameTitle::Destroy()
{

}

void CGameTitle::Create()
{
	//�X�v���C�g�̐���.
	m_pSpriteTitle		= std::make_shared<CSprite2D>();
	m_SpriteChoice		= std::make_shared<CSprite2D>();
	m_SpritePlaySelect	= std::make_shared<CSprite2D>();
	m_SpriteEndSelect	= std::make_shared<CSprite2D>();

	//�X�v���C�g�I�u�W�F�N�g�̐���.
	m_pSpriteTitleImg		= std::make_shared<CUIObject>();
	//�I�����̃X�v���C�g�I�u�W�F�N�g�̐���.
	m_pSpriteChoiceImg		= std::make_shared<CChoiceImage>(m_SceneType);
	//�I�����̃X�v���C�g�I�u�W�F�N�g�̐���.
	m_pSpritePlaySelectImg	= std::make_shared<CUIObject>();
	m_pSpriteEndSelectImg	= std::make_shared<CUIObject>();

	//�J�����N���X�̃C���X�^���X�쐬.
	m_pCamera = std::make_shared<CCamera>();

	//�L�[����.
	m_KeyInput = std::make_shared<CMultiInputKeyManager>();



	//���o�p.
	//�v���C���[�̃C���X�^���X����.
	m_pPlayer		 = std::make_shared<CPlayer>();
	m_pPlayerBody	 = std::make_shared<CBody>(0);
	m_pPlayerCannon	 = std::make_shared<CCannon>(0);

	//�v���C���[�̃��b�V��.
	m_pCannon	= std::make_shared<CStaticMesh>();
	m_pBody		= std::make_shared<CStaticMesh>();

	//�n��.
	m_Ground		= std::make_shared<CGround>();
	m_GroundMesh	= std::make_shared<CStaticMesh>();




}

HRESULT CGameTitle::LoadData()
{
	//�^�C�g���摜�̃X�v���C�g�ݒ�.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		WND_W,WND_H,		//�`�敝,����.
		WND_W,WND_H,		//���摜�̕�,����.
		WND_W,WND_H			//�A�j���[�V���������Ȃ��̂ŁA0�ł���.
	};
	//�I�����摜�̃X�v���C�g�ݒ�.
	CSprite2D::SPRITE_STATE C_SIZE = {
		16,32,		//�`�敝,����.
		16,32,		//���摜�̕�,����.
		16,32,		//�A�j���[�V���������Ȃ��̂ŁA0�ł���.
	};
	//�I���摜�̃X�v���C�g�ݒ�.
	CSprite2D::SPRITE_STATE SL_SIZE = {
		256,96,		//�`�敝,����.
		256,96,		//���摜�̕�,����.
		256,96,		//�A�j���[�V���������Ȃ��̂ŁA0�ł���.
	};

	//�^�C�g���X�v���C�g�̓ǂݍ���.
	m_pSpriteTitle->Init( _T("Data\\Texture\\Image\\Title.png"), WH_SIZE);
	//�I�����X�v���C�g�̓ǂݍ���.
	m_SpriteChoice->Init(_T("Data\\Texture\\UI\\Choice.png"), C_SIZE);
	//�I�����X�v���C�g�̓ǂݍ���.
	m_SpritePlaySelect->Init(_T("Data\\Texture\\UI\\PlaySelectImg.png"), SL_SIZE);
	m_SpriteEndSelect->Init(_T("Data\\Texture\\UI\\EndSelectImg.png"), SL_SIZE);

	//�摜�̐ݒ�.
	m_pSpriteTitleImg->AttachSprite(m_pSpriteTitle);
	//�摜�̐ݒ�.
	m_pSpriteChoiceImg->AttachSprite(m_SpriteChoice);
	//�摜�̐ݒ�.
	m_pSpritePlaySelectImg->AttachSprite(m_SpritePlaySelect);
	m_pSpriteEndSelectImg->AttachSprite(m_SpriteEndSelect);



	//���o�p.
	//�v���C���[�̃��b�V���̓ǂݍ���.
	m_pCannon->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Cannon\\Cannon.x"));
	m_pBody->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Body\\Body.x"));

	//�v���C���[�̉摜�ݒ�.
	m_pPlayer->SetCBody(m_pPlayerBody);
	m_pPlayer->SetCCannon(m_pPlayerCannon);
	m_pPlayer->AttachMeshse(m_pBody, m_pCannon);

	//�n�ʂɃA�^�b�`.
	m_GroundMesh->Init(_T("Data\\Mesh\\Static\\Ground\\ground.x"));
	m_Ground->AttachMesh( m_GroundMesh );


	return S_OK;
}