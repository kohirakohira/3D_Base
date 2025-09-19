#pragma once
//-----継承するクラス-----
#include "GameObject//CGameObject.h" // ゲームオブジェクトクラス
//-----ライブラリ-----
#include <iostream>

//-----外部クラス-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"	// バウンディングスフィア
#include "Collision/Shape/Volume/BoudingBox/CBoundingBox.h"				// バウンディングボックス
#include "Collision//Shape//Ray//CRay.h" // レイクラス
#include "Collision/Collider/CCollider.h"

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
	//ここで当たり判定の位置情報を更新しているので当たり判定が必要な物は絶対にこの場所のUpdateを通ること.
	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//メッシュを接続する.
	void AttachMesh(std::shared_ptr<CStaticMesh> pMesh) {
		m_pMesh = pMesh;
	}
	//メッシュを切り離す.
	void DetachMesh() {
		m_pMesh = nullptr;
	}

	//SphereColliderを作成する.
	void CreateSpehreCollider(float radius);
	//BoxColliderを作成する.
	void CreateBoxCollider(D3DXVECTOR3 min, D3DXVECTOR3 max);

	//モデルに合わせたバウンディングスフィア作成のラッパー関数
	HRESULT CreateBSphereForMesh(const CStaticMesh& pMesh) {
		return m_pBSphere->CreateSphereForMesh(pMesh);
	}
	//バウンディングボックス作成のラッパー関数.
	HRESULT CreateBBoxForMesh(const CStaticMesh& pMesh) {
		return m_pBBox->CreateBoxForMesh(pMesh);
	}

	std::shared_ptr<CStaticMesh> GetStaticMesh() { return m_pMesh; }

	//当たり判定の型を取得.
	std::shared_ptr<CCollider> GetCollider() const { return m_pCollider.get(); }

	float GetRadius() { return m_pBSphere->GetRadius(); }

	D3DXVECTOR3 GetMinPos() { return m_pBBox->GetMinPosition(); }
	D3DXVECTOR3 GetMaxPos() { return m_pBBox->GetMaxPosition(); }

	D3DXVECTOR3 DebugMin() { return m_vPosition + m_pBBox->GetMinPosition(); }
	D3DXVECTOR3 DebugMax() { return m_vPosition + m_pBBox->GetMaxPosition(); }

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
		D3DXVECTOR3* pVertices);

protected:
	std::shared_ptr<CStaticMesh> m_pMesh;

	std::shared_ptr<CBoundingSphere> m_pBSphere;
	std::shared_ptr<CBoundingBox> m_pBBox;

	std::shared_ptr<CCollider>	m_pCollider;
};
