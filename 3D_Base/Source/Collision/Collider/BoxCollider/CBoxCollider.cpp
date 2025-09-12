#include "CBoxCollider.h"
//-----外部クラス-----
#include "GameObject//CGameObject.h" // ゲームオブジェクトクラス
#include "Collision//Collider//SphereCollider//CSphereCollider.h" // スフィアコライダークラス


CBoxCollider::CBoxCollider(
	std::shared_ptr<CGameObject> owner,
	const D3DXVECTOR3& size)
	: CCollider	(owner, ColliderType::Box)
	, m_Size	(size)
{
}

CBoxCollider::~CBoxCollider()
{
}

//　衝突判定
bool CBoxCollider::CheckCollision(std::shared_ptr<CCollider> other) const
{
    if (other->GetType() == ColliderType::Box)
    {
        const std::shared_ptr<CBoxCollider> box = std::dynamic_pointer_cast<CBoxCollider>(other);

        const D3DXVECTOR3& posA = m_pOwner->GetPosition();
        const D3DXVECTOR3& posB = box->GetOwner()->GetPosition();

        const D3DXVECTOR3& sizeA = m_Size;
        const D3DXVECTOR3& sizeB = box->GetSize();

        // AABB 判定
        if (fabs(posA.x - posB.x) > (sizeA.x / 2 + sizeB.x / 2)) return false;
        if (fabs(posA.y - posB.y) > (sizeA.y / 2 + sizeB.y / 2)) return false;
        if (fabs(posA.z - posB.z) > (sizeA.z / 2 + sizeB.z / 2)) return false;

        return true;
    }
    else if (other->GetType() == ColliderType::Sphere)
    {
        const std::shared_ptr<CSphereCollider> sphere = std::dynamic_pointer_cast<CSphereCollider>(other);

        const D3DXVECTOR3& boxPos = m_pOwner->GetPosition();
        const D3DXVECTOR3& spherePos = sphere->GetOwner()->GetPosition();
        float radius = sphere->GetRadius();

        // AABB と Sphere の簡易判定
        D3DXVECTOR3 closest;
        closest.x = max(boxPos.x - m_Size.x / 2, min(spherePos.x, boxPos.x + m_Size.x / 2));
        closest.y = max(boxPos.y - m_Size.y / 2, min(spherePos.y, boxPos.y + m_Size.y / 2));
        closest.z = max(boxPos.z - m_Size.z / 2, min(spherePos.z, boxPos.z + m_Size.z / 2));

        D3DXVECTOR3 diff = spherePos - closest;
        float distSq = D3DXVec3LengthSq(&diff);

        return distSq <= (radius * radius);
    }

    return false;
}
