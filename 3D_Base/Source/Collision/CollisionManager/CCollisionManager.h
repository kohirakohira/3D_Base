#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Collision//Shape//Volume//BoudingBox//CBoundingBox.h"			// バウンディングボックス
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"	// バウンディングスフィア

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

	void Draw();

	void CheckAllCollisions();

	// オブジェクトのバウンディングを作成
	void CreateBounding();

	// バウンディングの座標更新
	void UpdateBounding();


	void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
private:
	std::shared_ptr<CBoundingBox>			m_pBBox;
	std::shared_ptr<CBoundingSphere>		m_pBSphere;

	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;
};