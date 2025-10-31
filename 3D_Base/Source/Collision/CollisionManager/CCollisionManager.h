
#pragma once
//-----ライブラリ-----
#include <vector>
#include <memory>

//-----外部クラス-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

#include "GameObject//StaticMeshObject//StageObject//CStageObject.h"
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

	//void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
private:
	// 当たり判定のメッシュ
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallW;
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallH;
	std::shared_ptr<CStaticMesh>	m_pStaticMeshGround;

	// 壁
	std::shared_ptr<CStageObject>		m_pWallTop;
	std::shared_ptr<CStageObject>		m_pWallBottom;
	std::shared_ptr<CStageObject>		m_pWallLeft;
	std::shared_ptr<CStageObject>		m_pWallRight;

	// 木箱
	std::shared_ptr<CStageObject>		m_pWoodBoxTopLeft;
	std::shared_ptr<CStageObject>		m_pWoodBoxTopRight;
	std::shared_ptr<CStageObject>		m_pWoodBoxCenter;
	std::shared_ptr<CStageObject>		m_pWoodBoxBottomLeft;
	std::shared_ptr<CStageObject>		m_pWoodBoxBottomRight;

	// 地面
	std::shared_ptr<CStageObject>		m_pGround;

	//std::shared_ptr<CBody>	m_pBody;
};
