#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" /* �p���N���X || �X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X */

/**************************************************
*	�e�N���X(�P��)
* ShotManager���v���C���[���Ƃɕ�����������
**/
class CShot
	: public CStaticMeshObject
{
public:
	CShot();
	virtual ~CShot() override;

	void Initialize(int id);
	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//�e���Đݒ�
	void Reload(const D3DXVECTOR3& Pos, float RotY);

	void SetDisplay(bool disp) { m_Display = disp; }
	bool IsActive() const { return m_Display; }

private:
	bool		m_Display = false;		// �\���ؑ�
	D3DXVECTOR3 m_MoveDirection;		// �ړ�����
	float		m_MoveSpeed= 0.5f;		// �ړ����x
	int 		m_LifeFramesInit = 120;	// ��3�b���炢�\������
	int			m_LifeFrames = 120;		// �t���[���J�E���^
	float 		m_Gravity = 1.0f;		// �e�̏d��
	float 		m_VelocityY = 0.0f;		// �����x

#if 0
protected:
	struct Shot
	{
		bool		m_Display			= false;				// �\���ؑ�
		D3DXVECTOR3 m_MoveDirection;							// �ړ�����
		float		m_MoveSpeed			= 0.002f;				// �ړ����x�@���Ƃ肠����0.2f��ݒ�;		// �ړ����x
		int			m_DisplayTime;								// ��3�b���炢�\������
		float		m_Gravity			= 0.001f;				// �e�̏d��
		float		m_Velocity			= 0.001f;				// �����x

	};

private:
	Shot	m_Shot[ShotMax];
#endif
};
