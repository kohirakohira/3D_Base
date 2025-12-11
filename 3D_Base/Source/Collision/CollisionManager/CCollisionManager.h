
#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>
#include <ranges>

//-----外部クラス-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

#include "GameObject//StaticMeshObject//StageObject//CStageObject.h"
#include "GameObject/StaticMeshObject/Character/CharacterObject/Player/CharacterManager/CCharacterManager.h"

#include "GameObject/StaticMeshObject/ItemBoxManager/CItemBoxManager.h"
#include "../../GameObject/StaticMeshObject/BlastManager/CBlastManager.h"

#include "Assets//Data/Singleton/GameDataManager/CGameDataManager.h"		//倒した数をカウントするクラス※シングルトン化.

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
	void CharactertoItemBox();
	// プレイヤーと弾
	void PlayertoShot();
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
	//アイテムと木箱.
	void ItemtoWoodBox();

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

	// キャラクターマネージャーのセット
	void SetCPlayerManager(std::shared_ptr<CCharacterManager> pPlayer) { m_pCharacterManager = pPlayer; }

	// アイテムボックスマネージャーのセット
	void SetCItemBoxManager(std::shared_ptr<CItemBoxManager> pItemBox) { m_pItemBoxManager = pItemBox; }

	//爆風マネージャーの設定.
	void SetCBlastManager(std::shared_ptr<CBlastManager> blast) { m_pBlastManager = blast; }

	//メッシュの設定.
	void SetBlastMesh(std::shared_ptr<CStaticMesh> mesh) { m_pStaticBlast = mesh; }

	//アイテムの設定.
	void SetItemInfomation(int Itemindex, int Playerindex);

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

	// キャラクターマネージャー
	std::shared_ptr<CCharacterManager>		m_pCharacterManager;

	// アイテムボックスマネージャークラス
	std::shared_ptr<CItemBoxManager>	m_pItemBoxManager;

	//爆風マネージャークラス.
	std::shared_ptr<CBlastManager>		m_pBlastManager;

	//秒数.
	float m_Speed;

};
