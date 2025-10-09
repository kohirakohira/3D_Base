#pragma once
//-----継承するクラス-----
#include "GameObject//CGameObject.h" // ゲームオブジェクトクラス 

//-----ライブラリ-----
#include <iostream>

//-----外部クラス-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h" // バウンディングスフィア
#include "Collision//Shape//Volume//BoudingBox//CBoundingBox.h"	// バウンディングボックス
#include "Collision//Shape//Ray//CRay.h" // レイクラス

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
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//メッシュを接続する.
	void AttachMesh(std::shared_ptr<CStaticMesh> pMesh) {
		m_pMesh = pMesh;
	}
	//メッシュを切り離す.
	void DetachMesh() {
		m_pMesh = nullptr;
	}

	// バウンディングスフィア取得
	std::shared_ptr<CBoundingSphere> GetBSphere() const {
		return m_pBSphere;
	}
	// バウンディングボックス取得
	std::shared_ptr<CBoundingBox> GetBBox() const {
		return m_pBBox;
	}

	//モデルに合わせたバウンディングスフィア作成のラッパー関数
	HRESULT CreateBSphereForMesh(const CStaticMesh& pMesh) {
		return m_pBSphere->CreateSphereForMesh(pMesh);
	}
	//バウンディングボックス作成のラッパー関数.
	HRESULT CreateBBoxForMesh(const CStaticMesh& pMesh) {
		return m_pBBox->CreateBoxForMesh(pMesh);
	}

	//バウンディングボックスをオブジェクト位置に合わせる
//※モデルの原点が中心の場合を想定
	void UpdateBBoxPos() {
		m_pBBox->SetPosition(m_vPosition);
	}

	//バウンディングスフィアをオブジェクト位置に合わせる
//※モデルの原点が中心の場合を想定
	void UpdateBSpherePos() {
		m_pBSphere->SetPosition(m_vPosition);
	}

	std::shared_ptr<CStaticMesh> GetStaticMesh() { return m_pMesh; }

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
	std::shared_ptr<CStaticMesh>		m_pMesh;
	std::shared_ptr<CBoundingSphere>	m_pBSphere;
	std::shared_ptr<CBoundingBox>		m_pBBox;

};

