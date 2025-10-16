#define GetKey(KEY) (GetAsyncKeyState(KEY))
#undef min	//�}�N����`������
#undef max	

#include "CGameMain.h"
//-----�T�E���h-----
#include "Assets//Sound//CSoundManager.h" // �T�E���h�}�l�[�W���[�N���X
#include "Assets//Effect//CEffect.h"	//Effekseer��g�����߂̃N���X

//-----DirectX-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9�N���X
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11�N���X

//定数宣言.
static constexpr int TIME = 90.0;

CGameMain::CGameMain(HWND hWnd)
	: m_hWnd					( hWnd )

	//�摜.
	, m_pSprite2DTimerArrow			( nullptr )
	, m_pSprite2DTimerFrame			( nullptr )
	, m_pSprite2DTimer				( nullptr )
	, m_pSprite2DKillNomber			( nullptr )
	, m_pSprite2DHitPoint			( nullptr )
	, m_pSprite2DPlayerIcon			()

	//�摜�̐ݒ�.
	, m_pSpriteTimerFrame			()
	, m_pSpriteTimer				()
	, m_pSpriteTimerArrow			()
	, m_pSpritePlayerIcon			()
	, m_pSpriteKillNomber			()
	, m_pSpriteHitPoint				()

	, m_pSpriteGround				( nullptr )
	, m_pSpritePlayer				( nullptr )
	, m_pSpriteExplosion			( nullptr )

	, m_pStaticMeshGround			( nullptr )
	, m_pStaticMeshBSphere			( nullptr )
	, m_pStaticMeshItemBox			( nullptr )

	// ���
	, m_pStaticMesh_TankBodyRed		( nullptr )
	, m_pStaticMesh_TankCannonRed	( nullptr )
	, m_pStaticMesh_TankBodyYellow	( nullptr )
	, m_pStaticMesh_TankCannonYellow( nullptr )
	, m_pStaticMesh_TankBodyBlue	( nullptr )
	, m_pStaticMesh_TankCannonBlue	( nullptr )
	, m_pStaticMesh_TankBodyGreen	( nullptr )
	, m_pStaticMesh_TankCannonGreen	( nullptr )

	// �e
	, m_pStaticMesh_BulletRed		( nullptr )
	, m_pStaticMesh_BulletYellow	( nullptr )
	, m_pStaticMesh_BulletBlue		( nullptr )
	, m_pStaticMesh_BulletGreen		( nullptr )

	// ��
	, m_pStaticMeshWallW			( nullptr )
	, m_pStaticMeshWallH			( nullptr )

	, m_pStcMeshObj					( nullptr )

	, m_pPlayerManager				()
	, m_pShotManager				()

	, m_pGround						( nullptr )

	, m_pDbgText					( nullptr )

	, m_StopTimeCount				( 0 )
	, m_pCameras					()

	, m_Timer						( nullptr )

	, m_pWallTop					( nullptr )
	, m_pWallBottom					( nullptr )
	, m_pWallLeft					( nullptr )
	, m_pWallRight					( nullptr )
	, m_pItemBoxManager				( nullptr )

	, m_Rot							( 0.0f )

	, time							( 0.0f )

{
	//�ŏ��̃V�[������C���ɂ���.
	m_SceneType = CSceneType::Main;
}

CGameMain::~CGameMain()
{
	//�O���ō쐬���Ă���̂ŁA�����ł͔j�����Ȃ�
	m_hWnd = nullptr;
}

void CGameMain::Update()
{
	//BGM�̃��[�v�Đ�.
	CSoundManager::PlayLoop(CSoundManager::BGM_Main);

//-----���C�����o�p-----.
	
	//Icon���]������.
	m_Rot += 0.02f;
	
	
	
	
	
//-----���C�����o�p-----.

	//�v���C���[�S���X�V
	m_pPlayerManager->Update();


	// �e�̔���
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(i))
		{
			if (player->GetCannon()->IsShot())
			{
				m_pShotManager->SetReload(i,
					player->GetCannon()->GetPosition(),
					player->GetCannon()->GetRotation().y);
			}
		}
		m_pShotManager->Update();
	}

	//�J�����Ǐ]���X�V.�C���
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(i))
		{
			const D3DXVECTOR3 camPos = player->GetCannonPosition();	//�C���̈ʒu
			float yaw = player->GetCannonYaw();	//�C���̌���Y

			m_pCameras[i]->SetTargetPos(camPos);
			m_pCameras[i]->SetTargetRotY(yaw);
		}
		m_pCameras[i]->Update();
	}

	//�A�C�e���̓���.
	m_pItemBoxManager->Update();
	
	//�萔�錾.
	const float PI = 3.14159265358979f;
	//���v�̐j�̉�].
	float remaining = m_Timer->GetRemainingTime();
	float totle = 90.f;
	time = (remaining / totle) * 360;
	float angle = time * (PI / 180);
	m_pSpriteTimerArrow->SetRotation(0.f, 0.f, angle);

