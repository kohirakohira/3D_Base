#include "CSphereCollider.h"
//-----外部クラス-----
#include "GameObject//CGameObject.h" // ゲームオブジェクトクラス
#include "Collision//Collider//SphereCollider//CSphereCollider.h" // スフィアコライダークラス

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
	//	// BoxCollider との衝突判定は CBoxCollider 側で処理
	//	return other->CheckCollision(shared_from_this());
	//	// 必ずスマートポインタで生成すること※shared_from_this()
	//}

	return false;
}
