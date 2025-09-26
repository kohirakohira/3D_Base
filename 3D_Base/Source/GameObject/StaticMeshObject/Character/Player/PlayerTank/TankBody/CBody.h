#pragma once
//-----�p������N���X-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // �L�����N�^�[�N���X

//-----�O���N���X-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h" // ��ԁF�C���N���X
#include "InputDevice//Input//CInputManager.h" // ���͎�t�N���X

/**************************************************
*	�ԑ̃N���X.
**/

// �O���錾
class CInputManager;

class CBody
	: public CCharacter	//�L�����N�^�[�N���X���p��.
{
public:
	// �ړ����
	enum enMoveState
	{
		Stop,		// ��~
		Forward,	// �O�i
		Backward,	// ���
		TurnLeft,	// ����]
		TurnRight,	// �E��]
	};
public:
	CBody(int inputID);
	virtual ~CBody() override;

	virtual void Initialize(int id);

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	// ���W�R������
	void RadioControl();

	// ���̓N���X��ݒ�
	void SetInputManager(const std::shared_ptr<CInputManager>& input);

	//�v���C���[���ǂɓ�����Ɩ߂�.
	void PushBack(const D3DXVECTOR3& push);

	// �o�E���f�B���O�{�b�N�X���쐬
	void CreateBounding(std::shared_ptr<CStaticMesh> pBody);


private:
	// �L�[���͎�t.
	void KeyInput();

protected:
	float		m_TurnSpeed;	// ��]���x
	float		m_MoveSpeed;	// �ړ����x
	enMoveState m_MoveState;	// �ړ����

private:
	std::shared_ptr<CInputManager> m_pInput;

	std::shared_ptr<CCollider>			m_pCollider;
};