#if 0
	//Effect����
	{
		//�G�t�F�N�g�̃C���X�^���X���ƂɕK�v�ȃn���h��
		//���R�`�悵�Đ��䂷��Ȃ�R�K�v�ɂȂ�
		static ::EsHandle hEffect = -1;
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			if (GetAsyncKeyState('Y') & 0x0001) {
				hEffect = CEffect::GetInstance().Play(CEffect::Test0, D3DXVECTOR3(0.f, 1.f, 0.f));

				//�g��k��
				CEffect::GetInstance().SetScale(hEffect, D3DXVECTOR3(0.8f, 0.8f, 0.8f));

				//��](Y����])

				CEffect::GetInstance().SetRotation(hEffect, D3DXVECTOR3(m_pPlayerManager->GetRotation(i)));

				//�ʒu��Đݒ�
				CEffect::GetInstance().SetLocation(hEffect, D3DXVECTOR3(m_pPlayerManager->GetPosition(i)));

			}
			if (GetAsyncKeyState('T') & 0x0001) {
				CEffect::GetInstance().Stop(hEffect);
			}
		}

	}
#endif

	//���s���(�m�F�p).
	//����.
	if (GetKey('K') & 0x8000)
	{
		//BGM�̃��[�v��~.
		CSoundManager::Stop(CSoundManager::BGM_Bonus);

		m_SceneType = CSceneType::Result;
	}
	//�s�k.
	//�̗͂��Ȃ��Ȃ邩
	if (GetKey('L') & 0x8000)
	{
		//BGM�̃��[�v��~.
		CSoundManager::Stop(CSoundManager::BGM_Bonus);

		m_SceneType = CSceneType::Result;
	}
	// C�L�[�����ꂽ�瑀��v���C���[�؂�ւ�
	if (GetKey('C') & 0x8000)
	{
		m_pPlayerManager->SwitchActivePlayer();
	}

	m_pWallTop->Update();
	m_pWallBottom->Update();
	m_pWallLeft->Update();
	m_pWallRight->Update();

	Collision();
}



void CGameMain::Draw()
{
	auto* pContext = CDirectX11::GetInstance().GetContext();

	//��ʃT�C�Yfloat�ň���D3D11_VIEWPORT�̌^�ɍ��킹��
	const float W = static_cast<float>(WND_W);
	const float H = static_cast<float>(WND_H);

	//2x2�����̒�`
	const int COLS = 2;		//2��1�ɂ�������ʁA2�Ȃ�l���.
	const int ROWS = 2;		//2��1�ɂ�������ʁA2�Ȃ�l���.
	const int MAX_VIEWS = COLS * ROWS;					//�������ĕ\���ł���ő�r���[��
	const int VIEWS = std::min(PLAYER_MAX, MAX_VIEWS);	//min�ŏ������ق��ɍ��킹��

	//1�r���[�|�[�g����`�悷�鏈��������_�ɂ܂Ƃ߂�
	auto DrawOneViewport = [&](std::shared_ptr<CCamera> camera, std::shared_ptr<CPlayer> owner)
	{
		//�J�����X�V
		camera->Update();

		//�X�i�b�v�V���b�g��const�Q�ƂŃL���v�`��
		D3DXMATRIX& view	= camera->m_mView;
		D3DXMATRIX& proj	= camera->m_mProj;
		LIGHT&		light	= camera->m_Light;
		CAMERA&		paramC	= camera->m_Camera;

		//�v���C���[��`��.�����őS���`��
		for (int players = 0; players < PLAYER_MAX; ++players)
		{
			if (auto p = m_pPlayerManager->GetControlPlayer(players))
			{
				p->Draw(view, proj, light, paramC);
			}
		}

	//�I�u�W�F�N�g�̕`��.
	//�e�`��.
	m_pShotManager->Draw(view, proj, light, paramC);

	//�n�ʕ`��
	if (owner) m_pGround->SetPlayer(*owner);
	{
		m_pGround->Draw(view, proj, light, paramC);

		m_pWallTop->Draw(view, proj, light, paramC);
		m_pWallBottom->Draw(view, proj, light, paramC);
		m_pWallLeft->Draw(view, proj, light, paramC);
		m_pWallRight->Draw(view, proj, light, paramC);
		//�G�t�F�N�g������ł��
	};
	//�A�C�e���{�b�N�X�`��.
	m_pItemBoxManager->Draw(view, proj, light, paramC);

//4��ʂ̎��̕\��.
		//�O��֌W����.
		CDirectX11::GetInstance().SetDepth(false);
		//UI.
		for (int i = 0; i < HP_MAX; i++)
		{
			m_pSpriteHitPoint[i]->Draw();
		}
		CDirectX11::GetInstance().SetDepth(true);

	//�G�t�F�N�g������ł��

	};


	//�����r���[�̃��[�v
	for (int i = 0; i < VIEWS; ++i)
	{
		//�r���[�|�[�g�ݒ�
		const D3D11_VIEWPORT vp = MakeGridViewport(i, COLS, ROWS, W, H);
		pContext->RSSetViewports(1, &vp);

		//�J�����Q�Ƃ�擾.�Q�ƊO���Ŏ��̂𒼐ڈ���
		std::shared_ptr<CCamera> camera = m_pCameras[i];

		std::shared_ptr<CPlayer> owner = m_pPlayerManager->GetControlPlayer(i);
			////////�f�o�b�O�e�L�X�g�̕`��
			//////m_pDbgText->SetColor(0.9f, 0.6f, 0.f);	//�F�̐ݒ�
			//////m_pDbgText->Render(_T("ABCD"), 10, 100);

		//1�r���[����`��
		DrawOneViewport(camera, owner);
			////////�f�o�b�O�e�L�X�g(���l����)�̕`��
			//////m_pDbgText->SetColor(1.f, 0.f, 0.f);
			//////TCHAR dbgText[64];
			//////_stprintf_s(dbgText, _T("Float:%f, %f"), 1.f, 2.2f);
			//////m_pDbgText->Render(dbgText, 10, 110);

		//4��ʂ̎��̕\��.
		//�O��֌W����.
		CDirectX11::GetInstance().SetDepth(false);
		//�v���C���[�ԍ��̕`��.
		switch (i)
		{
		case 0:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		case 1:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		case 2:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		case 3:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		default:
			break;
		}
		//�L�����̕`��.
		m_pSpriteKillNomber[i]->Draw();
		CDirectX11::GetInstance().SetDepth(true);

	}

	//�S��ʃr���[�|�[�g�ɖ߂�
	D3D11_VIEWPORT fullvp = {};
	fullvp.TopLeftX = 0;		//�r���[�|�[�g�����X���W
	fullvp.TopLeftY = 0;		//�r���[�|�[�g�����Y���W
	fullvp.Width	= 1920;		//�r���[�|�[�g�̕�.�����ō���͉�ʑS��ʂ��Ƃ���
	fullvp.Height	= 1080;		//�r���[�|�[�g�̍���
	fullvp.MinDepth = 0.0f;		//�[�x�o�b�t�@�̍ŏ��l
	fullvp.MaxDepth = 1.0f;		//�[�x�o�b�t�@�̍ő�l
	pContext->RSSetViewports(1, &fullvp);


////1��ʂ̎��̕\��.
	//�O��֌W����.
	CDirectX11::GetInstance().SetDepth(false);
	//�^�C�}�[�̘g�̕`��.
	m_pSpriteTimerFrame->Draw();
	//�^�C�}�[�̕`��.
	m_pSpriteTimer->Draw();
	//�^�C�}�[�̕`��.
	m_pSpriteTimerArrow->Draw();
	//�^�C�}�[�`��.
	m_Timer->Draw();
	CDirectX11::GetInstance().SetDepth(true);

}

