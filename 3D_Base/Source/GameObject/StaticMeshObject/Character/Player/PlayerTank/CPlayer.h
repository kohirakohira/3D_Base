#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // �p�� || �L�����N�^�[�N���X

//-----���C�u����-----
#include <iostream>
#include <memory>
#include <vector>

//-----�O���N���X-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h"		// ��ԁF�ԑ̃N���X
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h"	// ��ԁF�C���N���X


class CPlayer
	: public CCharacter
{
public:
	CPlayer();
	virtual ~CPlayer() override;

	virtual void Initialize(int id);

	void AttachMeshse(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon) ;
	void SetTankPosition(const D3DXVECTOR3& pos);
	void SetTankRotation(const D3DXVECTOR3& pos);

	//�v���C���[���ǂɓ����鏈�����܂Ƃ߂�.
	void SetPushBack(const D3DXVECTOR3& push);

	//Body�D��Ń��[���h���W�Ɖ�]��Ԃ�
	virtual D3DXVECTOR3 GetPosition() const;
	virtual D3DXVECTOR3 GetRotation() const;

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	// �o�E���f�B���O�I�u�W�F�N�g��ݒ�
	void SetBounding(std::shared_ptr<CStaticMesh> pBody, std::shared_ptr<CStaticMesh> pCannon);

	// �R���C�_�[�̍쐬
	void CreateCollider();

   	//�O���̃N���X������擾.
	void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
	void SetCCannon(std::shared_ptr<CCannon> pCannon) { m_pCannon = pCannon; }

	// �O���̃N���X�ɏ���n��
	std::shared_ptr<CCannon> GetCannon() const { return m_pCannon; }
	std::shared_ptr<CCannon> GetCannon() { return m_pCannon; }
	std::shared_ptr<CBody>	 GetBody()   const { return m_pBody; }

	float GetCannonYaw() const;
	D3DXVECTOR3 GetCannonPosition() const;

protected:
	std::shared_ptr<CBody> Body() const { return m_pBody; }
	std::shared_ptr<CCannon> Cannon() const { return m_pCannon; }

protected:

	std::shared_ptr<CBody>		m_pBody;
	std::shared_ptr<CCannon>	m_pCannon;
	int			m_Hp;
	int			m_PlayerID;


};
