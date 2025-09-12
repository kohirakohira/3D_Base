#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Collision//Collider//CCollider.h" // コライダークラス
#include "GameObject//CGameObject.h" // ゲームオブジェクトクラス

//===================================
//	コリジョンマネージャークラス
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
	std::vector<std::shared_ptr<CCollider>> m_Colliders; // 登録されているコライダー
};