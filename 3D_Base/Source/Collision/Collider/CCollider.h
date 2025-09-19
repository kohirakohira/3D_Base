#pragma once
//-----���C�u����-----
#include <d3dx9.h>
#include <vector>
#include <memory>

//===================================
//	���R���C�_�[�N���X
//===================================
class CCollider
{
public:
	//-----�񋓌^-----
	enum class ColliderType
	{
		Sphere,		// ��
		Box,		// ��`
	};

public:
	CCollider();
	virtual ~CCollider() {};

	//�����Collider���󂯎��.
	virtual bool CheckCollision(const CCollider& other)const = 0;

	virtual bool CheckCollisionSphere(const class CSphereCollider& sphere)const = 0;
	virtual bool CheckCollisionBox(const class CBoxCollider& box)const = 0;

	//���S���W���擾.
	virtual const D3DXVECTOR3& GetPosition()const = 0;
	//���S���W��ݒ�.
	virtual void SetPosition(const D3DXVECTOR3& pos) = 0;

	//���g�̌^�����̌^����Ԃ�.
	virtual ColliderType GetColType() const = 0;

protected:
	D3DXVECTOR3 m_CenterPos;	//���S���W.
};

