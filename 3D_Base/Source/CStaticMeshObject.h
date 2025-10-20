#pragma once
#include <iostream>

#include "CGameObject.h"
#include "CStaticMesh.h"
#include "CBoundingSphere.h"
#include "CRay.h"

/************************************************************
*	スタティックメッシュオブジェクトクラス.
**/
class CStaticMeshObject
	: public CGameObject
{
public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera ) override;

	//メッシュを接続する.
	void AttachMesh(std::shared_ptr<CStaticMesh> pMesh ){
		m_pMesh = pMesh;
	}
	//メッシュを切り離す.
	void DetachMesh(){
		m_pMesh = nullptr;
	}
	//バウンディングスフィア取得
	std::shared_ptr<CBoundingSphere> GetBSphere() const {
		return m_pBSphere;
	}
	//モデルに合わせたバウンディングスフィア作成のラッパー関数
	HRESULT CreateBSphereForMesh(const CStaticMesh& pMesh) {
		return m_pBSphere->CreateSphereForMesh(pMesh);
	}
	//バウンディングスフィアをオブジェクト位置に合わせる
	//※モデルの原点が中心の場合を想定
	void UpdateBSpherePos() {
		m_pBSphere->SetPosition(m_vPosition);
	}

	//レイとメッシュの当たり判定
	bool IsHitForRay(
		const RAY& pRay,			//レイ構造体
		float* pDistance,			//(out)距離
		D3DXVECTOR3* pIntersect);	//(out)交差点

private:
	//交差位置のポリゴンの超連を見つける.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices );

protected:
	std::shared_ptr<CStaticMesh>		m_pMesh;
	std::shared_ptr<CBoundingSphere>	m_pBSphere;

};
