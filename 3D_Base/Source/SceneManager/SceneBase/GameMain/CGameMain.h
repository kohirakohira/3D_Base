#pragma once
//-----�p������N���X-----
#include "SceneManager//SceneBase//CSceneBase.h" // �V�[�����N���X

//-----���C�u����-----
#include <algorithm>
#include <vector>
#include <array>	

//-----�f�o�b�N�e�L�X�g-----
#include "Assets//DebugText//CDebugText.h"		// �f�o�b�O�e�L�X�g�N���X

//-----�X�v���C�g-----
#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3D�X�v���C�g�N���X.
#include "Assets//Sprite//Sprite2D//CSprite2D.h" // 2D�X�v���C�g�N���X.

//-----�X�v���C�g�I�u�W�F�N�g-----
#include "GameObject//SpriteObject//CSpriteObject.h"

//-----�X�v���C�g-----
#include "GameObject//SpriteObject//Explosion//CExplosion.h" // �����X�v���C�g

//-----���b�V��-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // �X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X

//-----�L�����N�^�[-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // �L�����N�^�[�N���X
#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h" // �v���C���[�}�l�[�W���[�N���X
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h" // �e�N���X�}�l�[�W���[
#include "GameObject//StaticMeshObject//Ground//CGround.h" // �n�ʃN���X
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h" // ��ԁF�C���N���X

#include "GameObject/StaticMeshObject/ItemBoxManager/CItemBoxManager.h"//�A�C�e���{�b�N�X�}�l�[�W���[�N���X.

#include "Camera//CCamera.h" //�J�����N���X


//-----��-----
#include "GameObject//StaticMeshObject//Wall//CWall.h"

// UI
//-------------------------------
#include "GameObject//UI//CUIObject//CUIObject.h" // UI�I�u�W�F�N�g�N���X
#include "GameObject//UI//Timer//CTimer.h"		  // �^�C�}�[�N���X

#include "Global.h"

