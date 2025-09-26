#pragma once
#include "Collision//Collider//CCollider.h"

class CSphereCollider
	: public CCollider
{
public:
	CSphereCollider();
	virtual~CSphereCollider() override;

	//���g�̌^��Sphere�Ȃ̂ő���̌^��CheckCollisionSphere��ʂ�.
	bool CheckCollision(const CCollider& other)const override
	{
		return other.CheckCollisionSphere(*this);
	}

	bool CheckCollisionSphere(const class CSphereCollider& sphere)const override;
	bool CheckCollisionBox(const class CBoxCollider& box)const override;

	//���S���W���擾����
	const D3DXVECTOR3& GetPosition() const override { return m_CenterPos; }
	//���a(����)���擾����
	float GetRadius() const { return m_Radius; }

	//���S���W��ݒ肷��
	void SetPosition(const D3DXVECTOR3& Pos) override { m_CenterPos = Pos; }
	//���a(����)��ݒ肷��
	void SetRadius(float Radius) { m_Radius = Radius; }

	//���g�̌^�����̌^����Ԃ�.
	ColliderTipe GetColType()const override { return ColliderTipe::Sphere; }

private:
	float		m_Radius;
};