void CGameMain::Init()
{
	//�J�����ʒu�ݒ�.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//�v���C���[�}�l�[�W���[����e�v���C���[�̈ʒu��擾
		D3DXVECTOR3 pos = m_pPlayerManager->GetPosition(i);

		m_pCameras[i]->SetCameraPos(pos.x, pos.y, pos.z);
		m_pCameras[i]->SetLightPos(0.f, 2.f, 5.f);
	}
	//�n�ʂ̑傫���ݒ�.
	m_pGround->SetScale(0.4f, 0.4f, 0.4f);

	//�A�C�e���{�b�N�X�̐ݒ�.
	m_pItemBoxManager->SetPosition(-10.f, 20.f, 0.f);
	m_pItemBoxManager->SetRotation(0.f, 0.f, 0.f);
	m_pItemBoxManager->SetScale(0.2f, 0.2f, 0.2f);

//-----���S�\���p���W-----.
	//�������Ԙg�̉摜�̐ݒ�.
	m_pSpriteTimerFrame->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
	//�������ԉ~�̉摜�̐ݒ�.
	m_pSpriteTimer->SetPosition(WND_W / 2.f - 74.f, WND_H / 2 - 32.f, 0.f);
	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);



////-----���Ԕ��\�p-----.
//	//�������Ԙg�̉摜�̐ݒ�.
//	m_pSpriteTimerFrame->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
//	//�������ԉ~�̉摜�̐ݒ�.
//	m_pSpriteTimer->SetPosition(WND_W - 160.f, WND_H - 96.f, 0.f);
//	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);


////-----���S�\���p���W-----.
//	//.
//	m_pSpritePlayerIcon->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpritePlayerIcon->SetRotation(0.f, 0.f, 0.f);
//	m_pSpritePlayerIcon->SetScale(1.f, 1.f, 0.f);

//-----���Ԕ��\�p-----.
	//�v���C���[�ԍ��̉摜�̐ݒ�.
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//�v���C���[1P.
		case 2:			//�v���C���[3P.
			m_pSpritePlayerIcon[i]->SetPosition(0.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		case 1:			//�v���C���[2P.
		case 3:			//�v���C���[4P.
			m_pSpritePlayerIcon[i]->SetPosition(WND_W - 256.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		default:
			break;
		}
	}

