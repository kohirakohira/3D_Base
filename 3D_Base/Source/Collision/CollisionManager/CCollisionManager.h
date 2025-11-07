
#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

#include "GameObject//StaticMeshObject//StageObject//CStageObject.h"
#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h"

#include "Collision/BlastCollision/BlastCollisionManager/CBlastCollisionManager.h"
#include "GameObject/StaticMeshObject/ItemBoxManager/CItemBoxManager.h"

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
	// 木箱とプレイヤー
	void WoodBoxtoPlayer();
	// 木箱と弾
	void WoodBoxtoShot();
	// 地面と弾
	void GroundtoShot();
	// 地面とアイテムボックス
	void GroundtoItemBox();
	// 爆風とプレイヤーの当たり判定
	void PlayertoBlast();

	//-----------------------
	// 外部から情報をセット
	//-----------------------
	// 弾(爆風用)をセット
	void SetStaticBlast(std::shared_ptr<CStaticMesh> pBlast) { m_pStaticBlast = pBlast; }

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

	// 爆風当たり判定マネージャーのセット
	void SetCBlastCollisionManager(std::shared_ptr<CBlastCollisionManager> pBlast) { m_pBlastManager = pBlast; }

	// アイテムボックスマネージャーのセット
	void SetCItemBoxManager(std::shared_ptr<CItemBoxManager> pItemBox) { m_pItemBoxManager = pItemBox; }

	//メッシュの設定.
	void SetBlastMesh(std::shared_ptr<CStaticMesh> mesh) { m_pStaticBlast = mesh; }

private:
	// 爆風用のメッシュ
	std::shared_ptr<CStaticMesh>		m_pStaticBlast;			// 爆風のメッシュ

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

	// 爆風当たり判定マネージャー
	std::shared_ptr<CBlastCollisionManager> m_pBlastManager;

	// アイテムボックスマネージャークラス
	std::shared_ptr<CItemBoxManager>	m_pItemBoxManager;
};
