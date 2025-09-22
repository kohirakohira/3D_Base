#pragma once
//-----���C�u����-----
#include <d3dx9.h>
#include <vector>
#include <memory>

//-----�O���錾-----
class CGameObject; // �Q�[���I�u�W�F�N�g�N���X

//===================================
//	���R���C�_�[�N���X
//===================================
class CCollider
	: public std::enable_shared_from_this<CCollider> // shared_from_this()���g�����߂Ɍp��
{
public:
	//-----�񋓌^-----
	enum class ColliderType
	{
		Sphere,		// ��
		Box,		// ��`
	};

public:
	CCollider(std::shared_ptr<CGameObject> owner, ColliderType type);
	virtual ~CCollider() {}

	// �^�C�v�̎擾
	ColliderType GetType() const { return m_Type; }

	// �I�[�i�[�̎擾
	std::shared_ptr<CGameObject> GetOwner() const { return m_pOwner; }

	// �Փ˔���C���^�[�t�F�[�X
	virtual bool CheckCollision(std::shared_ptr<CCollider> other) const = 0;

protected:
	std::shared_ptr<CGameObject> m_pOwner; // ���̃R���C�_�[�����I�u�W�F�N�g
	ColliderType m_Type; // �R���C�_�[�̃^�C�v
};

