#pragma once
//-----���C�u����-----
#include <vector>
#include <memory>

//-----�O���N���X-----
#include "Collision//Collider//CCollider.h" // �R���C�_�[�N���X
#include "GameObject//CGameObject.h" // �Q�[���I�u�W�F�N�g�N���X

//===================================
//	�R���W�����}�l�[�W���[�N���X
//===================================
class CCollisionManager
{
public:
	void AddCollider(std::shared_ptr<CCollider> collider)
	{
		m_Colliders.push_back(collider);
	}

private:

	static CCollisionManager& Instance()
	{
		static CCollisionManager instance;
		return instance;
	}

	void Clear()
	{
		m_Colliders.clear();
	}

	void CheckAllCollisions();
private:
	CCollisionManager() {}
	~CCollisionManager() {}

private:
	std::vector<std::shared_ptr<CCollider>> m_Colliders; // �o�^����Ă���R���C�_�[
};