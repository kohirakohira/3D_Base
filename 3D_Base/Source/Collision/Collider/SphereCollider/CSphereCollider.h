#pragma once
//-----継承するクラス-----
#include "Collision//Collider//CCollider.h" // コライダークラス

//===================================
// スフィアコライダークラス 
//===================================
class CSphereCollider
	: public CCollider // コライダークラスを継承
{
public:
	CSphereCollider(
		std::shared_ptr<CGameObject> owner,
		float radius);

	virtual ~CSphereCollider() override;

	// サイズの取得
	float GetRadius() const { return m_Radius; }

	// 衝突判定インターフェースの実装
	virtual bool CheckCollision(std::shared_ptr<CCollider> other) const override;

private:
	float m_Radius;
};