////-----���S�\���p���W-----.
//	//.
//	m_pSpriteKillNomber->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpriteKillNomber->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteKillNomber->SetScale(1.f, 1.f, 0.f);

//-----���Ԕ��\�p-----.
	//�v���C���[�ԍ��̉摜�̐ݒ�.
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//�v���C���[1P.
		case 2:			//�v���C���[3P.
			m_pSpriteKillNomber[i]->SetPosition(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		case 1:			//�v���C���[2P.
		case 3:			//�v���C���[4P.
			m_pSpriteKillNomber[i]->SetPosition(WND_W - 320.f, 0.f, 0.f);//320:256�T�C�Y�̉摜�ɕ�����64�𑫂�����.
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		default:
			break;
		}
	}


////-----���S�\���p���W-----.
//	//.
//	m_pSpriteHitPoint->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpriteHitPoint->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteHitPoint->SetScale(1.f, 1.f, 0.f);

//-----���Ԕ��\�p-----.
	//HP�̉摜�̐ݒ�.
	for (int i = 0; i < HP_MAX; i++)
	{
		if (i <= 0)
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2 - 128.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
		else
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2 , 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
	}


	//�������Ԃ̕����T�C�Y.
	m_pDbgText->SetFontSize(5.0f);

//-----���S�\���p���W-----.
	//�Q�[���ŗV�ׂ�(�N���A��ʂɑJ�ڂ���)���ԁ�����.
	m_Timer->StartTimer(TIME);
	m_Timer->SetDebugFont(m_pDbgText);
	m_Timer->SetTimerPosition(WND_W / 2 - 15.f, WND_H / 2 - 30.f);

////-----���Ԕ��\�p-----.
//	//�Q�[���ŗV�ׂ�(�N���A��ʂɑJ�ڂ���)���ԁ�����.
//	m_Timer->StartTimer(TIME);
//	m_Timer->SetDebugFont(m_pDbgText);
//	m_Timer->SetTimerPosition(WND_W - 96.f, WND_H - 96.f);

}

void CGameMain::Destroy()
{

}

void CGameMain::Create()
{
	//Effect�N���X
	CEffect::GetInstance().Create(
		CDirectX11::GetInstance().GetDevice(),
		CDirectX11::GetInstance().GetContext());

	//UIObject�̃C���X�^���X����.
	m_pSpriteTimerFrame = std::make_shared<CUIObject>();
	m_pSpriteTimer		= std::make_shared<CUIObject>();
	m_pSpriteTimerArrow = std::make_shared<CUIObject>();
	//HP�̕���������.
	for (int i = 0; i < HP_MAX; i++)
	{
		m_pSpriteHitPoint[i] = std::make_shared<CUIObject>();
	}
	//�v���C���[�̕���������.
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		m_pSpritePlayerIcon[i] = std::make_shared<CUIObject>();
	}
	//�L�����̕���������.
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		m_pSpriteKillNomber[i] = std::make_shared<CUIObject>();
	}

	//UI�n�̃C���X�^���X����.
	m_pSprite2DTimerFrame	= std::make_shared<CSprite2D>();
	m_pSprite2DTimer		= std::make_shared<CSprite2D>();
	m_pSprite2DTimerArrow	= std::make_shared<CSprite2D>();
	m_pSprite2DKillNomber	= std::make_shared<CSprite2D>();
	m_pSprite2DHitPoint		= std::make_shared<CSprite2D>();
	//�v���C���[�̕���������.
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		m_pSprite2DPlayerIcon[i] = std::make_shared<CSprite2D>();
	}

	//�X�v���C�g�̃C���X�^���X�쐬.
	m_pSpriteGround = std::make_unique<CSprite3D>();
	m_pSpritePlayer = std::make_unique<CSprite3D>();
	m_pSpriteExplosion = std::make_shared<CSprite3D>();

	//�X�^�e�B�b�N���b�V���I�u�W�F�N�g�̃C���X�^���X�쐬
	m_pStcMeshObj = std::make_unique<CStaticMeshObject>();

	//�X�^�e�B�b�N���b�V���̃C���X�^���X�쐬
	m_pStaticMeshGround			= std::make_shared<CStaticMesh>();
	m_pStaticMeshBSphere		= std::make_shared<CStaticMesh>();
	m_pStaticMeshItemBox		= std::make_shared<CStaticMesh>();

	// ��Ԃ̃��b�V��.
	m_pStaticMesh_TankBodyRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonBlue	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyGreen		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonGreen	= std::make_shared<CStaticMesh>();

	// �e�̃��b�V��.
	m_pStaticMesh_BulletRed			= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletYellow		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletGreen		= std::make_shared<CStaticMesh>();

	// �ǂ̃��b�V��
	m_pStaticMeshWallW				= std::make_shared<CStaticMesh>();
	m_pStaticMeshWallH				= std::make_shared<CStaticMesh>();

	//�f�o�b�O�e�L�X�g�̃C���X�^���X�쐬
	m_pDbgText = std::make_unique<CDebugText>();

	//�v���C���[�ƖC���̃C���X�^���X����
	m_pPlayerManager = std::make_shared<CPlayerManager>();
	
	//�}�l�[�W���[�͈�񂾂�Initialize
	m_pPlayerManager->Initialize();

	//�e�N���X�̃C���X�^���X�쐬
	m_pShotManager = std::make_shared<CShotManager>();
	m_pShotManager->Initialize(PLAYER_MAX);


	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//�v���C���[i�̈ʒu����������炷
		float offsetX = (i % 2) * 12.0f;
		float offsetZ = (i / 2) * 12.0f;
		m_pPlayerManager->SetPlayerPosition(i, D3DXVECTOR3(offsetX, 0.0f, offsetZ));
		//��]��ݒ�.
		m_pPlayerManager->SetPlayerRotation(i, D3DXVECTOR3(0.f, 0.f, 0.f));

		////�C���̐��������
		//auto cannon = std::make_unique<CCannon>();

		//�J���������E�Z�b�g�A�b�v
		auto camera = std::make_unique<CCamera>();
		camera->SetTargetPos(m_pPlayerManager->GetPosition(i));
		camera->SetTargetRotY(m_pPlayerManager->GetRotation(i).y);
		m_pCameras[i] = std::move(camera);

	}

	//�n�ʃN���X�̃C���X�^���X�쐬.
	m_pGround = std::make_unique<CGround>();

	//�������Ԃ̃C���X�^���X����.
	m_Timer = std::make_shared<CTimer>();

	// �ǃN���X�̃C���X�^���X����
	m_pWallTop		= std::make_shared<CWall>();
	m_pWallBottom	= std::make_shared<CWall>();
	m_pWallLeft		= std::make_shared<CWall>();
	m_pWallRight	= std::make_shared<CWall>();
	//�A�C�e���}�l�[�W���[�N���X�̃C���X�^���X����.
	m_pItemBoxManager = std::make_shared<CItemBoxManager>();
	m_pItemBoxManager->Create();
}

