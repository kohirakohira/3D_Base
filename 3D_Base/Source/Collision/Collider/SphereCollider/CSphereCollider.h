#pragma once
#include "Collision//Collider//CCollider.h"

class CSphereCollider
	: public CCollider
{
public: // 構造体
	struct SPHERE
	{
		D3DXVECTOR3 CenterPos;
		float Radius;
	} m_Sphere;

public:
	CSphereCollider();
	virtual~CSphereCollider() override;

	// 行列変換を更新
	void UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale) override;

	//自身の型がSphereなので相手の型のCheckCollisionSphereを通る.
	bool CheckCollision(const CCollider& other)const override
	{
		return other.CheckCollisionSphere(*this);
	}

	bool CheckCollisionSphere(const class CSphereCollider& sphere)const override;
	bool CheckCollisionBox(const class CBoxCollider& box)const override;

	// 衝突深度付き判定
	CollisionResultOBB CheckCollisionBoxDetail(const CBoxCollider& box) const override;


	//中心座標を取得する
	const D3DXVECTOR3& GetPosition() const override { return m_CenterPos; }
	//半径(長さ)を取得する
	float GetRadius() const { return m_Radius; }

	//中心座標を設定する
	void SetPosition(const D3DXVECTOR3& Pos) override { m_CenterPos = Pos; }
	
	//半径(長さ)を設定する
	void SetRadius(float radius) override;

	//自身の型が何の型かを返す.
	ColliderType GetColType()const override { return ColliderType::Sphere; }

private:
	float	m_Radius;
};