#pragma once
//-----�p������N���X-----
#include "Collision//Collider//CCollider.h" // �R���C�_�[�N���X

//===================================
// �X�t�B�A�R���C�_�[�N���X 
//===================================
class CSphereCollider
	: public CCollider // �R���C�_�[�N���X���p��
{
public:
	CSphereCollider(
		std::shared_ptr<CGameObject> owner,
		float radius);

	virtual ~CSphereCollider() override;

	// �T�C�Y�̎擾
	float GetRadius() const { return m_Radius; }

	// �Փ˔���C���^�[�t�F�[�X�̎���
	virtual bool CheckCollision(std::shared_ptr<CCollider> other) const override;

private:
	float m_Radius;
};