HRESULT CGameMain::LoadData()
{
	//�f�o�b�O�e�L�X�g�̓ǂݍ���.
	if (FAILED(m_pDbgText->Init(CDirectX11::GetInstance())))
	{
		return E_FAIL;
	}

	//Effect�N���X
	if (FAILED(CEffect::GetInstance().LoadData())) {
		return E_FAIL;
	}

	//�^�C�}�[�摜�̃X�v���C�g�ݒ�.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		1920, 1080,		//�`�敝,����.
		1920, 1080,		//���摜�̕�,����.
		1920, 1080		//�A�j���[�V��������Ȃ��̂ŁA0�ł���.
	};
	//�^�C�}�[�g�摜�̃X�v���C�g�ݒ�.
	CSprite2D::SPRITE_STATE TIMER_SIZE = {
		256, 256,		//�`�敝,����.
		256, 256,		//���摜�̕�,����.
		256, 256		//�A�j���[�V��������Ȃ��̂ŁA0�ł���.
	};
	//�^�C�}�[�g�摜�̃X�v���C�g�ݒ�.
	CSprite2D::SPRITE_STATE ICON_SIZE = {
		256, 256,		//�`�敝,����.
		256, 256,		//���摜�̕�,����.
		256, 256		//�A�j���[�V��������Ȃ��̂ŁA0�ł���.
	};
	//�������Ԃ̘g�̓ǂݍ���.
	m_pSprite2DTimerFrame	->Init(_T("Data\\Texture\\UI\\Timer\\TimerFrame.png"), WH_SIZE, false);
	m_pSprite2DTimer		->Init(_T("Data\\Texture\\UI\\Timer\\Timer.png"), TIMER_SIZE, false);
	m_pSprite2DTimerArrow	->Init(_T("Data\\Texture\\UI\\Timer\\TimerArrow.png"), TIMER_SIZE, true);
	m_pSprite2DKillNomber	->Init(_T("Data\\Texture\\UI\\KillNum.png"), ICON_SIZE, false);
	m_pSprite2DHitPoint		->Init(_T("Data\\Texture\\UI\\HP.png"), ICON_SIZE, true);

	//�摜��A�^�b�`.
	m_pSpriteTimerFrame	->AttachSprite(m_pSprite2DTimerFrame);
	m_pSpriteTimer		->AttachSprite(m_pSprite2DTimer);
	m_pSpriteTimerArrow	->AttachSprite(m_pSprite2DTimerArrow);
	//HP�̕������A�^�b�`.
	for (int i = 0; i < HP_MAX; i++)
	{
		m_pSpriteHitPoint[i]->AttachSprite(m_pSprite2DHitPoint);
	}
	//�L�����̕������A�^�b�`.
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		m_pSpriteKillNomber[i]->AttachSprite(m_pSprite2DKillNomber);
	}
	//�v���C���[�̕������A�^�b�`.
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		switch (i)
		{
			case 0:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\OneP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
			case 1:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TwoP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
			case 2:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TreeP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
			case 3:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\FourP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
		default:
			break;
		}
	}

	//�n�ʃX�v���C�g�̍\����
	CSprite3D::SPRITE_STATE SSGround;
	SSGround.Disp.w = 1.f;
	SSGround.Disp.h = 1.f;
	SSGround.Base.w = 256.f;
	SSGround.Base.h = 256.f;
	SSGround.Stride.w = 256.f;
	SSGround.Stride.h = 256.f;
	//�n�ʃX�v���C�g�̓ǂݍ���.
	m_pSpriteGround->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Ground.png"), SSGround);

	//�v���C���[�X�v���C�g�̍\����
	CSprite3D::SPRITE_STATE SSPlayer =
	{ 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };
	//�v���C���[�X�v���C�g�̓ǂݍ���.
	m_pSpritePlayer->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Player.png"), SSPlayer);

	//�����X�v���C�g�̍\����
	CSprite3D::SPRITE_STATE SSExplosion =
	{ 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };
	//�����X�v���C�g�̓ǂݍ���.
	m_pSpriteExplosion->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\explosion.png"), SSExplosion);

	//--------------------------------------------------------------------------
	// 	   �摜�̓ǂݍ���.
	//--------------------------------------------------------------------------
	//�X�^�e�B�b�N���b�V���̓ǂݍ���
	m_pStaticMeshGround->Init(_T("Data\\Mesh\\Static\\Stage\\stage.x"));
	m_pStaticMeshItemBox->Init(_T("Data\\Mesh\\Static\\ItemBox\\ItemBox.x"));

	// ���(��)
	m_pStaticMesh_TankBodyRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Body\\Body.x"));
	m_pStaticMesh_TankCannonRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Cannon\\Cannon.x"));

	// ���(��)
	m_pStaticMesh_TankBodyYellow->Init(_T("Data\\Mesh\\Static\\Tank\\Yellow\\Body\\Body.x"));
	m_pStaticMesh_TankCannonYellow->Init(_T("Data\\Mesh\\Static\\Tank\\Yellow\\Cannon\\Cannon.x"));

	// ���(��)
	m_pStaticMesh_TankBodyBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Body\\Body.x"));
	m_pStaticMesh_TankCannonBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Cannon\\Cannon.x"));

	// ���(��)
	m_pStaticMesh_TankBodyGreen->Init(_T("Data\\Mesh\\Static\\Tank\\Green\\Body\\Body.x"));
	m_pStaticMesh_TankCannonGreen->Init(_T("Data\\Mesh\\Static\\Tank\\Green\\Cannon\\Cannon.x"));
	
	// �e(��)
	m_pStaticMesh_BulletRed->Init(_T("Data\\Mesh\\Static\\Bullet\\Red\\Ball.x"));
	// �e(��)
	m_pStaticMesh_BulletYellow->Init(_T("Data\\Mesh\\Static\\Bullet\\Yellow\\Ball.x"));
	// �e(��)
	m_pStaticMesh_BulletBlue->Init(_T("Data\\Mesh\\Static\\Bullet\\Blue\\Ball.x"));
	// �e(��)
	m_pStaticMesh_BulletGreen->Init(_T("Data\\Mesh\\Static\\Bullet\\Green\\Ball.x"));
	
	// ��
	m_pStaticMeshWallW->Init(_T("Data\\Mesh\\Static\\Wall\\Wall1.x"));
	m_pStaticMeshWallH->Init(_T("Data\\Mesh\\Static\\Wall\\Wall2.x"));

	//�o�E���f�B���O�X�t�B�A(�����蔻��p).
	m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Sphere.x"));


	// ���ꂼ��̃v���C���[�ɐF�ɂ�������Ԃ�A�^�b�`
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		switch (i)
		{
		case 0:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyRed, m_pStaticMesh_TankCannonRed);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletRed);
			break;
		case 1:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyYellow, m_pStaticMesh_TankCannonYellow);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletYellow);
			break;
		case 2:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletGreen);
			break;
		case 3:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletBlue);
			break;
		default:
			break;
		}
	}

	//�X�^�e�B�b�N���b�V����ݒ�
	m_pGround->AttachMesh(m_pStaticMeshGround);

	//�A�C�e���{�b�N�X�}�l�[�W���[�Ƀ��b�V����ݒ�.
	m_pItemBoxManager->AttachMesh(m_pStaticMeshItemBox);

	////�o�E���f�B���O�X�t�B�A�̍쐬.
	//m_pPlayer->CreateBSphareForMesh(*m_pStaticMeshBSphere);

	m_pWallTop->AttachMesh(m_pStaticMeshWallW);
	m_pWallBottom->AttachMesh(m_pStaticMeshWallW);
	m_pWallLeft->AttachMesh(m_pStaticMeshWallH);
	m_pWallRight->AttachMesh(m_pStaticMeshWallH);

	CreateBounding();

	return S_OK;
}

