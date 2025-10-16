#pragma once

#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"

#include "Assets//Mesh//StaticMesh//CStaticMesh.h"

//=======================================
//	バウンディングボックスクラス
//=======================================

class CBoundingBox
{
public:
	CBoundingBox();
	~CBoundingBox();

	//モデルに合わせたバウンディングボックスを作成.
	HRESULT CreateBoxForMesh(const CStaticMesh& pMesh);

	// モデルに合わせてOBB作成
	HRESULT CreateOBBForMesh(const CStaticMesh& pMesh);

	//中心座標を取得する
	const D3DXVECTOR3& GetMinPosition() const { return m_MinPos; }
	const D3DXVECTOR3& GetMaxPosition() const { return m_MaxPos; }

	// OBBの中心と半分サイズ、ローカル軸を取得
	const D3DXVECTOR3& GetCenter() const { return m_Center; }
	const D3DXVECTOR3& GetHalfSize() const { return m_HalfSize; }
	const D3DXVECTOR3* GetAxis() const { return m_Axis; }  // X,Y,Zの3軸

private:
	D3DXVECTOR3		m_MinPos;	//最小位置.
	D3DXVECTOR3		m_MaxPos;	//最大位置.

	// OBB用
	D3DXVECTOR3		m_Center;     // 中心
	D3DXVECTOR3		m_HalfSize;   // 各軸の半分サイズ
	D3DXVECTOR3		m_Axis[3];    // ローカル軸（デフォルトはワールド軸）
};
