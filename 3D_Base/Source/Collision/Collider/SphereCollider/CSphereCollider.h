#pragma once
//-----継承するクラス-----
#include "Collision//Collider//CCollider.h" // コライダークラス

//-----ライブラリ-----
#include <algorithm>

//-----外部クラス-----
#include "Collision//Collider//BoxCollider//CBoxCollider.h" // ボックスコライダークラス


//===================================
// スフィアコライダークラス 
//===================================
class CSphereCollider
	: public CCollider // コライダークラスを継承
{
public:
	CSphereCollider();
	virtual~CSphereCollider() override;

	//自身の型がSphereなので相手の型のCheckCollisionSphereを通る.
	bool CheckCollision(const CCollider& other)const override
	{
		return other.CheckCollisionSphere(*this);
	}

	bool CheckCollisionSphere(const class CSphereCollider& sphere)const override;
	bool CheckCollisionBox(const class CBoxCollider& box)const override;

	//中心座標を取得する
	const D3DXVECTOR3& GetPosition() const override { return m_CenterPos; }
	//半径(長さ)を取得する
	float GetRadius() const { return m_Radius; }

	//中心座標を設定する
	void SetPosition(const D3DXVECTOR3& Pos) override { m_CenterPos = Pos; }
	//半径(長さ)を設定する
	void SetRadius(float Radius) { m_Radius = Radius; }

	//自身の型が何の型かを返す.
	ColliderType GetColType()const override { return ColliderType::Sphere; }

private:
	float		m_Radius;
};