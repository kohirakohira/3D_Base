#pragma once
//-----ライブラリ-----
#include <d3dx9.h>
#include <vector>
#include <memory>

//===================================
//	基底コライダークラス
//===================================
class CCollider
{
public:
	//-----列挙型-----
	enum class ColliderType
	{
		Sphere,		// 球
		Box,		// 矩形
	};

public:
	CCollider();
	virtual ~CCollider() {};

	//相手のColliderを受け取る.
	virtual bool CheckCollision(const CCollider& other)const = 0;

	virtual bool CheckCollisionSphere(const class CSphereCollider& sphere)const = 0;
	virtual bool CheckCollisionBox(const class CBoxCollider& box)const = 0;

	//中心座標を取得.
	virtual const D3DXVECTOR3& GetPosition()const = 0;
	//中心座標を設定.
	virtual void SetPosition(const D3DXVECTOR3& pos) = 0;

	//自身の型が何の型かを返す.
	virtual ColliderType GetColType() const = 0;

protected:
	D3DXVECTOR3 m_CenterPos;	//中心座標.
};

