#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" /* �p���N���X || �L�����N�^�[�N���X */

//-----���C�u����-----
#include <iostream>
#include <vector>

//-----�O���N���X-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // �v���C���[�N���X
#include "GameObject//StaticMeshObject//Shot//CShot.h" // �V���b�g�N���X
#include "gameObject//StaticMeshObject//Character//COM//CComPlayer.h" // COM�v���C���[�N���X
#include "Global.h"

//������.
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h" // ��ԃ{�f�B�N���X
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h" // ��ԃL���m���N���X

class CPlayerManager
	: public CCharacter
{
public:
	CPlayerManager();
	~CPlayerManager();

	void Initialize();
	void AttachMeshesToPlayer(int index, std::shared_ptr<CStaticMesh> body, std::shared_ptr<CStaticMesh> cannon);
	void SetPlayerPosition(int index, const D3DXVECTOR3& pos);
	//������.
	void SetPlayerRotation(int index, const D3DXVECTOR3& rad);
	void Update() override;
	void Draw (D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;
	
	D3DXVECTOR3 GetPosition();
	std::shared_ptr<CPlayer> GetControlPlayer(int index);

	void SwitchActivePlayer();

	//�v���C���[�̈ʒu�Ɖ�]���擾.�����ɂ͊e�v���C���[������
	D3DXVECTOR3 GetPosition(int index)const;
	D3DXVECTOR3 GetRotation(int index)const;

	//�ǔ��̃f�o�b�N
	//���b�N�Ώۂ̃C���f�b�N�X�w��
	void SetLockTargetIndex(int index) { m_LockTargetIndex = index; }

	//������
	//�q�I�u�W�F�N�g�ɊeBody��Cannon��ݒ肵�Ă�����֐�.
	void SetBodyAndCannon(std::shared_ptr<CBody> body, std::shared_ptr<CCannon> cannon);

	std::shared_ptr<CShotManager> m_ShotManager; //�e�}�l�[�W���[

	void SetShotManager(std::shared_ptr<CShotManager>& mgr);

private:
	//������.
	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;

    std::vector<std::shared_ptr<CPlayer>>	m_pPlayers;
	int m_ActivePlayerIndex;	// ���ݑ��쒆�̃v���C���[(�f�o�b�O�p)

	int m_LockTargetIndex = -1;	//���b�N����(�f�o�b�N�p)
};


