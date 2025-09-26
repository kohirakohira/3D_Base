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

	//中心座標を取得する
	const D3DXVECTOR3& GetMinPosition() const { return m_MinPos; }
	const D3DXVECTOR3& GetMaxPosition() const { return m_MaxPos; }

private:
	D3DXVECTOR3		m_MinPos;	//最小位置.
	D3DXVECTOR3		m_MaxPos;	//最大位置.
};