void CGameMain::SetPosition()
{
	// ��̕ǂ̏������W��ݒ�
	m_pWallTop->SetPosition(0, 0, 30);
	m_pWallTop->SetRotation(0, 0, 0);

	// ���̕ǂ̏������W��ݒ�
	m_pWallBottom->SetPosition(0, 0, -30);
	m_pWallBottom->SetRotation(0, 0, 0);

	// ���̕ǂ̏������W��ݒ�
	m_pWallLeft->SetPosition(-30, 0, 0);
	m_pWallLeft->SetRotation(0, 0, 0);

	// �E�̕ǂ̏������W��ݒ�
	m_pWallRight->SetPosition(30, 0, 0);
	m_pWallRight->SetRotation(0, 0, 0);
}

void CGameMain::CreateBounding()
{
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//�v���C���[�̃o�E���f�B���O�̍쐬.
		switch (i)
		{
		case 0:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyRed, m_pStaticMesh_TankCannonRed);
			break;
		case 1:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyYellow, m_pStaticMesh_TankCannonYellow);
			break;
		case 2:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
			break;
		case 3:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			break;
		}
		//�v���C���[�̓����蔻���쐬.
		m_pPlayerManager->CreateCollider(i);
	}
	
	//��̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallTop->CreateBBoxForMesh(*m_pStaticMeshWallW);
	//��̕ǂ̓����蔻���ݒ�.
	m_pWallTop->CreateBoxCollider(m_pWallTop->GetMinPos(), m_pWallTop->GetMaxPos());

	//���̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallBottom->CreateBBoxForMesh(*m_pStaticMeshWallW);
	//���̕ǂ̓����蔻���ݒ�.
	m_pWallBottom->CreateBoxCollider(m_pWallBottom->GetMinPos(), m_pWallBottom->GetMaxPos());

	//���̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallLeft->CreateBBoxForMesh(*m_pStaticMeshWallH);
	//���̕ǂ̓����蔻���ݒ�.
	m_pWallLeft->CreateBoxCollider(m_pWallLeft->GetMinPos(), m_pWallLeft->GetMaxPos());

	//�E�̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallRight->CreateBBoxForMesh(*m_pStaticMeshWallH);
	//�E�̕ǂ̓����蔻���ݒ�.
	m_pWallRight->CreateBoxCollider(m_pWallRight->GetMinPos(), m_pWallRight->GetMaxPos());


	//�A�C�e���{�b�N�X�̃o�E���f�B���O�̍쐬.
	m_pItemBoxManager->CreateBounding(m_pStaticMeshItemBox);
	//�A�C�e���{�b�N�X�̓����蔻���ݒ�.
	m_pItemBoxManager->CreateCollider();
	
}

