#include "CCollisionManager.h"
//-----ƒ‰ƒCƒuƒ‰ƒŠ-----
#include <iostream>

void CCollisionManager::CheckAllCollisions()
{
	for (size_t i = 0; i < m_Colliders.size(); ++i)
	{
		for (size_t j = i + 1; j < m_Colliders.size(); ++j)
		{
			if (m_Colliders[i]->CheckCollision(m_Colliders[j]))
			{
				std::cout << "Collision detected between collider " << i << " and collider " << j << std::endl;
			}
		}
	}
}
