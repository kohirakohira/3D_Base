#pragma once
#include "Collision//Collider//CCollider.h"
#include "Vector3.h"

class SohereCollider;

class CBoxCollider
	: public CCollider
{
public:
	CBoxCollider();
	virtual~CBoxCollider()override;

	//���g�̌^��Box�Ȃ̂ő���̌^��CheckCollisionBox��ʂ�.
	bool CheckCollision(const CCollider& other)const override
	{
		return other.CheckCollisionBox(*this);
	}

	bool CheckCollisionSphere(const class CSphereCollider& sphere)const override;
	bool CheckCollisionBox(const class CBoxCollider& box)const override;

	//���S���W���擾����.
	const D3DXVECTOR3& GetPosition()const override { return m_CenterPos; }
	//�ŏ����W���擾����.
	D3DXVECTOR3 GetMinPosition()const { return m_MinPos; }
	//�ő���W���擾����.
	D3DXVECTOR3 GetMaxPosition()const { return m_MaxPos; }

	//�󂯎�������S���W����A�ŏ��A�ő���W��ݒ�.
	void SetPosition(const D3DXVECTOR3& pos) override;

	//�ŏ����W��ݒ肷��.
	void SetMinPosition(const D3DXVECTOR3& MinPos) { m_Min = MinPos; }
	//�ő���W��ݒ肷��.
	void SetMaxPosition(const D3DXVECTOR3& MaxPos) { m_Max = MaxPos; }

	//���g�̌^�����̌^����Ԃ�.
	ColliderTipe GetColType()const override { return ColliderTipe::Box; }

private:
	D3DXVECTOR3 m_Min;
	D3DXVECTOR3 m_Max;

	D3DXVECTOR3 m_MinPos;
	D3DXVECTOR3 m_MaxPos;
};