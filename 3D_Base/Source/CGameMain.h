#pragma once
#include <algorithm>
#include <vector>
#include <array>	

#include "CDebugText.h"			//仮.
#include "CSprite3D.h"
#include "CSprite2D.h"

#include "CSpriteObject.h"
#include "CUIObject.h"

#include "CExplosion.h"

#include "CStaticMesh.h"
#include "CStaticMeshObject.h"

#include "CCharacter.h"
#include "CPlayerManager.h"
#include "CShotManager.h"
#include "CGround.h"
#include "CCannon.h"

#include "CCamera.h"

#include "CTimer.h"				//時間用.

#include "CSceneBase.h"

#include "Global.h"

class CGameMain
	:public CSceneBase
{
public:

	CGameMain(HWND hWnd);
	~CGameMain()override;


	//動作関数.
	void Update()override;
	//描画関数.
	void Draw()override;
	//初期化関数.
	void Init()override;
	//解放関数.
	void Destroy()override;
	//インスタンス作成.
	void Create()override;
	//データの読み込み.
	HRESULT LoadData()override;


	//画面をグリッドに分割したとき、idx番目のマスに対応する
	//D3D11_VIEWPORTを作成して返す関数
	static D3D11_VIEWPORT MakeGridViewport(int idx, int cols, int rows, float totalW, float totalH);

	//シーンの種類.
	CSceneType GetSceneType() const override;

public:
	//ウィンドウハンドル.
	HWND		m_hWnd;

	//カメラ.
	//std::vector<std::shared_ptr<CCamera>> m_pCameras;
	std::array<std::shared_ptr<CCamera>, PLAYER_MAX> m_pCameras;

	//デバッグテキスト.
	std::shared_ptr<CDebugText>		m_pDbgText;

	//ゲーム内で扱うUI系.
	std::shared_ptr<CSprite2D>		m_pSprite2DTimerFrame;	//制限時間の枠.
	std::shared_ptr<CSprite2D>		m_pSprite2DTimer;		//制限時間の時計枠.

	//スタティックメッシュオブジェクトクラス.
	std::shared_ptr<CUIObject>		m_pSpriteTimerFrame;
	std::shared_ptr<CUIObject>		m_pSpriteTimer;

	//ゲームで扱うスプライトデータ(使いまわす資源).
	std::unique_ptr<CSprite3D>		m_pSpriteGround;
	std::unique_ptr<CSprite3D>		m_pSpritePlayer;
	std::shared_ptr<CSprite3D>		m_pSpriteExplosion;

	//スタティックメッシュ(使いまわす資源)
	std::shared_ptr<CStaticMesh>	m_pStaticMeshGround;		//地面
	std::shared_ptr<CStaticMesh>	m_pStaticMeshBSphere;		//バウンディングスフィア(当たり判定用).



	// 戦車
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyRed;		// 車体赤
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonRed;	// 砲塔赤
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyYellow;	// 車体黄
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonYellow;	// 砲塔黄
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyBlue;		// 車体青
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonBlue;	// 砲塔青
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyGreen;	// 車体緑
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonGreen;	// 砲塔緑

	// 弾
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletRed;		// 弾赤
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletYellow;		// 弾黄
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletBlue;		// 弾青
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletGreen;		// 弾緑

	// スタティックメッシュオブジェクトクラス
	std::unique_ptr<CStaticMeshObject>			m_pStcMeshObj;

	// プレイヤーマネージャー
	std::shared_ptr<CPlayerManager>				m_pPlayerManager;

	// 弾クラスマネージャー
	std::shared_ptr<CShotManager>				m_pShotManager;

	// 地面クラス.
	std::unique_ptr<CGround>					m_pGround;

	//タイマークラス.
	std::shared_ptr<CTimer>						m_Timer;

	// シーン列挙変数.
	CSceneType		m_SceneType;

	// 簡易時間を止める変数.
	int m_StopTimeCount;

};