void CGameMain::Collision()
{
	// �ǂƃv���C���[�̓����蔻��.
	WalltoPlayer();

	// �A�C�e���{�b�N�X�ƃv���C���[�̓����蔻��.
	ItemBoxtoPlayer();

}

void CGameMain::WalltoPlayer()
{
	const float pushStrength = 0.1f; // �����߂��̋����i�t���[�����Ƃɒ����\�j

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// i �Ԃ̃v���C���[��擾
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		// �ǂƂ̏Փ˂�`�F�b�N���Ė@���x�N�g�������
		D3DXVECTOR3 push(0.0f, 0.0f, 0.0f);

		if (Coll && m_pWallTop->GetCollider() &&
			Coll->CheckCollision(*m_pWallTop->GetCollider()))
		{
			push += D3DXVECTOR3(0.f, 0.f, -1.f);
		}
		if (Coll && m_pWallBottom->GetCollider() &&
			Coll->CheckCollision(*m_pWallBottom->GetCollider()))
		{
			push += D3DXVECTOR3(0.f, 0.f, 1.f);
		}
		if (Coll && m_pWallLeft->GetCollider() &&
			Coll->CheckCollision(*m_pWallLeft->GetCollider()))
		{
			push += D3DXVECTOR3(1.f, 0.f, 0.f);
		}
		if (Coll && m_pWallRight->GetCollider() &&
			Coll->CheckCollision(*m_pWallRight->GetCollider()))
		{
			push += D3DXVECTOR3(-1.f, 0.f, 0.f);
		}

		// �����ǂɓ��������ꍇ�͐��K�����ĉ����߂��x�N�g������R��
		if (D3DXVec3Length(&push) > 0.f)
		{
			D3DXVec3Normalize(&push, &push);
			push *= pushStrength;
		}

		player->GetBody()->PushBack(push);
	}
}

//�A�C�e���{�b�N�X�ƃv���C���[�̓����蔻��.
void CGameMain::ItemBoxtoPlayer()
{
	//�A�C�e���{�b�N�X�ƃv���C���[�̓����蔻��.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto playerColl = player->GetBody()->GetCollider();

		//�A�C�e���{�b�N�X�}�l�[�W���[����A�C�e���{�b�N�X��擾.
		auto itemBoxes = m_pItemBoxManager->GetItem();

		//�A�C�e���{�b�N�X�S�Ăɑ΂��ē����蔻���`�F�b�N.
		for (auto& itemBox : itemBoxes)
		{
			if (playerColl && itemBox->GetCollider() &&
				playerColl->CheckCollision(*itemBox->GetCollider()))
			{
				//�Փ˂����ꍇ�̏���.
				itemBox->HitPlayer();
			}
		}
	}
}

