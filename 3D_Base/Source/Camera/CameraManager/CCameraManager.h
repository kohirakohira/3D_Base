#pragma once
//-----���C�u����-----
#include <iostream>
#include <array>	

//-----�q�N���X-----
#include "Camera\\CCamera.h"

//�J�����}�l�[�W���[�N���X.
class CCameraManager
{
public:
	CCameraManager();
	~CCameraManager();

	//�����֐�.
	void Update();
	//�`��֐�.
	void Draw();
	//�������֐�.
	void Init();
	//�C���X�^���X����.
	void Create();

	//�v���W�F�N�V�����֐�.
	void Projection();

	//�O�l�̃J����
	void ThirdPersonCamera(
		CAMERA* pCamera, const D3DXVECTOR3& TargetPos, const float TargetRotY);
public:
	//�J�����֐�.
	void SetCamera();
	//�J�����̈ʒu�ݒ�.
	void SetCameraPos(const float x, const float y, const float z);
	//���C�g�̈ʒu�ݒ�.
	void SetLightPos(const float x, const float y, const float z);
	//���C�g�̉�]�ݒ�.
	void SetLightRot(const float x, const float y, const float z);
	//�^�[�Q�b�g�ʒu�Ɖ�]�̐ݒ�
	void SetTargetPos(const D3DXVECTOR3& pos);
	void SetTargetRotY(const float rotY);

public:
	//�J�����ʒu�̎擾.
	D3DXVECTOR3& GetCameraPosition();
	//�����_�ʒu�̎擾.
	D3DXVECTOR3& GetLookPosition();
	//�r���[�s��̎擾
	D3DXMATRIX GetViewMatrix() const;
	//�J�����̃C���f�b�N�X�ԍ����擾����.
	std::shared_ptr<CCamera> GetCamera(int idx) const
	{
		//�C���f�b�N�X�ԍ����͈͊O(0��菭�Ȃ��A����4�ȏ�)�Ȃ�nullptr��Ԃ�.
		if (idx < 0 || idx >= PLAYER_MAX)
		{
			return nullptr;
		}
		return m_Cameras[idx];
	}

public:
	//�J�����N���X�ϐ�(�v���C���[�̐���)���f�t�H���g�������F����{}.
	std::array<std::shared_ptr<CCamera>, PLAYER_MAX>		m_Cameras{};

private:

};