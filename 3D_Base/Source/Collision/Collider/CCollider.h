#pragma once
#include <memory>

//�����蔻��̏���������^��V����������炱���ɒǉ�.
enum class ColliderTipe
{
	Sphere,
	Box
};

//���N���X.
class CCollider
{
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
	virtual ColliderTipe GetColType() const = 0;

protected:
	D3DXVECTOR3 m_CenterPos;	//���S���W.
};