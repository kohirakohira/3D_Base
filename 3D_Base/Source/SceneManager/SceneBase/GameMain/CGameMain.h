#pragma once
//-----継承するクラス-----.
#include "SceneManager//SceneBase//CSceneBase.h" // シーン基底クラス.

//-----ライブラリ-----.
#include <algorithm>
#include <vector>
#include <array>	

//-----デバックテキスト-----.
#include "Assets//DebugText//CDebugText.h"		// デバッグテキストクラス.

//-----スプライト-----.
#include "Assets//Sprite//Sprite3D//CSprite3D.h" // 3Dスプライトクラス..
#include "Assets//Sprite//Sprite2D//CSprite2D.h" // 2Dスプライトクラス..

//-----スプライトオブジェクト-----.
#include "GameObject//SpriteObject//CSpriteObject.h"

//-----スプライト-----.
#include "GameObject//SpriteObject//Explosion//CExplosion.h" // 爆発スプライト.

//-----メッシュ-----.
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // スタティックメッシュクラス.
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // スタティックメッシュオブジェクトクラス.

//-----キャラクター-----.
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // キャラクタークラス.
#include "GameObject//StaticMeshObject//Character//Player//PlayerManager//CPlayerManager.h" // プレイヤーマネージャークラス.
#include "GameObject//StaticMeshObject//Shot//ShotManager//CShotManager.h" // 弾クラスマネージャー.
#include "GameObject//StaticMeshObject//Ground//CGround.h" // 地面クラス.
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h" // 戦車：砲塔クラス.

#include "Collision//BlastCollision//CBlastCollision.h"	//爆風クラス.

#include "GameObject/StaticMeshObject/ItemBoxManager/CItemBoxManager.h"//アイテムボックスマネージャークラス..

#include "Camera//CCamera.h" //カメラクラス.


//-----ステージオブジェクトクラス-----
#include "GameObject//StaticMeshObject//StageObject//CStageObject.h"

//-------------------------------.
// UI.
//-------------------------------.
#include "GameObject//UI//CUIObject//CUIObject.h" // UIオブジェクトクラス.
#include "GameObject//UI//Timer//CTimer.h"		  // タイマークラス.

//コントローラー
#include "XInput.h"

#include "Global.h"

class CXInput;

