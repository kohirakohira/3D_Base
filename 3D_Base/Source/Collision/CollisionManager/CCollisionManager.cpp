#include "CCollisionManager.h"
//-----ライブラリ-----
#include <iostream>

void CCollisionManager::CheckAllCollisions()
{
    for (size_t i = 0; i < m_Colliders.size(); ++i)
    {
        for (size_t j = i + 1; j < m_Colliders.size(); ++j)
        {
            auto ownerA = m_Colliders[i]->GetOwner();
            auto ownerB = m_Colliders[j]->GetOwner();

            // 同じオブジェクト内部の判定はスキップ（戦車 Body/Cannon の内部など）
            if (ownerA == ownerB &&
                (ownerA->GetType() == CGameObject::ObjectType::TankBody ||
                 ownerA->GetType() == CGameObject::ObjectType::TankCannon))
            {
                continue;
            }

            if (m_Colliders[i]->CheckCollision(m_Colliders[j]))
            {
                ownerA->OnCollision(m_Colliders[j]);
                ownerB->OnCollision(m_Colliders[i]);
            }
        }
    }
}
