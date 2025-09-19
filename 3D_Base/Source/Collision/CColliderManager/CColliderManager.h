#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>
#include <iostream>

//-----外部クラス-----
#include "Collision//Collider//CCollider.h" // コライダークラス

class CColliderManager
{
public:
	CColliderManager();
	~CColliderManager();

	//CColliderで新しく当たり判定のあるものを作成したときに使用する.
	void AddCollider(std::unique_ptr<CCollider> collider);

	//当たり判定をすべて行う.
	void CheckAllCollisions()const;

private:
	//ユニークポインタCColider型をVector(可変長配列)にしてる.
	std::vector< std::unique_ptr<CCollider> > m_pCollider;
};