class CGameMain
	:public CSceneBase
{
public:

	CGameMain(HWND hWnd);
	~CGameMain()override;


	//動作関数..
	void Update()override;
	//描画関数..
	void Draw()override;
	//初期化関数..
	void Init()override;
	//解放関数..
	void Destroy()override;
	//インスタンス作成..
	void Create()override;
	//データの読み込み..
	HRESULT LoadData()override;

	//壁の位置設定.
	void SetPosition();

	//当たり判定の生成.
	void CreateBounding();
	// 当たり判定.
	void Collision();
	// 壁とプレイヤーの当たり判定判別.
	void WalltoPlayer();
	// 壁と弾の当たり判定
	void WalltoShot();
	// プレイヤーとプレイヤー当たり判定判別
	void PlayertoPlayer();
	// プレイヤーとアイテムボックス
	void PlayertoItemBox();
	// プレイヤーと弾
	void PlayertoShot();
	//爆風とプレイヤーの当たり判定.
	void PlayertoBlast();


	//画面をグリッドに分割したとき、idx番目のマスに対応する.
	//D3D11_VIEWPORTを作成して返す関数.
	static D3D11_VIEWPORT MakeGridViewport(int idx, int cols, int rows, float totalW, float totalH);

	//シーンの種類..
	CSceneType GetSceneType() const override;


	//ラッピング関数..
	//制限時間画像の設定..
	void EachSettingTimer();
	//プレイヤー番号画像の設定..
	void EachSettingPlayerNumber();
	//倒した数画像の設定..
	void EachSettingKillNumber();
	//倒した数画像の設定..
	void EachSettingHitPoint();

public:		
	//クラス用..
	//定数宣言..
	static constexpr int HP_MAX = 2;			//最大HP..
	static constexpr int PLAYERNUM_MAX = 4;		//プレイヤー番号..
	static constexpr int KILLNUM_MAX = 4;		//キル数..

	//ウィンドウハンドル..
	HWND		m_hWnd;

	//カメラ..
	//std::vector<std::shared_ptr<CCamera>> m_pCameras;.
	std::array<std::shared_ptr<CCamera>, PLAYER_MAX> m_pCameras;

	//デバッグテキスト..
	std::shared_ptr<CDebugText>									m_pDbgText;

	//ゲーム内で扱うUI系..
	std::shared_ptr<CSprite2D>									m_pSprite2DTimerArrow;			//時計の針..
	std::shared_ptr<CSprite2D>									m_pSprite2DTimerFrame;			//制限時間の枠..
	std::shared_ptr<CSprite2D>									m_pSprite2DTimer;				//制限時間の時計枠..
	std::shared_ptr<CSprite2D>									m_pSprite2DKillNomber;			//キル数の画像..
	std::shared_ptr<CSprite2D>									m_pSprite2DHitPoint;			//HPの画像..
	std::array < std::shared_ptr<CSprite2D>, PLAYERNUM_MAX>		m_pSprite2DPlayerIcon;			//プレイヤー番号画像..

	//スタティックメッシュオブジェクトクラス(UI)..
	std::array<std::shared_ptr<CUIObject>, PLAYERNUM_MAX>		m_pSpritePlayerIcon;			//プレイヤーアイコン..
	std::array<std::shared_ptr<CUIObject>, KILLNUM_MAX>			m_pSpriteKillNomber;			//キル数アイコン..
	std::array<std::shared_ptr<CUIObject>, HP_MAX>				m_pSpriteHitPoint;				//HPアイコン..
	std::shared_ptr<CUIObject>									m_pSpriteTimerFrame;			//制限時間の枠..
	std::shared_ptr<CUIObject>									m_pSpriteTimer;					//制限時間の時計枠..
	std::shared_ptr<CUIObject>									m_pSpriteTimerArrow;			//時計の針..

	//ゲームで扱うスプライトデータ(使いまわす資源)..
	std::unique_ptr<CSprite3D>		m_pSpriteGround;
	std::unique_ptr<CSprite3D>		m_pSpritePlayer;
	std::shared_ptr<CSprite3D>		m_pSpriteExplosion;

	//スタティックメッシュ(使いまわす資源).
	std::shared_ptr<CStaticMesh>	m_pStaticMeshGround;		//地面.
	std::shared_ptr<CStaticMesh>	m_pStaticMeshBSphere;		//バウンディングスフィア(当たり判定用)..
	std::shared_ptr<CStaticMesh>	m_pStaticMeshItemBox;		//アイテムボックス..

	// 戦車.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyRed;		// 車体赤.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonRed;	// 砲塔赤.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyYellow;	// 車体黄.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonYellow;	// 砲塔黄.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyBlue;		// 車体青.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonBlue;	// 砲塔青.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankBodyGreen;	// 車体緑.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_TankCannonGreen;	// 砲塔緑.

	// 弾.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletRed;		// 弾赤.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletYellow;		// 弾黄.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletBlue;		// 弾青.
	std::shared_ptr<CStaticMesh>	m_pStaticMesh_BulletGreen;		// 弾緑.

	//壁メッシュ.
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallW;
	std::shared_ptr<CStaticMesh>	m_pStaticMeshWallH;
	
	// スタティックメッシュオブジェクトクラス.
	std::unique_ptr<CStaticMeshObject>			m_pStcMeshObj;

	// プレイヤーマネージャー.
	std::shared_ptr<CPlayerManager>				m_pPlayerManager;

	// 弾クラスマネージャー.
	std::shared_ptr<CShotManager>				m_pShotManager;

	// 地面クラス..
	std::unique_ptr<CGround>					m_pGround;

	//タイマークラス..
	std::shared_ptr<CTimer>						m_Timer;

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

	//爆風.
	std::shared_ptr<CBlastCollision>	m_pBlast;

	//アイテムボックスマネージャークラス..
	std::shared_ptr<CItemBoxManager>			m_pItemBoxManager;

	// シーン列挙変数..
	CSceneType		m_SceneType;

public:		
	//変数用..
	//簡易時間を止める変数..
	int		m_StopTimeCount;
	//Iconの回転用..
	float	m_Rot;

	//時計の針.
	float	time;

	//これは何用？
	D3DXVECTOR3 push;

	std::shared_ptr<CXInput> m_pPad;
};