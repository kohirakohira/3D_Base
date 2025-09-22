#pragma once
//-----���C�u����-----
#include <vector>
#include <memory>

//-----�O���N���X-----
#include "Collision/Collider/CCollider.h" // �R���C�_�[�N���X
#include "Collision//Collider//SphereCollider//CSphereCollider.h" // �X�t�B�A�R���C�_�[�N���X
#include "Collision//Collider//BoxCollider//CBoxCollider.h" // �{�b�N�X�R���C�_�[�N���X

//===================================
//	�Q�[���I�u�W�F�N�g�N���X.
//===================================
class CGameObject
	: public std::enable_shared_from_this<CGameObject> // shared_from_this()���g�����߂Ɍp��
{
public:
	// �񋓌^
	enum class ObjectType
	{
		// �����蔻��̃I�u�W�F�N�g���o�p
		TankBody,	// ��ԁF�ԑ�
		TankCannon, // ��ԁF�C��
		Shot,		// �e
		ItemBox,	// �A�C�e���{�b�N�X

		None
	};

public:
	CGameObject();
	//�p�����s���ꍇ�́A�f�X�g���N�^��virtual��t���ĉ��z�֐��ɂ��邱��.
	virtual ~CGameObject();

	//virtual �^ �֐��� = 0; �������z�֐�.
	//�q�N���X�ɏ��������C������̂ŁA�����ł͖��O�����錾���Ē�`�͏����Ȃ�.
	virtual void Update() = 0;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) = 0;

	// --- Collider �Ǘ� ---
	// �X�t�B�A�R���C�_�[�ǉ�
	std::shared_ptr<CSphereCollider> AddSphereCollider(float radius)
	{
		// shared_ptr�ɕϊ����ĊǗ�
		auto collider = std::make_shared<CSphereCollider>(shared_from_this(), radius);

		// CCollider �� vector �� shared_ptr ���i�[����ꍇ�͌^�����킹��
		m_Colliders.push_back(collider);

		return collider;
	}
	// �{�b�N�X�R���C�_�[�ǉ�
	std::shared_ptr<CBoxCollider> AddBoxCollider(const D3DXVECTOR3& size)
	{
		// shared_ptr�ɕϊ����ĊǗ�
		auto collider = std::make_shared<CBoxCollider>(shared_from_this(), size);
		// CCollider �� vector �� shared_ptr ���i�[����ꍇ�͌^�����킹��
		m_Colliders.push_back(collider);
		return collider;
	}

	const std::vector<std::shared_ptr<CCollider>>& GetColliders() const { return m_Colliders; }

	virtual void OnCollision(std::shared_ptr<CCollider> other) {}
	ObjectType GetType() const { return m_Type; }
	void SetType(ObjectType type) { m_Type = type; }

	//���W�ݒ�֐�.
	void SetPosition(float x, float y, float z) {
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};
	void SetPosition(const D3DXVECTOR3& pos) {
		m_vPosition = pos;
	}
	//���W�擾�֐�.
	const D3DXVECTOR3& GetPosition() const {
		return m_vPosition;
	}

	//��]�ݒ�֐�.
	void SetRotation(float x, float y, float z) {
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;
	};
	void SetRotation(const D3DXVECTOR3& rot) {
		m_vRotation = rot;
	}
	//��]�擾�֐�.
	const D3DXVECTOR3& GetRotation() const {
		return m_vRotation;
	}

	//�g�k�ݒ�֐�.
	void SetScale(float x, float y, float z) {
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
	}
	void SetScale(float xyz) {
		m_vScale = D3DXVECTOR3(xyz, xyz, xyz);
	}
	//�g�k�擾�֐�.
	const D3DXVECTOR3& GetScale() const {
		return m_vScale;
	}

protected://protected�͎q�N���X�̂݃A�N�Z�X�\.
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR3	m_vRotation;
	D3DXVECTOR3	m_vScale;

	std::vector<std::shared_ptr<CCollider>> m_Colliders; // �����R���C�_�[�ێ�
	ObjectType m_Type = ObjectType::None;				 // �I�u�W�F�N�g�^�C�v	
};