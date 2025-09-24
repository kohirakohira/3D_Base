#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Collision//Shape//Volume//BoudingBox//CBoundingBox.h"			// バウンディングボックス
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"	// バウンディングスフィア

#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9クラス
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス

#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h"		// 戦車：車体
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h"	// 戦車：砲塔

//===================================
//	コリジョンマネージャークラス
//===================================
class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();

	// ワイヤーメッシュ
	void Draw();

	// オブジェクトの生成
	void Create();
	//データの読み込み.
	HRESULT LoadData();

	// バウンディングの座標更新
	void UpdateBounding();
	void CheckAllCollisions();

	void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
private:
	std::shared_ptr<CStaticMesh>			m_pStaticMeshBSphere;		//バウンディングスフィア(当たり判定用).

	std::shared_ptr<CBoundingBox>			m_pBBox;
	std::shared_ptr<CBoundingSphere>		m_pBSphere;

	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;
};