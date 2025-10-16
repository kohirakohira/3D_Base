#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Collision//Shape//Volume//BoudingBox//CBoundingBox.h"			// バウンディングボックス
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"	// バウンディングスフィア

#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h"		// 戦車：車体
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h"	// 戦車：砲塔
#include "GameObject/StaticMeshObject/Character/Player/PlayerManager/CPlayerManager.h"				
//キーの入力.
#include "InputDevice//Matuoka//CMultiInputKeyManager.h" // キー入力用

//===================================
//	コリジョンマネージャークラス
//===================================
class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();

	// ワイヤーメッシュ
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera);

	// オブジェクトの生成
	void Create();

	//データの読み込み.
	HRESULT LoadData();

	// バウンディングの座標更新
	void UpdateBounding();
	void CheckAllCollisions();

	// 外部のクラス情報をセット
	void SetPlayerManager(std::shared_ptr<CPlayerManager> pPlayerMgr);

private:
	std::shared_ptr<CStaticMesh>			m_pStaticMeshBSphere;		// バウンディングスフィア(当たり判定用).

	std::shared_ptr<CStaticMesh>			m_pCannonBBox;				// バウンディングボックス(砲塔)
	std::shared_ptr<CStaticMesh>			m_pBodyBBox;				// バウンディングボックス(車体)

	// プレイヤーマネージャー
	std::shared_ptr<CPlayerManager>			m_pPlayerManager;

		//キー入力.
	std::shared_ptr<CMultiInputKeyManager>	m_KeyInput;

	// 当たり判定表示フラグ
	bool						m_Draw;
};
