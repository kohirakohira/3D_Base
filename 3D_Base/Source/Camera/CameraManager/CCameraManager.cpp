#include "CCameraManager.h"

CCameraManager::CCameraManager()
	: m_Cameras				()
{
}

CCameraManager::~CCameraManager()
{
}

//�����֐�.
void CCameraManager::Update()
{
	//�J�����̐������X�V.
	for (auto cam : m_Cameras)
	{
		cam->Update();
	}
}
//�`��֐�.
void CCameraManager::Draw()
{
	//�J�����̐������`��.
	for (auto cam : m_Cameras)
	{
		cam->Draw();
	}
}

//�������֐�.
void CCameraManager::Init()
{
	//�J�����̐�����������.
	for (auto cam : m_Cameras)
	{
		cam->Init();
	}
}

//�C���X�^���X����.
void CCameraManager::Create()
{
	//�J�����̐�����������m_Cameras.size()���̃T�C�Y.
	for (size_t player = 0; player < m_Cameras.size(); player++)
	{
		//�J�����N���X�𐶐�.
		m_Cameras[player] = std::make_shared<CCamera>();
	}
}

//�J�����֐�.
void CCameraManager::SetCamera()
{
	//�J�����̐������ݒ�.
	for (auto cam : m_Cameras)
	{
		cam->SetCamera();
	}
}

//�J�����̈ʒu�ݒ�.
void CCameraManager::SetCameraPos(const float x, const float y, const float z)
{
	//�J�����̐������ʒu�ݒ�.
	for (auto cam : m_Cameras)
	{
		cam->SetCameraPos(x, y, z);
	}
}

//���C�g�̈ʒu�ݒ�.
void CCameraManager::SetLightPos(const float x, const float y, const float z)
{
	//�J�����̐��������C�g�̈ʒu�ݒ�.
	for (auto cam : m_Cameras)
	{
		cam->SetLightPos(x, y, z);
	}
}

//���C�g�̉�]�ݒ�.
void CCameraManager::SetLightRot(const float x, const float y, const float z)
{
	//�J�����̐��������C�g�̉�]�ݒ�.
	for (auto cam : m_Cameras)
	{
		cam->SetLightRot(x, y, z);
	}
}

//�v���W�F�N�V�����֐�.
void CCameraManager::Projection()
{
	//�J�����̐������v���W�F�N�V�����ݒ�.
	for (auto cam : m_Cameras)
	{
		cam->Projection();
	}
}

//�O�l�̃J����
void CCameraManager::ThirdPersonCamera(
	CAMERA* pCamera, const D3DXVECTOR3& TargetPos, const float TargetRotY)
{
	//�J�����̐������O�l�̃J�����ݒ�.
	for (auto cam : m_Cameras)
	{
		cam->ThirdPersonCamera(pCamera, TargetPos, TargetRotY);
	}
}


//�^�[�Q�b�g�ʒu�Ɖ�]�̐ݒ�
void CCameraManager::SetTargetPos(const D3DXVECTOR3& pos)
{
	//�J�����̐������^�[�Q�b�g�ʒu�ݒ�.
	for (auto cam : m_Cameras)
	{
		cam->SetTargetPos(pos);
	}
}

void CCameraManager::SetTargetRotY(const float rotY)
{
	for (auto cam : m_Cameras)
	{
		cam->SetTargetRotY(rotY);
	}
}

