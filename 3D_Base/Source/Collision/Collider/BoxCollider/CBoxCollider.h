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

	//自身の型がBoxなので相手の型のCheckCollisionBoxを通る.
	bool CheckCollision(const CCollider& other)const override
	{
		return other.CheckCollisionBox(*this);
	}


	bool CheckCollisionSphere(const class CSphereCollider& sphere)const override;
	bool CheckCollisionBox(const class CBoxCollider& box)const override;
	
	bool CheckCollisionOBBtoOBB(OBB* A, OBB* B);
	// 中心座標を取得する.
	const D3DXVECTOR3& GetPosition()const override { return m_CenterPos; }
	// 受け取った中心座標から、最小、最大座標を設定.
	void SetPosition(const D3DXVECTOR3& pos) override;

	// 回転を取得する
	const D3DXVECTOR3& GetRotation()const override { return m_Rotation; }
	//	回転を設定する
	void SetRotation(const D3DXVECTOR3& rotation) override;

	// スケールを取得する
	const D3DXVECTOR3& GetScale()const override { return m_Scale; }
	// スケールを設定する
	void SetScale(const D3DXVECTOR3& scale) override;

	//最小座標を取得する.
	D3DXVECTOR3 GetMinPosition()const { return m_MinPos; }
	//最大座標を取得する.
	D3DXVECTOR3 GetMaxPosition()const { return m_MaxPos; }
	//最小座標を設定する.
	void SetMinPosition(const D3DXVECTOR3& MinPos) { m_Min = MinPos; }
	//最大座標を設定する.
	void SetMaxPosition(const D3DXVECTOR3& MaxPos) { m_Max = MaxPos; }

	//自身の型が何の型かを返す.
	ColliderType GetColType()const override { return ColliderType::Box; }

private:
	D3DXVECTOR3 m_Min;
	D3DXVECTOR3 m_Max;

	D3DXVECTOR3 m_MinPos;
	D3DXVECTOR3 m_MaxPos;
};