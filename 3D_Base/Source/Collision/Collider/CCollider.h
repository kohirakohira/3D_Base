#pragma once
#include <memory>

//当たり判定の処理をする型を新しく作ったらここに追加.
enum class ColliderType
{
	Sphere,
	Box,
};

struct CollisionResultOBB
{
	bool Hit = false;
	float Penetration = FLT_MAX;	// 衝突深度
	D3DXVECTOR3 Normal;				// 押し出し方向(A→B)
};

//基底クラス.
class CCollider
{
public:
	CCollider();
	virtual ~CCollider();

	// 行列更新
	virtual void UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale) = 0;

	// 相手のColliderを受け取る.
	virtual bool CheckCollision(const CCollider& other)const = 0;

	virtual bool CheckCollisionSphere(const class CSphereCollider& sphere)const = 0;
	virtual bool CheckCollisionBox(const class CBoxCollider& box)const = 0;
	
	virtual CollisionResultOBB CheckCollisionBoxDetail(const CBoxCollider& box) const = 0;

	// 中心座標を取得.
	virtual const D3DXVECTOR3& GetPosition()const = 0;
	// 中心座標を設定.
	virtual void SetPosition(const D3DXVECTOR3& pos) = 0;

	// 半径を設定
	virtual void SetRadius(float radius) = 0;

	//自身の型が何の型かを返す.
	virtual ColliderType GetColType() const = 0;

protected:
	D3DXVECTOR3 m_CenterPos;	// 中心座標
	D3DXVECTOR3	m_Rotation;		// 回転
	D3DXVECTOR3	m_Scale;		// スケール
};