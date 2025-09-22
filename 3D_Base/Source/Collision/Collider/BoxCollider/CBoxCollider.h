#pragma once
//-----�p������N���X-----
#include "Collision//Collider//CCollider.h" // �R���C�_�[�N���X

//===================================
// �{�b�N�X�R���C�_�[�N���X (AABB<�Ƃ肠����>)
//===================================
class CBoxCollider
	: public CCollider // �R���C�_�[�N���X���p��
{
public:
	CBoxCollider(
		std::shared_ptr<CGameObject> owner,
		const D3DXVECTOR3& size);

	virtual ~CBoxCollider() override;

	// �T�C�Y�̎擾
	const D3DXVECTOR3& GetSize() const { return m_Size; }

	// �Փ˔���C���^�[�t�F�[�X�̎���
	virtual bool CheckCollision(std::shared_ptr<CCollider> other) const override;

private:
	D3DXVECTOR3 m_Size; // �{�b�N�X�̃T�C�Y (���A�����A���s��)
};