//-----�p������N���X-----
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//CPlayer.h" // �v���C���[�N���X

//-----���C�u����-----
#include <d3dx9math.h>

//-----�O���N���X-----
#include "GameObject/StaticMeshObject/Shot/ShotManager/CShotManager.h"	//�V���b�g�}�l�[�W���[
#include "GameObject/StaticMeshObject/Character/CCharacter.h"			//�L�����N�^�[�N���X.���C�p

class CComPlayer 
	: public CPlayer
{
public:

	enum class ComPattern
	{
		Idle,			//��~
		Priority = 0,	//�D��x
		Aim,			//�_��
		ItemSearch,		//�A�C�e���T��
		PlayerSearch,	//�v���C���[�T��.COM���܂߂�

	};

	//����ł���Ώۂ̃^�C�v
	enum class ActiveType
	{
		player,
		com
	};

	CComPlayer();
	~CComPlayer() override;

	void Initialize(int id)override;
	void Update() override;

	//�ǔ��Ώۂ̐ݒ�
	void SetTarget(std::shared_ptr<CPlayer> player);
	void ClearTarget();

	//�`���[�j���O�l,m_��t���Y�ꂽ�̂ł��ƂŏC������
	float MoveSpeed;			// 1�t���[���̑O�i��
	float TurnStep;				// 1�t���[���̉񓪗�
	float AimTurnStep;			// �C���񓪂�1�t���[����
	float KeepDistance;			// ���̋�����ۂ�
	float CannonHeight;			// �C���̍����I�t�Z�b
	float m_AvoidRadius;		// �ق�COM���痣��锼�a
	float m_AvoidWeight;		// �����x�N�g���̏d��(0�Ŗ���.1����)

	D3DXVECTOR3 GetPosition() const override;
	D3DXVECTOR3 GetRotation() const override;

	void AttachShotManager(std::shared_ptr<CShotManager>& mgr) { m_pShotManager = mgr; }

	void SetActiveType(ActiveType ActiveType) { m_ActiveType = ActiveType; }
	ActiveType GetActiveType() const { return m_ActiveType; }

private:
	ComPattern m_ComPattern;			//COM�̍s���p�^�[��
	ActiveType m_ActiveType;			//����ł���Ώ�
	int m_Priority = 0;					//�D��x
	std::shared_ptr<CPlayer> m_Target;	//�ǔ��Ώ�
	bool m_Registered;					//�C���X�^���X�o�^�Ǘ�

	//�������˗p�̃p�����[�^
	std::weak_ptr<CShotManager> m_pShotManager; //�e�}�l�[�W���[
	int m_ShotCD;				//�N�[���_�E��
	int	ShotCooldownFrames;		//�N�[���_�E������(�t���[��)
	float FireAngleEpsDeg;		//���̊p�x�ȓ��Ȃ甭��
	float MuzzleOffsetZ;		//�C���̃I�t�Z�b�g

private:
	//��������
	void SanitizeParams();
	void TickChaseTo(const D3DXVECTOR3& targetPos);
	void TickAimTo(const D3DXVECTOR3& targetPos);

	// �w���p
	static float Wrap(float rad);                         // [-��,��]�ɐ��K��
	static float Approach(float cur, float goal, float step);
	static D3DXVECTOR3 ForwardFromYaw(float yaw);         // (sin(yaw),0,cos(yaw))

	//����COM���d�Ȃ����肷��̂�h���v�Z
	void ComputeSeparation(const D3DXVECTOR3& selfPos,
		D3DXVECTOR3& outSep, float& outNearest) const;

	//COM�C���X�^���X�̐ÓI���W�X�g��
	static std::vector<CComPlayer*>& Instances();

	static inline float ToRad(float d) { return d * (D3DX_PI / 180.0f); }

	void ComputeMuzzle(D3DXVECTOR3& outpos, float& outYaw) const;

};