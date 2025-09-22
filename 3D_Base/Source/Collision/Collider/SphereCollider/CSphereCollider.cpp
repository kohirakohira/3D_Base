#include "CSphereCollider.h"
//-----�O���N���X-----
#include "GameObject//CGameObject.h" // �Q�[���I�u�W�F�N�g�N���X
#include "Collision//Collider//SphereCollider//CSphereCollider.h" // �X�t�B�A�R���C�_�[�N���X

CSphereCollider::CSphereCollider(
	std::shared_ptr<CGameObject> owner,
	float radius)
	: CCollider(owner, ColliderType::Sphere)
	, m_Radius(radius)
{
}

CSphereCollider::~CSphereCollider()
{
}

bool CSphereCollider::CheckCollision(std::shared_ptr<CCollider> other) const
{
	if (other->GetType() == ColliderType::Sphere)
	{
		const std::shared_ptr<CSphereCollider> Sphere = std::dynamic_pointer_cast<CSphereCollider>(other);

		const D3DXVECTOR3& posA = m_pOwner->GetPosition();
		const D3DXVECTOR3& posB = Sphere->GetOwner()->GetPosition();

		D3DXVECTOR3 diff = posA - posB;
		float distSq = D3DXVec3LengthSq(&diff);

		float r = m_Radius + Sphere->GetRadius();
		return distSq <= (r * r);
	}
	//else if (other->GetType() == ColliderType::Box)
	//{
	//	// BoxCollider �Ƃ̏Փ˔���� CBoxCollider ���ŏ���
	//	return other->CheckCollision(shared_from_this());
	//	// �K���X�}�[�g�|�C���^�Ő������邱�Ɓ�shared_from_this()
	//}

	return false;
}