//��ʂ�O���b�h�ɕ��������Ƃ��Aidx�Ԗڂ̃}�X�ɑΉ�����
//D3D11_VIEWPORT��쐬���ĕԂ��֐�
D3D11_VIEWPORT CGameMain::MakeGridViewport(int idx, int cols, int rows, float totalW, float totalH)
{
	//�ǂ̃}�X���A��E�s��Z�o
	const int col = idx % cols;	//��ԍ�
	const int row = idx / cols;	//�s�ԍ�

	//�r���[�|�[�g��쐬
	D3D11_VIEWPORT vp{};					//�[��������
	vp.TopLeftX = (totalW / cols) * col;	//����(0,0)����̃I�t�Z�b�g
	vp.TopLeftY = (totalH / rows) * row;
	vp.Width = totalW / cols;
	vp.Height = totalH / rows;
	vp.MinDepth = 0.0f;						//�[�x�o�b�t�@�̍ŏ��l�ݒ�
	vp.MaxDepth = 1.0f;						//�[�x�o�b�t�@�̍ő�l�ݒ�
	return vp;
}


CSceneType CGameMain::GetSceneType() const
{
	return m_SceneType;
}


//-----�e�ݒ��֐���-----.

//�������ԉ摜�̐ݒ�.
void CGameMain::EachSettingTimer()
{
	//-----���S�\���p���W-----.
		//�������Ԙg�̉摜�ݒ�.
	m_pSpriteTimerFrame->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
	//�������ԉ~�̉摜�ݒ�.
	m_pSpriteTimer->SetPosition(WND_W / 2.f - 74.f, WND_H / 2 - 32.f, 0.f);
	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);
	//���v�̐j�̉摜�ݒ�.
	m_pSpriteTimerArrow->SetPosition(WND_W / 2.f - 42.f, WND_H / 2, 0.f);
	m_pSpriteTimerArrow->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerArrow->SetScale(0.25f, 0.25f, 0.f);

	//-----���S�\���p���W-----.
	//�Q�[���ŗV�ׂ�(�N���A��ʂɑJ�ڂ���)���ԁ�����.
	m_Timer->StartTimer(TIME);
	m_Timer->SetDebugFont(m_pDbgText);
	m_Timer->SetTimerPosition(WND_W / 2 - 15.f, WND_H / 2 - 30.f);

	////-----���Ԕ��\�p-----.
	//	//�Q�[���ŗV�ׂ�(�N���A��ʂɑJ�ڂ���)���ԁ�����.
	//	m_Timer->StartTimer(TIME);
	//	m_Timer->SetDebugFont(m_pDbgText);
	//	m_Timer->SetTimerPosition(WND_W - 96.f, WND_H - 96.f);

}
//�v���C���[�ԍ��摜�̐ݒ�.
void CGameMain::EachSettingPlayerNumber()
{
	//-----���Ԕ��\�p-----.
		//�v���C���[�ԍ��̉摜�̐ݒ�.
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//�v���C���[1P.
		case 2:			//�v���C���[3P.
			m_pSpritePlayerIcon[i]->SetPosition(0.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		case 1:			//�v���C���[2P.
		case 3:			//�v���C���[4P.
			m_pSpritePlayerIcon[i]->SetPosition(WND_W - 256.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		default:
			break;
		}
	}
}
//�|�������摜�̐ݒ�.
void CGameMain::EachSettingKillNumber()
{
	////-----���S�\���p���W-----.
	//	//.
	//	m_pSpriteKillNomber->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
	//	m_pSpriteKillNomber->SetRotation(0.f, 0.f, 0.f);
	//	m_pSpriteKillNomber->SetScale(1.f, 1.f, 0.f);

	//-----���Ԕ��\�p-----.
		//�v���C���[�ԍ��̉摜�̐ݒ�.
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//�v���C���[1P.
		case 2:			//�v���C���[3P.
			m_pSpriteKillNomber[i]->SetPosition(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		case 1:			//�v���C���[2P.
		case 3:			//�v���C���[4P.
			m_pSpriteKillNomber[i]->SetPosition(WND_W - 320.f, 0.f, 0.f);//320:256�T�C�Y�̉摜�ɕ�����64�𑫂�����.
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		default:
			break;
		}
	}
}
//�|�������摜�̐ݒ�.
void CGameMain::EachSettingHitPoint()
{
	//-----4��ʗp-----.
		//HP�̉摜�̐ݒ�.
	for (int i = 0; i < HP_MAX; i++)
	{
		if (i <= 0)
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2 - 128.f, 64.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
		else
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2, 64.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
	}
}