class CGameMain
	:public CSceneBase
{
public:

	CGameMain(HWND hWnd);
	~CGameMain()override;


	//����֐�.
	void Update()override;
	//�`��֐�.
	void Draw()override;
	//�������֐�.
	void Init()override;
	//����֐�.
	void Destroy()override;
	//�C���X�^���X�쐬.
	void Create()override;
	//�f�[�^�̓ǂݍ���.
	HRESULT LoadData()override;

	// �Q�[���J�n���̏������W��ݒ�
	void SetPosition();

	// �����蔻��̃o�E���f�B���O�Ɠ����蔻��̍쐬
	void CreateBounding();

	//�����蔻�菈��������ɓ����.
	void Collision();

	// �ǂƃv���C���[�̓����蔻��
	void WalltoPlayer();

	// �A�C�e���{�b�N�X�ƃv���C���[�̓����蔻��
	void ItemBoxtoPlayer();

	//��ʂ�O���b�h�ɕ��������Ƃ��Aidx�Ԗڂ̃}�X�ɑΉ�����
	//D3D11_VIEWPORT��쐬���ĕԂ��֐�
	static D3D11_VIEWPORT MakeGridViewport(int idx, int cols, int rows, float totalW, float totalH);

	//�V�[���̎��.
	CSceneType GetSceneType() const override;



	//���b�s���O�֐�.
	//�������ԉ摜�̐ݒ�.
	void EachSettingTimer();
	//�v���C���[�ԍ��摜�̐ݒ�.
	void EachSettingPlayerNumber();
	//�|�������摜�̐ݒ�.
	void EachSettingKillNumber();
	//�|�������摜�̐ݒ�.
	void EachSettingHitPoint();




public:		//�N���X�p.
	//�萔�錾.
	static constexpr int HP_MAX = 2;			//�ő�HP.
	static constexpr int PLAYERNUM_MAX = 4;		//�v���C���[�ԍ�.
	static constexpr int KILLNUM_MAX = 4;		//�L����.

	//�E�B���h�E�n���h��.
	HWND		m_hWnd;

	//�J����.
	//std::vector<std::shared_ptr<CCamera>> m_pCameras;
	std::array<std::shared_ptr<CCamera>, PLAYER_MAX> m_pCameras;

	//�f�o�b�O�e�L�X�g.
	std::shared_ptr<CDebugText>		m_pDbgText;

	//�Q�[����ň���UI�n.
	std::shared_ptr<CSprite2D>									m_pSprite2DTimerArrow;			//���v�̐j.
	std::shared_ptr<CSprite2D>									m_pSprite2DTimerFrame;			//�������Ԃ̘g.
	std::shared_ptr<CSprite2D>									m_pSprite2DTimer;				//�������Ԃ̎��v�g.
	std::shared_ptr<CSprite2D>									m_pSprite2DKillNomber;			//�L�����̉摜.
	std::shared_ptr<CSprite2D>									m_pSprite2DHitPoint;			//HP�̉摜.
	std::array < std::shared_ptr<CSprite2D>, PLAYERNUM_MAX>		m_pSprite2DPlayerIcon;			//�v���C���[�ԍ��摜.

	//�X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X(UI).
	std::array<std::shared_ptr<CUIObject>, PLAYERNUM_MAX>	m_pSpritePlayerIcon;			//�v���C���[�A�C�R��.
	std::array<std::shared_ptr<CUIObject>, KILLNUM_MAX>		m_pSpriteKillNomber;			//�L�����A�C�R��.
	std::array<std::shared_ptr<CUIObject>, HP_MAX>			m_pSpriteHitPoint;				//HP�A�C�R��.
	std::shared_ptr<CUIObject>								m_pSpriteTimerFrame;			//�������Ԃ̘g.
	std::shared_ptr<CUIObject>								m_pSpriteTimer;					//�������Ԃ̎��v�g.
	std::shared_ptr<CUIObject>								m_pSpriteTimerArrow;			//時計の針.

	//�Q�[���ň����X�v���C�g�f�[�^(�g���܂킷����).
	std::unique_ptr<CSprite3D>		m_pSpriteGround;
	std::unique_ptr<CSprite3D>		m_pSpritePlayer;
	std::shared_ptr<CSprite3D>		m_pSpriteExplosion;

	//�X�^�e�B�b�N���b�V��(�g���܂킷����)
	std::shared_ptr<CStaticMesh>	m_pStaticMeshGround;		//�n��
	std::shared_ptr<CStaticMesh>	m_pStaticMeshBSphere;		//�o�E���f�B���O�X�t�B�A(�����蔻��p).
	std::shared_ptr<CStaticMesh>	m_pStaticMeshItemBox;		//�A�C�e���{�b�N�X.

	// ���
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyRed;		// �ԑ̐�
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonRed;	// �C����
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyYellow;	// �ԑ̉�
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonYellow;	// �C����
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyBlue;		// �ԑ̐�
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonBlue;	// �C����
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyGreen;	// �ԑ̗�
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonGreen;	// �C����

	// �e
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletRed;		// �e��
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletYellow;		// �e��
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletBlue;		// �e��
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletGreen;		// �e��

	// ��
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallW;		// ���ɒ�����
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallH;		// �c�ɒ�����
	
	// �X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X
	std::unique_ptr<CStaticMeshObject>			m_pStcMeshObj;

	// �v���C���[�}�l�[�W���[
	std::shared_ptr<CPlayerManager>				m_pPlayerManager;

	// �e�N���X�}�l�[�W���[
	std::shared_ptr<CShotManager>				m_pShotManager;

	// �n�ʃN���X.
	std::unique_ptr<CGround>					m_pGround;

	//�^�C�}�[�N���X.
	std::shared_ptr<CTimer>						m_Timer;

	// �ǃN���X
	std::shared_ptr<CWall>		m_pWallTop;		// ���
	std::shared_ptr<CWall>		m_pWallBottom;	// ����
	std::shared_ptr<CWall>		m_pWallLeft;	// ����
	std::shared_ptr<CWall>		m_pWallRight;	// �E��
	//�A�C�e���{�b�N�X�}�l�[�W���[�N���X.
	std::shared_ptr<CItemBoxManager>			m_pItemBoxManager;

	// �V�[���񋓕ϐ�.
	CSceneType		m_SceneType;

public:		//�ϐ��p.
	// �ȈՎ��Ԃ�~�߂�ϐ�.
	int		m_StopTimeCount;
	//Icon�̉�]�p.
	float	m_Rot;

	//���v�̐j.
	float	time;

	// ���� ��Ԃ̉����߂�
	D3DXVECTOR3 push;
};