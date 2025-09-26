#pragma once
//-----継承するクラス-----
#include "SceneManager//SceneBase//CSceneBase.h" // シーン基底クラス

//-----ライブラリ-----
#include <algorithm>
#include <vector>
#include <array>	

//-----デバックテキスト-----
#include "Assets//DebugText//CDebugText.h"		// デバッグテキストクラス

//-----スプライト-----
#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3Dスプライトクラス.
#include "Assets//Sprite//Sprite2D//CSprite2D.h" // 2Dスプライトクラス.

//-----スプライトオブジェクト-----
#include "GameObject//SpriteObject//CSpriteObject.h"

//-----スプライト-----
#include "GameObject//SpriteObject//Explosion//CExplosion.h" // 爆発スプライト

//-----メッシュ-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス

//-----キャラクター-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // キャラクタークラス
#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h" // プレイヤーマネージャークラス
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h" // 弾クラスマネージャー
#include "GameObject//StaticMeshObject//Ground//CGround.h" // 地面クラス
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h" // 戦車：砲塔クラス

#include "Camera//CCamera.h" //カメラクラス

//-----壁-----
#include "GameObject//StaticMeshObject//Wall//CWall.h"

//--------------------------------
// UI
//-------------------------------
#include "GameObject//UI//CUIObject//CUIObject.h" // UIオブジェクトクラス
#include "GameObject//UI//Timer//CTimer.h"		  // タイマークラス

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

	// ゲーム開始時の初期座標を設定
	void SetPosition();

	// 当たり判定のバウンディングと当たり判定の作成
	void CreateBounding();

	//当たり判定処理をここに入れる.
	void Collision();

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

	//スタティックメッシュオブジェクトクラス(UI).
	std::shared_ptr<CUIObject>		m_pSpriteTimerFrame;
	std::shared_ptr<CUIObject>		m_pSpriteTimer;
	std::shared_ptr<CUIObject>		m_pSpritePlayerIcon[PLAYER_MAX]; //プレイヤーアイコン.

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

	// 壁
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallW;		// 横に長い壁
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallH;		// 縦に長い壁
	
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

	// 壁クラス
	std::shared_ptr<CWall>		m_pWallTop;		// 上壁
	std::shared_ptr<CWall>		m_pWallBottom;	// 下壁
	std::shared_ptr<CWall>		m_pWallLeft;	// 左壁
	std::shared_ptr<CWall>		m_pWallRight;	// 右壁

	// シーン列挙変数.
	CSceneType		m_SceneType;

	// 簡易時間を止める変数.
	int m_StopTimeCount;
};