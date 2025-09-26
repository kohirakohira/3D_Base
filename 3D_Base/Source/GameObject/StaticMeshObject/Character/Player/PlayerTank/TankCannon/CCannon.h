#pragma once
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // �p�� || �L�����N�^�[�N���X

//-----�O���N���X-----
#include "Camera//CCamera.h" //�J�����N���X
#include "InputDevice//Input//CInputManager.h" // ���͎�t�N���X

/**************************************************
*	�C���N���X.
**/
class CCannon
	: public CCharacter	//�L�����N�^�[�N���X���p��.
{
public:
	CCannon(int inputID);
	virtual ~CCannon() override;

	virtual void Initialize(int id);

	virtual void Update() override;
	virtual void Draw(
		D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	void SetCannonPosition(const D3DXVECTOR3& Pos);

	D3DXVECTOR3 GetCannonPosition() const { return m_vPosition; }

	// ���̓N���X��ݒ�
	void SetInputManager(const std::shared_ptr<CInputManager>& input);

	//�v���C���[���ǂɓ�����Ɩ߂�.
	void PushBack(const D3DXVECTOR3& push);

private:
	// �L�[���͎�t
	void KeyInput();

protected:
	float		m_TurnSpeed;	// ��]���x

	int			m_ShotCoolTime; // �V���b�g�̃N�[���^�C��
	const int	m_ShotInterval; // �V���b�g�̃C���^�[�o��
	CCamera*	m_pCamera;	

private:
	std::shared_ptr<CInputManager> m_pInput;
};