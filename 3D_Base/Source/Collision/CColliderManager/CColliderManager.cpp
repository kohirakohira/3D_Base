#include "CColliderManager.h"

CColliderManager::CColliderManager()
{
}

CColliderManager::~CColliderManager()
{
}

//CCollider�ŐV���������蔻��̂�����̂��쐬�����Ƃ��Ɏg�p����.
void CColliderManager::AddCollider(std::unique_ptr<CCollider> collider)
{
	//
	m_pCollider.push_back(std::move(collider));
}

//�����蔻������ׂčs��.
void CColliderManager::CheckAllCollisions() const
{
}


//void CCollisionManager::CheckAllCollisions()
//{
//    for (size_t i = 0; i < m_Colliders.size(); ++i)
//    {
//        for (size_t j = i + 1; j < m_Colliders.size(); ++j)
//        {
//            auto ownerA = m_Colliders[i]->GetOwner();
//            auto ownerB = m_Colliders[j]->GetOwner();
//
//            // �����I�u�W�F�N�g�����̔���̓X�L�b�v�i��� Body/Cannon �̓����Ȃǁj
//            if (ownerA == ownerB &&
//                (ownerA->GetType() == CGameObject::ObjectType::TankBody ||
//                 ownerA->GetType() == CGameObject::ObjectType::TankCannon))
//            {
//                continue;
//            }
//
//            if (m_Colliders[i]->CheckCollision(m_Colliders[j]))
//            {
//                ownerA->OnCollision(ownerB);
//                ownerB->OnCollision(ownerA);
//            }
//        }
//    }
//}
