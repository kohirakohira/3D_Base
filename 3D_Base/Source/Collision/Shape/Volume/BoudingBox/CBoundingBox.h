#pragma once

#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"
#include "Assets//Mesh//StaticMesh//CStaticMesh.h"
#include <d3dx9.h>

//=======================================
//	バウンディングボックス (OBB対応)
//=======================================
class CBoundingBox
{
public:
	CBoundingBox();
	~CBoundingBox();

	// モデルに合わせたOBBを作成
	HRESULT CreateBoxForMesh(const CStaticMesh& mesh);

	// ほかの矩形との当たり判定
	bool IsHitBox(const CBoundingBox pBBox);

	// 情報取得
	//中心座標を設定する
	void SetPosition(const D3DXVECTOR3& Pos) { m_Position = Pos; }
	//回転情報を設定する
	void SetRotation(const D3DXVECTOR3& Rot);
	const D3DXVECTOR3& GetHalfSize() const { return m_HalfSize; }
	const D3DXVECTOR3* GetAxis() const { return m_Axis; }

	// AABB互換的にMin/Maxも保持（ワールド行列無変換時の値）
	const D3DXVECTOR3& GetMinPosition() const { return m_MinPos; }
	const D3DXVECTOR3& GetMaxPosition() const { return m_MaxPos; }


private:
	D3DXVECTOR3		m_MinPos;		// 最小位置（ローカル空間）
	D3DXVECTOR3		m_MaxPos;		// 最大位置（ローカル空間）

	D3DXVECTOR3		m_Position;		// 中心座標
	D3DXVECTOR3		m_Rotation;		// 回転情報
	D3DXVECTOR3		m_HalfSize;		// 半分サイズ（ローカル空間）
	D3DXVECTOR3		m_Axis[3];		// ローカル軸（X,Y,Zワールド空間）
};
