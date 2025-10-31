
#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

#include "GameObject//StaticMeshObject//StageObject//CStageObject.h"
#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h"
//===================================
//	コリジョンマネージャークラス
//===================================
class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();

	//動作関数..
	void Update();
	// オブジェクトの生成
	void Create();
	// データの読み込み
	HRESULT LoadData();

	//-----------------------
	// それぞれの判定関数
	//-----------------------
	
	// 壁とプレイヤーの当たり判定判別
	void WalltoPlayer();
	// 壁と弾の当たり判定
	void WalltoShot();
	// プレイヤーとプレイヤー当たり判定判別
	void PlayertoPlayer();
	// プレイヤーとアイテムボックス
	void PlayertoItemBox();
	// プレイヤーと弾
	void PlayertoShot();
	// 弾と弾
	void ShottoShot();
	// 箱とプレイヤー
	void WoodBoxtoPlayer();
	// 箱と弾
	void WoodBoxtoShot();
	// 地面と弾
	void GroundtoShot();
	// 地面とアイテムボックス
	void GroundtoItemBox();
	//爆風とプレイヤーの当たり判定.
	void PlayertoBlast();

	//-----------------------
	// 外部から情報をセット
	//-----------------------
	// 壁のセット
	void SetCStageWall(std::shared_ptr<CStageObject> pWallTop,
					   std::shared_ptr<CStageObject> pWallBottom,
					   std::shared_ptr<CStageObject> pWallLeft,
					   std::shared_ptr<CStageObject> pWallRight)
	{
		m_pWallTop	  = pWallTop;
		m_pWallBottom = pWallBottom;
		m_pWallLeft	  = pWallLeft;
		m_pWallRight  = pWallRight;
	}

	// 地面のセット
	void SetCStageGround(std::shared_ptr<CStageObject> pGround) { m_pGround = pGround; }

	// 木箱のセット
	void SetCStageWoodBox(std::shared_ptr<CStageObject> pWoodBoxTopLeft, 
						  std::shared_ptr<CStageObject> pWoodBoxTopRight,
						  std::shared_ptr<CStageObject> pWoodBoxCenter,
						  std::shared_ptr<CStageObject> pWoodBoxBottomLeft,
						  std::shared_ptr<CStageObject> pWoodBoxBottomRight)
	{
		m_pWoodBoxTopLeft	  = pWoodBoxTopLeft;
		m_pWoodBoxTopRight	  = pWoodBoxTopRight;
		m_pWoodBoxCenter	  = pWoodBoxCenter;
		m_pWoodBoxBottomLeft  = pWoodBoxBottomLeft;
		m_pWoodBoxBottomRight = pWoodBoxBottomRight;
	}

	// 弾マネージャーのセット
	void SetCShotManager(std::shared_ptr<CShotManager> pShot) { m_pShotManager = pShot; }

	// プレイヤーマネージャーのセット
	void SetCPlayerManager(std::shared_ptr<CPlayerManager> pPlayer) { m_pPlayerManager = pPlayer; }
private:
	// 当たり判定のメッシュ
	std::shared_ptr<CStaticMesh>		m_pCollisionWallW;		// 横向き壁
	std::shared_ptr<CStaticMesh>		m_pCollisionWallH;		// 縦向き壁
	std::shared_ptr<CStaticMesh>		m_pCollisionGround;		// 地面	
	std::shared_ptr<CStaticMesh>		m_pCollisionWoodBox;	// 木箱
	std::shared_ptr<CStaticMesh>		m_pCollisionItemBox;	// アイテムボックス
	std::shared_ptr<CStaticMesh>		m_pCollisionShot;		// 弾
	std::shared_ptr<CStaticMesh>		m_pCollisionCannon;		// 砲塔
	std::shared_ptr<CStaticMesh>		m_pCollisionBody;		// 車体


	// 壁
	std::shared_ptr<CStageObject>		m_pWallTop;
	std::shared_ptr<CStageObject>		m_pWallBottom;
	std::shared_ptr<CStageObject>		m_pWallLeft;
	std::shared_ptr<CStageObject>		m_pWallRight;

	// 地面
	std::shared_ptr<CStageObject>		m_pGround;

	// 木箱
	std::shared_ptr<CStageObject>		m_pWoodBoxTopLeft;
	std::shared_ptr<CStageObject>		m_pWoodBoxTopRight;
	std::shared_ptr<CStageObject>		m_pWoodBoxCenter;
	std::shared_ptr<CStageObject>		m_pWoodBoxBottomLeft;
	std::shared_ptr<CStageObject>		m_pWoodBoxBottomRight;

	// 弾クラスマネージャー
	std::shared_ptr<CShotManager>		m_pShotManager;

	// プレイヤーマネージャー
	std::shared_ptr<CPlayerManager>		m_pPlayerManager;
};
