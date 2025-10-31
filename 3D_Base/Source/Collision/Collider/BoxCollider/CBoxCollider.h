#pragma once
#include "Collision//Collider//CCollider.h"
#include "Vector3.h"

class SohereCollider;

class CBoxCollider
	: public CCollider
{
public: // 構造体
	struct OBB
	{
		D3DXVECTOR3 CenterPos;		// 中心点の座標
		D3DXVECTOR3 LocalAxes[3];	// XYZの各座標軸の傾きを表す方向ベクトル
		D3DXVECTOR3 HarfLength;		// OBBの各座標軸に沿った長さの半分(中心点から面の長さ)
	} m_OBB; 

public:
	CBoxCollider();
	virtual~CBoxCollider()override;

	// 行列変換を更新
	void UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale) override;

	//自身の型がBoxなので相手の型のCheckCollisionBoxを通る.
	bool CheckCollision(const CCollider& other)const override
	{
		return other.CheckCollisionBox(*this);
	}

	bool CheckCollisionSphere(const class CSphereCollider& sphere)const override;
	bool CheckCollisionBox(const class CBoxCollider& box)const override;

	// 中心座標を取得する.
	const D3DXVECTOR3& GetPosition()const override { return m_CenterPos; }
	// 受け取った中心座標から、最小、最大座標を設定.
	void SetPosition(const D3DXVECTOR3& pos) override;

	// 初期サイズの設定
	void SetBaseHalfExtents(const D3DXVECTOR3& min_local, const D3DXVECTOR3& max_local);

	//自身の型が何の型かを返す.
	ColliderType GetColType()const override { return ColliderType::Box; }

private:
	// OBB同士の判定
	static bool CheckCollisionOBBtoOBB(const OBB* A, const OBB* B);
	static bool CheckCollisionOBBtoSphere(const CSphereCollider& sphere, const OBB* box, D3DXVECTOR3* tempP); // tempP:仮計算した最接近点

	// OBBと点の最接近点の算出
	static void ClosestPointOBB(const D3DXVECTOR3* centerPoint, const OBB* box, D3DXVECTOR3* closePoint);

private:
	D3DXVECTOR3 m_BaseHalfExtents; // 半分のサイズ
};