#include "CCollider.h"

CCollider::CCollider(
	std::shared_ptr<CGameObject> owner,
	ColliderType type)
	: m_pOwner(owner)
	, m_Type(type)
{
}
