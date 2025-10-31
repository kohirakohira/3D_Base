#define GetKey(KEY) (GetAsyncKeyState(KEY))
#undef min	//マクロ定義無効化.
#undef max	

static bool prevC = false;
static bool prevA = false;
#include "CGameMain.h"
//-----サウンド-----.
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス.
#include "Assets//Effect//CEffect.h"	//Effekseerを使うためのクラス.

//-----DirectX-----.
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9クラス.
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス.

//定数宣言.
static constexpr int TIME = 90.0;

CGameMain::CGameMain(HWND hWnd)
	: m_hWnd						( hWnd )

	// 画像
	, m_pSprite2DTimerArrow			( nullptr )
	, m_pSprite2DTimerFrame			( nullptr )
	, m_pSprite2DTimer				( nullptr )
	, m_pSprite2DKillNomber			( nullptr )
	, m_pSprite2DHitPoint			( nullptr )
	, m_pSprite2DPlayerIcon			()

	// 画像の設定
	, m_pSpriteTimerFrame			()
	, m_pSpriteTimer				()
	, m_pSpriteTimerArrow			()
	, m_pSpritePlayerIcon			()
	, m_pSpriteKillNomber			()
	, m_pSpriteHitPoint				()

	, m_pSpriteGround				( nullptr )
	, m_pSpritePlayer				( nullptr )
	, m_pSpriteExplosion			( nullptr )

	, m_pStaticMeshStage			( nullptr )
	, m_pStaticMeshBSphere			( nullptr )
	, m_pStaticMeshItemBox			( nullptr )

	// 戦車
	, m_pStaticMesh_TankBodyRed		( nullptr )
	, m_pStaticMesh_TankCannonRed	( nullptr )
	, m_pStaticMesh_TankBodyYellow	( nullptr )
	, m_pStaticMesh_TankCannonYellow( nullptr )
	, m_pStaticMesh_TankBodyBlue	( nullptr )
	, m_pStaticMesh_TankCannonBlue	( nullptr )
	, m_pStaticMesh_TankBodyGreen	( nullptr )
	, m_pStaticMesh_TankCannonGreen	( nullptr )

	// 弾
	, m_pStaticMesh_BulletRed		( nullptr )
	, m_pStaticMesh_BulletYellow	( nullptr )
	, m_pStaticMesh_BulletBlue		( nullptr )
	, m_pStaticMesh_BulletGreen		( nullptr )

	// 壁のメッシュ
	, m_pStaticMeshWallW			( nullptr )
	, m_pStaticMeshWallH			( nullptr )

	// 地面のメッシュ
	, m_pStaticMeshGround			( nullptr )

	// 木箱のメッシュ
	, m_pStaticMeshWoodBox			( nullptr )

	, m_pPlayerManager				()
	, m_pShotManager				()

	, m_pStage						( nullptr )

	, m_pDbgText					( nullptr )

	, m_StopTimeCount				( 0 )
	, m_pCameras					()

	, m_Timer						( nullptr )

	, m_pWallTop					( nullptr )
	, m_pWallBottom					( nullptr )
	, m_pWallLeft					( nullptr )
	, m_pWallRight					( nullptr )

	, m_pWoodBoxTopLeft				( nullptr )
	, m_pWoodBoxTopRight			( nullptr )
	, m_pWoodBoxCenter				( nullptr )
	, m_pWoodBoxBottomLeft			( nullptr )
	, m_pWoodBoxBottomRight			( nullptr )

	, m_pGround						( nullptr )
	
	,m_pItemBoxManager				( nullptr )

	, m_Rot							( 0.0f )

	, time							( 0.0f )

	, m_pBlastManager				( nullptr )

{
	//最初のシーンをメインにする..
	m_SceneType = CSceneType::Main;
}

CGameMain::~CGameMain()
{
	//外部で作成しているので、ここでは破棄しない.
	m_hWnd = nullptr;
}

void CGameMain::Update()
{
	//BGMのループ再生..
	//CSoundManager::PlayLoop(CSoundManager::BGM_Main);

//-----メイン演出用-----..
	
	//Iconを回転させる..
	m_Rot += 0.02f;
	
//-----メイン演出用-----..

	//プレイヤー全員更新.
	m_pPlayerManager->Update();

	// 弾の発射.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(i))
		{
			if (player->GetCannon()->IsShot())
			{
				m_pShotManager->SetReload(i,
					player->GetCannon()->GetPosition(),
					player->GetCannon()->GetRotation().y);
			}
		}
	}
	m_pShotManager->Update();


	//カメラ追従＆更新.砲塔基準
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(i))
		{
			const D3DXVECTOR3 camPos = player->GetCannonPosition();	//砲塔の位置.
			float yaw = player->GetCannonYaw();	//砲塔の向きY.

			m_pCameras[i]->SetTargetPos(camPos);
			m_pCameras[i]->SetTargetRotY(yaw);
		}
		m_pCameras[i]->Update();
	}

	//アイテムの動作..
	m_pItemBoxManager->Update();
	
	//定数宣言..
	const float PI = 3.14159265358979f;
	//時計の針の回転..
	float remaining = m_Timer->GetRemainingTime();
	float totle = 90.f;
	time = (remaining / totle) * 360;
	float angle = time * (PI / 180);
	m_pSpriteTimerArrow->SetRotation(0.f, 0.f, angle);

	const bool nowC = (GetAsyncKeyState('C') & 0x8000) != 0;

	if (nowC && !prevC)
	{
		m_pPlayerManager->SwitchActivePlayer();
	}
	prevC = nowC;


	// Cキー押されたら操作プレイヤー切り替え
	if (GetAsyncKeyState('C') & 0x0001)
	{
		m_pPlayerManager->SwitchActivePlayer();
	}

	// 壁の更新
	m_pWallTop->Update();
	m_pWallBottom->Update();
	m_pWallLeft->Update();
	m_pWallRight->Update();

	//爆風の動作処理.
	//m_pBlastManager->Update();
	//m_pBlastManager->SetScale(m_pBlastManager->GetBlastRadius() * 2);

	// 木箱の更新
	m_pWoodBoxTopLeft->Update();
	m_pWoodBoxTopRight->Update();
	m_pWoodBoxCenter->Update();
	m_pWoodBoxBottomLeft->Update();
	m_pWoodBoxBottomRight->Update();

	// 地面の更新
	m_pGround->Update();

	// 当たり判定
	Collision();

	//勝敗条件(確認用)..
	//勝ち..
	if (GetKey('K') & 0x8000)
	{
		//BGMのループ停止..
		CSoundManager::Stop(CSoundManager::BGM_Bonus);

		m_SceneType = CSceneType::Result;
	}
	//敗北..
	//体力がなくなるか.
	if (GetKey('L') & 0x8000)
	{
		//BGMのループ停止..
		CSoundManager::Stop(CSoundManager::BGM_Bonus);

		m_SceneType = CSceneType::Result;
	}

}



void CGameMain::Draw()
{
	auto* pContext = CDirectX11::GetInstance().GetContext();

	//画面サイズfloatで扱うD3D11_VIEWPORTの型に合わせる.
	const float W = static_cast<float>(WND_W);
	const float H = static_cast<float>(WND_H);

	//2x2分割の定義.
	const int COLS = 2;		//2を1にしたら一画面、2なら四画面..
	const int ROWS = 2;		//2を1にしたら一画面、2なら四画面..
	const int MAX_VIEWS = COLS * ROWS;					//分割して表示できる最大ビュー数.
	const int VIEWS = std::min(PLAYER_MAX, MAX_VIEWS);	//minで小さいほうに合わせる.

	//1ビューポート分を描画する処理をラムダにまとめる.
	auto DrawOneViewport = [&](std::shared_ptr<CCamera> camera, std::shared_ptr<CPlayer> owner)
	{
		//カメラ更新.
		camera->Update();

		//スナップショットをconst参照でキャプチャ.
		D3DXMATRIX& view	= camera->m_mView;
		D3DXMATRIX& proj	= camera->m_mProj;
		LIGHT&		light	= camera->m_Light;
		CAMERA&		paramC	= camera->m_Camera;

		//プレイヤーを描画.ここで全員描く.
		for (int players = 0; players < PLAYER_MAX; ++players)
		{
			if (auto p = m_pPlayerManager->GetControlPlayer(players))
			{
				p->Draw(view, proj, light, paramC);
			}
		}

//オブジェクトの描画..
		//弾描画..
		m_pShotManager->Draw(view, proj, light, paramC);

		//地面描画.
		m_pStage->Draw(view, proj, light, paramC);

		// 木箱の描画
		m_pWoodBoxTopLeft->Draw(view, proj, light, paramC);
		m_pWoodBoxTopRight->Draw(view, proj, light, paramC);
		m_pWoodBoxCenter->Draw(view, proj, light, paramC);
		m_pWoodBoxBottomLeft->Draw(view, proj, light, paramC);
		m_pWoodBoxBottomRight->Draw(view, proj, light, paramC);

		//アイテムボックスの描画.
		m_pItemBoxManager->Draw(view, proj, light, paramC);

		//爆風の表示.
		m_pBlastManager->Draw(view, proj, light, paramC);

//4画面に体力を表示.
		//前後関係無視..
		CDirectX11::GetInstance().SetDepth(false);
		//UI.
		for (int i = 0; i < HP_MAX; i++)
		{
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, m_Rot);
			m_pSpriteHitPoint[i]->Draw();
		}
		CDirectX11::GetInstance().SetDepth(true);

	};
	//分割ビューのループ.
	for (int i = 0; i < VIEWS; ++i)
	{
		//ビューポート設定.
		const D3D11_VIEWPORT vp = MakeGridViewport(i, COLS, ROWS, W, H);
		pContext->RSSetViewports(1, &vp);

		//カメラ参照を取得.参照外しで実体を直接扱う.
		std::shared_ptr<CCamera> camera = m_pCameras[i];

		std::shared_ptr<CPlayer> owner = m_pPlayerManager->GetControlPlayer(i);
		////////デバッグテキストの描画.
		//////m_pDbgText->SetColor(0.9f, 0.6f, 0.f);	//色の設定.
		//////m_pDbgText->Render(_T("ABCD"), 10, 100);.

	//1ビュー分を描画.
		DrawOneViewport(camera, owner);
		////////デバッグテキスト(数値入り)の描画.
		//////m_pDbgText->SetColor(1.f, 0.f, 0.f);.
		//////TCHAR dbgText[64];.
		//////_stprintf_s(dbgText, _T("Float:%f, %f"), 1.f, 2.2f);.
		//////m_pDbgText->Render(dbgText, 10, 110);.


//4画面の時の表示..
		//前後関係無視..
		CDirectX11::GetInstance().SetDepth(false);
		//プレイヤー番号の描画..
		switch (i)
		{
		case 0:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		case 1:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		case 2:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		case 3:
			m_pSpritePlayerIcon[i]->Draw();
			break;
		default:
			break;
		}
		//キル数の描画..
		m_pSpriteKillNomber[i]->Draw();
		CDirectX11::GetInstance().SetDepth(true);

	}
	//全画面ビューポートに戻す.
	D3D11_VIEWPORT fullvp = {};
	fullvp.TopLeftX = 0;		//ビューポート左上のX座標.
	fullvp.TopLeftY = 0;		//ビューポート左上のY座標.
	fullvp.Width = 1920;		//ビューポートの幅.ここで今回は画面全画面を基準とする.
	fullvp.Height = 1080;		//ビューポートの高さ.
	fullvp.MinDepth = 0.0f;		//深度バッファの最小値.
	fullvp.MaxDepth = 1.0f;		//深度バッファの最大値.
	pContext->RSSetViewports(1, &fullvp);

	////1画面の時の表示..
	//前後関係無視..
	CDirectX11::GetInstance().SetDepth(false);
	//タイマーの枠の描画..
	m_pSpriteTimerFrame->Draw();
	//タイマーの描画..
	m_pSpriteTimer->Draw();
	//タイマーの描画..
	m_pSpriteTimerArrow->Draw();
	//タイマー描画..
	m_Timer->Draw();
	CDirectX11::GetInstance().SetDepth(true);


	//エフェクトもここでやる.

}




void CGameMain::Init()
{
	//カメラ位置設定..
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//プレイヤーマネージャーから各プレイヤーの位置を取得.
		D3DXVECTOR3 pos = m_pPlayerManager->GetPosition(i);

		m_pCameras[i]->SetCameraPos(pos.x, pos.y, pos.z);
		m_pCameras[i]->SetLightPos(0.f, 2.f, 5.f);
	}
	//地面の大きさ設定..
	m_pStage->SetRotation(0.f, 0.f, 0.f);
	m_pStage->SetScale(0.4f, 0.4f, 0.4f);

	//アイテムボックスの設定..
	m_pItemBoxManager->SetPosition(-10.f, 20.f, 0.f);
	m_pItemBoxManager->SetRotation(0.0f, 0.0f, 0.0f);
	m_pItemBoxManager->SetScale(0.2f, 0.2f, 0.2f);

//-----UI系統の初期化-----.
	//時計の枠.
	m_pSpriteTimerFrame->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
	//時計本体.
	m_pSpriteTimer->SetPosition(WND_W / 2.f - 74.f, WND_H / 2 - 32.f, 0.f);
	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);
	//制限時間画像の設定..
	EachSettingTimer();
	//プレイヤー番号画像の設定..
	EachSettingPlayerNumber();
	//倒した数画像の設定..
	EachSettingKillNumber();
	//HPの画像の設定..
	EachSettingHitPoint();

	//制限時間の文字サイズ..
	m_pDbgText->SetFontSize(5.0f);

	//タイマーの初期化.
	m_Timer->StartTimer(TIME);
	m_Timer->SetDebugFont(m_pDbgText);
	m_Timer->SetTimerPosition(WND_W / 2 - 15.f, WND_H / 2 - 30.f);

	SetPosition();
}

void CGameMain::Destroy()
{

}

void CGameMain::Create()
{
	//Effectクラス.
	CEffect::GetInstance().Create(
		CDirectX11::GetInstance().GetDevice(),
		CDirectX11::GetInstance().GetContext());

	//UIObjectのインスタンス生成.
	m_pSpriteTimerFrame = std::make_shared<CUIObject>();
	m_pSpriteTimer		= std::make_shared<CUIObject>();
	m_pSpriteTimerArrow = std::make_shared<CUIObject>();

	//HPの分だけ生成..
	for (int i = 0; i < HP_MAX; i++)
	{
		m_pSpriteHitPoint[i] = std::make_shared<CUIObject>();
	}
	//プレイヤーの分だけ生成..
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		m_pSpritePlayerIcon[i] = std::make_shared<CUIObject>();
	}
	//キル数の分だけ生成..
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		m_pSpriteKillNomber[i] = std::make_shared<CUIObject>();
	}

	//UI系のインスタンス生成..
	m_pSprite2DTimerFrame	= std::make_shared<CSprite2D>();
	m_pSprite2DTimer		= std::make_shared<CSprite2D>();
	m_pSprite2DTimerArrow	= std::make_shared<CSprite2D>();
	m_pSprite2DKillNomber	= std::make_shared<CSprite2D>();
	m_pSprite2DHitPoint		= std::make_shared<CSprite2D>();
	//プレイヤーの分だけ生成..
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		m_pSprite2DPlayerIcon[i] = std::make_shared<CSprite2D>();
	}

	//スプライトのインスタンス作成..
	m_pSpriteGround = std::make_unique<CSprite3D>();
	m_pSpritePlayer = std::make_unique<CSprite3D>();
	m_pSpriteExplosion = std::make_shared<CSprite3D>();

	//スタティックメッシュのインスタンス作成.
	m_pStaticMeshStage			= std::make_shared<CStaticMesh>();
	m_pStaticMeshBSphere		= std::make_shared<CStaticMesh>();
	m_pStaticMeshItemBox		= std::make_shared<CStaticMesh>();

	// 戦車のメッシュ..
	m_pStaticMesh_TankBodyRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonBlue	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyGreen		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonGreen	= std::make_shared<CStaticMesh>();

	// 弾のメッシュ..
	m_pStaticMesh_BulletRed			= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletYellow		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletGreen		= std::make_shared<CStaticMesh>();

	//壁のメッシュ.
	m_pStaticMeshWallW				= std::make_shared<CStaticMesh>();
	m_pStaticMeshWallH				= std::make_shared<CStaticMesh>();

	// 地面のメッシュ
	m_pStaticMeshGround				= std::make_shared<CStaticMesh>();

	// 木箱のメッシュ
	m_pStaticMeshWoodBox			= std::make_shared<CStaticMesh>();

	//デバッグテキストのインスタンス作成.
	m_pDbgText = std::make_unique<CDebugText>();

	//プレイヤーと砲塔のインスタンス生成.
	m_pPlayerManager = std::make_shared<CPlayerManager>();
	
	//マネージャーは一回だけInitialize.
	m_pPlayerManager->Initialize();

	//弾クラスのインスタンス作成.
	m_pShotManager = std::make_shared<CShotManager>();
	m_pShotManager->Initialize();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//プレイヤーiの位置を変更
		float offsetX = 20.0f;
		float offsetZ = 20.0f;

		// プレイヤーの向き
		float AngleY = 45.0;

		if (i == 0)
		{
			m_pPlayerManager->SetPlayerPosition(i, D3DXVECTOR3(-offsetX, 0.0f, -offsetZ));
			//回転を設定..
			m_pPlayerManager->SetPlayerRotation(i, D3DXVECTOR3(0.f, D3DXToRadian(AngleY), 0.f));
		}
		else if (i == 1)
		{
			m_pPlayerManager->SetPlayerPosition(i, D3DXVECTOR3(-offsetX, 0.0f, offsetZ));
			//回転を設定..
			m_pPlayerManager->SetPlayerRotation(i, D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 3), 0.f));
		}
		else if (i == 2)
		{
			m_pPlayerManager->SetPlayerPosition(i, D3DXVECTOR3(offsetX, 0.0f, offsetZ));
			//回転を設定..
			m_pPlayerManager->SetPlayerRotation(i, D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 5), 0.f));
		}
		else if (i == 3)
		{
			m_pPlayerManager->SetPlayerPosition(i, D3DXVECTOR3(offsetX, 0.0f, -offsetZ));
			//回転を設定..
			m_pPlayerManager->SetPlayerRotation(i, D3DXVECTOR3(0.f, D3DXToRadian(AngleY * 7), 0.f));
		}

		//カメラ生成・セットアップ.
		auto camera = std::make_unique<CCamera>();
		camera->SetTargetPos(m_pPlayerManager->GetPosition(i));
		camera->SetTargetRotY(m_pPlayerManager->GetRotation(i).y);
		m_pCameras[i] = std::move(camera);
	}

	//地面クラスのインスタンス作成
	m_pStage = std::make_unique<CGround>();

	//制限時間のインスタンス生成
	m_Timer = std::make_shared<CTimer>();

	//壁
	m_pWallTop		= std::make_shared<CStageObject>();
	m_pWallBottom	= std::make_shared<CStageObject>();
	m_pWallLeft		= std::make_shared<CStageObject>();
	m_pWallRight	= std::make_shared<CStageObject>();

	// 木箱
	m_pWoodBoxTopLeft		= std::make_shared<CStageObject>();
	m_pWoodBoxTopRight		= std::make_shared<CStageObject>();
	m_pWoodBoxCenter		= std::make_shared<CStageObject>();
	m_pWoodBoxBottomLeft	= std::make_shared<CStageObject>();
	m_pWoodBoxBottomRight	= std::make_shared<CStageObject>();

	// 地面
	m_pGround = std::make_shared<CStageObject>();
	
	//爆発クラスのインスタンス生成.
	m_pBlastManager = std::make_shared<CBlastCollisionManager>();

	//アイテムマネージャークラスのインスタンス生成..
	m_pWoodBoxTopLeft		= std::make_shared<CStageObject>();
	m_pWoodBoxTopRight		= std::make_shared<CStageObject>();
	m_pWoodBoxCenter		= std::make_shared<CStageObject>();
	m_pWoodBoxBottomLeft	= std::make_shared<CStageObject>();
	m_pWoodBoxBottomRight	= std::make_shared<CStageObject>();

	// アイテムマネージャークラスのインスタンス生成
	m_pItemBoxManager = std::make_shared<CItemBoxManager>();
	m_pItemBoxManager->Create();
}

HRESULT CGameMain::LoadData()
{
	//デバッグテキストの読み込み..
	if (FAILED(m_pDbgText->Init(CDirectX11::GetInstance())))
	{
		return E_FAIL;
	}

	//Effectクラス.
	if (FAILED(CEffect::GetInstance().LoadData())) {
		return E_FAIL;
	}

	//タイマー画像のスプライト設定
	CSprite2D::SPRITE_STATE WH_SIZE = {
		1920, 1080,		//描画幅,高さ..
		1920, 1080,		//元画像の幅,高さ..
		1920, 1080		//アニメーションをしないので、0でいい..
	};
	//タイマー枠画像のスプライト設定
	CSprite2D::SPRITE_STATE TIMER_SIZE = {
		256, 256,		//描画幅,高さ..
		256, 256,		//元画像の幅,高さ..
		256, 256		//アニメーションをしないので、0でいい..
	};
	//タイマー枠画像のスプライト設定
	CSprite2D::SPRITE_STATE ICON_SIZE = {
		256, 256,		//描画幅,高さ..
		256, 256,		//元画像の幅,高さ..
		256, 256		//アニメーションをしないので、0でいい..
	};
	//制限時間の枠の読み込み
	m_pSprite2DTimerFrame	->Init(_T("Data\\Texture\\UI\\Timer\\TimerFrame.png"), WH_SIZE, false);
	m_pSprite2DTimer		->Init(_T("Data\\Texture\\UI\\Timer\\Timer.png"), TIMER_SIZE, false);
	m_pSprite2DTimerArrow	->Init(_T("Data\\Texture\\UI\\Timer\\TimerArrow.png"), TIMER_SIZE, true);
	m_pSprite2DKillNomber	->Init(_T("Data\\Texture\\UI\\KillNum.png"), ICON_SIZE, false);
	m_pSprite2DHitPoint		->Init(_T("Data\\Texture\\UI\\HP.png"), ICON_SIZE, true);

	//画像をアタッチ..
	m_pSpriteTimerFrame	->AttachSprite(m_pSprite2DTimerFrame);
	m_pSpriteTimer		->AttachSprite(m_pSprite2DTimer);
	m_pSpriteTimerArrow	->AttachSprite(m_pSprite2DTimerArrow);
	//HPの分だけアタッチ..
	for (int i = 0; i < HP_MAX; i++)
	{
		m_pSpriteHitPoint[i]->AttachSprite(m_pSprite2DHitPoint);
	}
	//キル数の分だけアタッチ..
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		m_pSpriteKillNomber[i]->AttachSprite(m_pSprite2DKillNomber);
	}
	//プレイヤーの分だけアタッチ..
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		switch (i)
		{
			case 0:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\OneP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
			case 1:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TwoP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
			case 2:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TreeP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
			case 3:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\FourP.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
		default:
			break;
		}
	}

	//地面スプライトの構造体.
	CSprite3D::SPRITE_STATE SSGround;
	SSGround.Disp.w = 1.f;
	SSGround.Disp.h = 1.f;
	SSGround.Base.w = 256.f;
	SSGround.Base.h = 256.f;
	SSGround.Stride.w = 256.f;
	SSGround.Stride.h = 256.f;
	//地面スプライトの読み込み..
	m_pSpriteGround->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Ground.png"), SSGround);

	//プレイヤースプライトの構造体.
	CSprite3D::SPRITE_STATE SSPlayer =
	{ 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };
	//プレイヤースプライトの読み込み
	m_pSpritePlayer->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Player.png"), SSPlayer);

	//爆発スプライトの構造体
	CSprite3D::SPRITE_STATE SSExplosion =
	{ 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };
	//爆発スプライトの読み込み
	m_pSpriteExplosion->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\explosion.png"), SSExplosion);

	//--------------------------------------------------------------------------.
	// 	   メッシュの読み込み..
	//--------------------------------------------------------------------------.
	//スタティックメッシュの読み込み.
	m_pStaticMeshStage->Init(_T("Data\\Mesh\\Static\\Stage\\stage.x"));
	m_pStaticMeshItemBox->Init(_T("Data\\Mesh\\Static\\ItemBox\\ItemBox.x"));

	// 戦車(赤)
	m_pStaticMesh_TankBodyRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Body\\Body.x"));
	m_pStaticMesh_TankCannonRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Cannon\\Cannon.x"));

	// 戦車(黄)
	m_pStaticMesh_TankBodyYellow->Init(_T("Data\\Mesh\\Static\\Tank\\Yellow\\Body\\Body.x"));
	m_pStaticMesh_TankCannonYellow->Init(_T("Data\\Mesh\\Static\\Tank\\Yellow\\Cannon\\Cannon.x"));

	// 戦車(青)
	m_pStaticMesh_TankBodyBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Body\\Body.x"));
	m_pStaticMesh_TankCannonBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Cannon\\Cannon.x"));

	// 戦車(緑)
	m_pStaticMesh_TankBodyGreen->Init(_T("Data\\Mesh\\Static\\Tank\\Green\\Body\\Body.x"));
	m_pStaticMesh_TankCannonGreen->Init(_T("Data\\Mesh\\Static\\Tank\\Green\\Cannon\\Cannon.x"));
	
	// 弾(赤)
	m_pStaticMesh_BulletRed->Init(_T("Data\\Mesh\\Static\\Bullet\\Red\\Ball.x"));
	// 弾(黄)
	m_pStaticMesh_BulletYellow->Init(_T("Data\\Mesh\\Static\\Bullet\\Yellow\\Ball.x"));
	// 弾(青)
	m_pStaticMesh_BulletBlue->Init(_T("Data\\Mesh\\Static\\Bullet\\Blue\\Ball.x"));
	// 弾(緑)
	m_pStaticMesh_BulletGreen->Init(_T("Data\\Mesh\\Static\\Bullet\\Green\\Ball.x"));
	
	//壁
	m_pStaticMeshWallW->Init(_T("Data\\Mesh\\Static\\Wall\\Wall1.x"));
	m_pStaticMeshWallH->Init(_T("Data\\Mesh\\Static\\Wall\\Wall2.x"));

	// 地面
	m_pStaticMeshGround->Init(_T("Data\\Collision\\Ground.x"));

	// 木箱
	m_pStaticMeshWoodBox->Init(_T("Data\\Mesh\\Static\\Block\\Block.x"));

	// バウンディングスフィア(当たり判定用)
	m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Sphere.x"));


	// それぞれのプレイヤーに色にあった戦車をアタッチ
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		switch (i)
		{
		case 0:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyRed, m_pStaticMesh_TankCannonRed);
			break;
		case 1:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyYellow, m_pStaticMesh_TankCannonYellow);
			break;
		case 2:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			break;
		case 3:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
			break;
		default:
			break;
		}
	}

	//弾メッシュ情報を持たせる.
	m_pShotManager->AttachMeshToPlayerShot(BulletKinds::Mesh_1, m_pStaticMesh_BulletRed);
	m_pShotManager->AttachMeshToPlayerShot(BulletKinds::Mesh_2, m_pStaticMesh_BulletYellow);
	m_pShotManager->AttachMeshToPlayerShot(BulletKinds::Mesh_3, m_pStaticMesh_BulletGreen);
	m_pShotManager->AttachMeshToPlayerShot(BulletKinds::Mesh_4, m_pStaticMesh_BulletBlue);

	//スタティックメッシュを設定
	m_pStage->AttachMesh(m_pStaticMeshStage);

	//アイテムボックスマネージャーにメッシュを設定
	m_pItemBoxManager->AttachMesh(m_pStaticMeshItemBox);

	//壁にメッシュを設定
	m_pWallTop->AttachMesh(m_pStaticMeshWallW);
	m_pWallBottom->AttachMesh(m_pStaticMeshWallW);
	m_pWallLeft->AttachMesh(m_pStaticMeshWallH);
	m_pWallRight->AttachMesh(m_pStaticMeshWallH);

	// 木箱にメッシュを設定 
	m_pWoodBoxCenter->AttachMesh(m_pStaticMeshWoodBox);
	m_pWoodBoxTopLeft->AttachMesh(m_pStaticMeshWoodBox);
	m_pWoodBoxTopRight->AttachMesh(m_pStaticMeshWoodBox);
	m_pWoodBoxBottomLeft->AttachMesh(m_pStaticMeshWoodBox);
	m_pWoodBoxBottomRight->AttachMesh(m_pStaticMeshWoodBox);

	// 地面にメッシュを設定
	m_pGround->AttachMesh(m_pStaticMeshGround);

	// バウンディングの作成
	CreateBounding();

	return S_OK;
}

void CGameMain::SetPosition()
{
	// 壁上の位置設定
	m_pWallTop->SetPosition(0, 0, 30);
	m_pWallTop->SetRotation(0, 0, 0);

	// 壁下の位置設定
	m_pWallBottom->SetPosition(0, 0, -30);
	m_pWallBottom->SetRotation(0, 0, 0);

	// 壁左の位置設定
	m_pWallLeft->SetPosition(-30, 0, 0);
	m_pWallLeft->SetRotation(0, 0, 0);

	// 壁右の位置設定
	m_pWallRight->SetPosition(30, 0, 0);
	m_pWallRight->SetRotation(0, 0, 0);


	//-------------------------
	// 木箱の位置設定
	// モデルサイズ X:20, Y:5, Z:20;
	//-------------------------
	// 左上
	m_pWoodBoxTopLeft->SetPosition(-12.0f, -0.4f, 12.0f);
	m_pWoodBoxTopLeft->SetRotation(0.0f, 0.0f, 0.0f);
	m_pWoodBoxTopLeft->SetScale(1.0f, 1.3f, 1.0f);

	// 右上
	m_pWoodBoxTopRight->SetPosition(12.0f, -0.4f, 12.0f);
	m_pWoodBoxTopRight->SetRotation(0.0f, 0.0f, 0.0f);
	m_pWoodBoxTopRight->SetScale(1.0f, 1.3f, 1.0f);

	// 中央
	m_pWoodBoxCenter->SetPosition(0.0f, -0.4f, 0.0f);
	m_pWoodBoxCenter->SetRotation(0.0f, 0.0f, 0.0f);
	m_pWoodBoxCenter->SetScale(1.0f, 1.3f, 1.0f);

	// 左下
	m_pWoodBoxBottomLeft->SetPosition(-12.0f, -0.4f, -12.0f);
	m_pWoodBoxBottomLeft->SetRotation(0.0f, 0.0f, 0.0f);
	m_pWoodBoxBottomLeft->SetScale(1.0f, 1.3f, 1.0f);

	// 右下
	m_pWoodBoxBottomRight->SetPosition(12.0f, -0.4f, -12.0f);
	m_pWoodBoxBottomRight->SetRotation(0.0f, 0.0f, 0.0f);
	m_pWoodBoxBottomRight->SetScale(1.0f, 1.3f, 1.0f);

	// 地面
	m_pGround->SetPosition(0.f, -3.f, 0.f);
	m_pGround->SetRotation(D3DXToRadian(0.f), D3DXToRadian(0.f), D3DXToRadian(0.f));
}

void CGameMain::CreateBounding()
{
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//各プレイヤーの当たり判定作成
		switch (i)
		{
		case 0:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyRed, m_pStaticMesh_TankCannonRed);
			break;
		case 1:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyYellow, m_pStaticMesh_TankCannonYellow);
			break;
		case 2:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
			break;
		case 3:
			m_pPlayerManager->CreateBounding(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			break;
		}
		//各プレイヤーの当たり判定作成.
		m_pPlayerManager->CreateCollider(i);
	}
	
	// 壁の当たり判定生成
	m_pWallTop->CreateBBoxForMesh(*m_pStaticMeshWallW);
	m_pWallBottom->CreateBBoxForMesh(*m_pStaticMeshWallW);
	m_pWallLeft->CreateBBoxForMesh(*m_pStaticMeshWallH);
	m_pWallRight->CreateBBoxForMesh(*m_pStaticMeshWallH);

	// 壁の当たり判定設定
	m_pWallTop->CreateBoxCollider(m_pWallTop->GetMinPos(), m_pWallTop->GetMaxPos());
	m_pWallBottom->CreateBoxCollider(m_pWallBottom->GetMinPos(), m_pWallBottom->GetMaxPos());
	m_pWallLeft->CreateBoxCollider(m_pWallLeft->GetMinPos(), m_pWallLeft->GetMaxPos());
	m_pWallRight->CreateBoxCollider(m_pWallRight->GetMinPos(), m_pWallRight->GetMaxPos());

	// 地面の当たり判定生成
	m_pGround->CreateBBoxForMesh(*m_pStaticMeshGround);
	// 地面の当たり判定設定
	m_pGround->CreateBoxCollider(m_pGround->GetMinPos(), m_pGround->GetMaxPos());

	// 木箱の当たり判定生成
	m_pWoodBoxTopLeft->CreateBBoxForMesh(*m_pStaticMeshWoodBox);
	m_pWoodBoxTopRight->CreateBBoxForMesh(*m_pStaticMeshWoodBox);
	m_pWoodBoxCenter->CreateBBoxForMesh(*m_pStaticMeshWoodBox);
	m_pWoodBoxBottomLeft->CreateBBoxForMesh(*m_pStaticMeshWoodBox);
	m_pWoodBoxBottomRight->CreateBBoxForMesh(*m_pStaticMeshWoodBox);

	// 木箱の当たり判定設定
	m_pWoodBoxTopLeft->CreateBoxCollider(m_pWoodBoxTopLeft->GetMinPos(), m_pWoodBoxTopLeft->GetMaxPos());
	m_pWoodBoxTopRight->CreateBoxCollider(m_pWoodBoxTopRight->GetMinPos(), m_pWoodBoxTopRight->GetMaxPos());
	m_pWoodBoxCenter->CreateBoxCollider(m_pWoodBoxCenter->GetMinPos(), m_pWoodBoxCenter->GetMaxPos());
	m_pWoodBoxBottomLeft->CreateBoxCollider(m_pWoodBoxBottomLeft->GetMinPos(), m_pWoodBoxBottomLeft->GetMaxPos());
	m_pWoodBoxBottomRight->CreateBoxCollider(m_pWoodBoxBottomRight->GetMinPos(), m_pWoodBoxBottomRight->GetMaxPos());

	// アイテムボックスの当たり判定生成
	m_pItemBoxManager->CreateBounding(m_pStaticMeshItemBox);
	// 当たり判定設定
	m_pItemBoxManager->CreateCollider();	

	// 弾の当たり判定生成
	m_pShotManager->CreateBounding(m_pStaticMesh_BulletRed);
	// 当たり判定設定
	m_pShotManager->CreateCollider();

	////爆風の当たり判定生成.
	//m_pBlastManager->CreateBSphereForMesh(m_pStaticMesh_BulletRed);
	////当たり判定設定.
	//m_pBlastManager->CreateSpehreCollider(m_pBlastManager->GetBlastRadius());

}

void CGameMain::Collision()
{
	// 壁とプレイヤーの当たり判定.
	WalltoPlayer();

	// 壁と弾の当たり判定
	WalltoShot();

	// プレイヤーとプレイヤー
	PlayertoPlayer();

	// プレイヤーとアイテム
	PlayertoItemBox();

	// プレイヤーと弾
	PlayertoShot();

	//// 弾と弾
	// このコードやばい(重い).
	//ShottoShot();

	// 箱とプレイヤー
	WoodBoxtoPlayer();

	// 箱と弾
	WoodBoxtoShot();

	// 地面と弾
	GroundtoShot();

	// 地面とアイテムボックス
	GroundtoItemBox();

	//  プレイヤーと爆風.
	//PlayertoBlast();
}

void CGameMain::WalltoPlayer()
{
	// 押し返しの強さ
	const float pushStrength = 0.1f; 

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// i 番のプレイヤーを取得
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		// 押し返すための変数
		D3DXVECTOR3 push(0.0f, 0.0f, 0.0f);

		// 車体が壁と接触したとき
		if (Coll->CheckCollision(*m_pWallTop->GetCollider()))
		{
			push.z -= 0.1f;
		}
		if (Coll->CheckCollision(*m_pWallBottom->GetCollider()))
		{
			push.z += 0.1f;
		}
		if (Coll->CheckCollision(*m_pWallLeft->GetCollider()))
		{
			push.x += 0.1f;
		}
		if (Coll->CheckCollision(*m_pWallRight->GetCollider()))
		{
			push.x -= 0.1f;
		}

		// 押し返しを正規化
		if (D3DXVec3Length(&push) > 0.f)
		{
			D3DXVec3Normalize(&push, &push);
			push *= pushStrength;
		}

		// 壁に当たった時に押し返す
		player->GetBody()->PushBack(push);
		player->GetCannon()->PushBack(push);
	}
}

void CGameMain::WalltoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットのコライダー取得
		auto Shots = m_pShotManager->GetShot();
		auto ShotsColl = Shots[i]->GetCollider();


		// 壁が弾と接触したとき
		if (ShotsColl->CheckCollision(*m_pWallTop->GetCollider()))
		{			
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);
			
			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWallBottom->GetCollider()))
		{
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWallLeft->GetCollider()))
		{
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWallRight->GetCollider()))
		{
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
	}
}

void CGameMain::PlayertoPlayer()
{
	const float pushStrength = 0.1f;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// プレイヤーAのコライダー取得
		auto playerA = m_pPlayerManager->GetControlPlayer(i);
		auto CollA = playerA->GetBody()->GetCollider();

		for (int j = 0; j < PLAYER_MAX; j++)
		{ 
			// 自分自身との判定をスキップ
			if (i == j) continue; 

			// プレイヤーBのコライダー取得
			auto playerB = m_pPlayerManager->GetControlPlayer(j);
			auto CollB = playerB->GetBody()->GetCollider();

			if (CollA->CheckCollision(*CollB))
			{
				// 衝突時の押し返し処理例
				D3DXVECTOR3 push = playerA->GetBody()->GetPosition() - playerB->GetBody()->GetPosition();

				// pushベクトルを正規化して押し返しの強さをかける
				float length = D3DXVec3Length(&push);
				if (length > 0.0001f)
				{
					push /= length;
					push *= pushStrength;
					playerA->GetBody()->PushBack(push);
				}
			}
		}
	}
}

// アイテムボックスの当たり判定
void CGameMain::PlayertoItemBox()
{
	for (int PlayerIndex = 0; PlayerIndex < PLAYER_MAX; ++PlayerIndex)
	{
		// i 番のプレイヤーを取得
		auto player = m_pPlayerManager->GetControlPlayer(PlayerIndex);
		auto Coll = player->GetBody()->GetCollider();

		for (int ItemIndex = 0; ItemIndex < ITEM_MAX; ++ItemIndex)
		{
			auto Item = m_pItemBoxManager->GetItem();
			auto ItemColl = Item[ItemIndex]->GetCollider();

			// プレイヤーがアイテムと接触したとき
			if (Coll->CheckCollision(*ItemColl))
			{
				Item[ItemIndex]->HitPlayer();
			}
		}
	}
}

void CGameMain::PlayertoShot()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// プレイヤーのコライダー取得
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		for (int i = 0; i < ShotMax; i++)
		{
			// ショットのコライダー取得
			auto Shots = m_pShotManager->GetShot();
			auto ShotsColl = Shots[i]->GetCollider();

			if (ShotsColl->CheckCollision(*Coll))
			{
				////動的に作成.
				//m_pBlastManager->Create();
				////爆風のメッシュを設定.
				//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
				////爆風の情報.
				//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
				//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
				//m_pBlastManager->SetBlastFlag(true);

				Shots[i]->HitShot();
			}
		}
	}
}

void CGameMain::ShottoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットAのコライダー取得
		auto ShotsA = m_pShotManager->GetShot();
		auto ShotsCollA = ShotsA[i]->GetCollider();

		for (int j = 0; j < ShotMax; j++)
		{
			// 自分自身との判定をスキップ
			if (i == j) continue;

			// ショットBのコライダー取得
			auto ShotsB = m_pShotManager->GetShot();
			auto ShotsCollB = ShotsB[i]->GetCollider();

			if (ShotsCollA->CheckCollision(*ShotsCollB))
			{
				ShotsA[i]->HitShot();
				ShotsB[i]->HitShot();
			}
		}
	}
}

//爆風とプレイヤーの当たり判定.
void CGameMain::PlayertoBlast()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//i番目のプレイヤーを取得.
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		//if (m_pBlastManager->GetBlastFlag() == true)
		//{
		//	////車体が爆風と接触したとき.
		//	if (Coll->CheckCollision(*m_pBlastManager->GetCollider()))
		//	{
		//		m_pBlastManager->HitBlast();
		//	}
		//}
	}
}

void CGameMain::WoodBoxtoPlayer()
{
	// 押し返しの強さ
	const float pushStrength = 0.1f;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// i 番のプレイヤーを取得
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();

		// 押し返すための変数
		D3DXVECTOR3 push(0.0f, 0.0f, 0.0f);

		// 車体が壁と接触したとき
		// 左上
		if (Coll->CheckCollision(*m_pWoodBoxTopLeft->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxTopLeft->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 右上
		if (Coll->CheckCollision(*m_pWoodBoxTopRight->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxTopRight->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 中央
		if (Coll->CheckCollision(*m_pWoodBoxCenter->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxCenter->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 左下
		if (Coll->CheckCollision(*m_pWoodBoxBottomLeft->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxBottomLeft->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}
		// 右下
		if (Coll->CheckCollision(*m_pWoodBoxBottomRight->GetCollider()))
		{
			// 衝突時の押し返し処理例
			D3DXVECTOR3 push = player->GetBody()->GetPosition() - m_pWoodBoxBottomRight->GetPosition();

			// pushベクトルを正規化して押し返しの強さをかける
			float length = D3DXVec3Length(&push);
			if (length > 0.0001f)
			{
				push /= length;
				push *= pushStrength;
				player->GetBody()->PushBack(push);
			}
		}

		// 押し返しを正規化
		if (D3DXVec3Length(&push) > 0.f)
		{
			D3DXVec3Normalize(&push, &push);
			push *= pushStrength;
		}

		// 壁に当たった時に押し返す
		player->GetBody()->PushBack(push);
		player->GetCannon()->PushBack(push);
	}
}

void CGameMain::WoodBoxtoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットのコライダー取得
		auto Shots = m_pShotManager->GetShot();
		auto ShotsColl = Shots[i]->GetCollider();

		// 壁が弾と接触したとき
		if (ShotsColl->CheckCollision(*m_pWoodBoxTopLeft->GetCollider()))
		{
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxTopRight->GetCollider()))
		{
			//// 動的に作成
			//m_pBlastManager->Create();
			//// 爆風のメッシュを設定
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			//// 爆風の情報
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxCenter->GetCollider()))
		{
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxBottomLeft->GetCollider()))
		{
			////動的に作成
			//m_pBlastManager->Create();
			////爆風のメッシュを設定
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
		if (ShotsColl->CheckCollision(*m_pWoodBoxBottomRight->GetCollider()))
		{
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
	}
}

void CGameMain::GroundtoShot()
{
	for (int i = 0; i < ShotMax; i++)
	{
		// ショットのコライダー取得
		auto Shots = m_pShotManager->GetShot();
		auto ShotsColl = Shots[i]->GetCollider();

		if (ShotsColl->CheckCollision(*m_pGround->GetCollider()))
		{
			////動的に作成.
			//m_pBlastManager->Create();
			////爆風のメッシュを設定.
			//m_pBlastManager->AttachMesh(m_pStaticMesh_BulletRed);
			////爆風の情報.
			//m_pBlastManager->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->GetCollider()->SetPosition(Shots[i]->GetPosition());
			//m_pBlastManager->SetBlastFlag(true);

			Shots[i]->HitShot();
		}
	}
}

void CGameMain::GroundtoItemBox()
{
	for (int ItemIndex = 0; ItemIndex < ITEM_MAX; ++ItemIndex)
	{
		auto Item = m_pItemBoxManager->GetItem();
		auto ItemColl = Item[ItemIndex]->GetCollider();

		if (ItemColl->CheckCollision(*m_pGround->GetCollider()))
		{
			// アイテムボックスの処理を入れる
			Item[ItemIndex]->SetGravity(true);
		}
	}
}

//画面をグリッドに分割したとき、idx番目のマスに対応する.
//D3D11_VIEWPORTを作成して返す関数.
D3D11_VIEWPORT CGameMain::MakeGridViewport(int idx, int cols, int rows, float totalW, float totalH)
{
	//どのマスか、列・行を算出.
	const int col = idx % cols;	//列番号.
	const int row = idx / cols;	//行番号.

	//ビューポートを作成.
	D3D11_VIEWPORT vp{};					//ゼロ初期化.
	vp.TopLeftX = (totalW / cols) * col;	//左上(0,0)からのオフセット.
	vp.TopLeftY = (totalH / rows) * row;
	vp.Width = totalW / cols;
	vp.Height = totalH / rows;
	vp.MinDepth = 0.0f;						//深度バッファの最小値設定.
	vp.MaxDepth = 1.0f;						//深度バッファの最大値設定.
	return vp;
}


CSceneType CGameMain::GetSceneType() const
{
	return m_SceneType;
}


//-----各設定を関数化-----..

//制限時間画像の設定..
void CGameMain::EachSettingTimer()
{
	//定数宣言..
	static constexpr int TIME = 90.0;

	//-----中心表示用座標-----..
		//制限時間枠の画像設定..
	m_pSpriteTimerFrame->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
	//制限時間円の画像設定..
	m_pSpriteTimer->SetPosition(WND_W / 2.f - 74.f, WND_H / 2 - 32.f, 0.f);
	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);
	//時計の針の画像設定..
	m_pSpriteTimerArrow->SetPosition(WND_W / 2.f - 42.f, WND_H / 2, 0.f);
	m_pSpriteTimerArrow->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerArrow->SetScale(0.25f, 0.25f, 0.f);

	//-----中心表示用座標-----..
	//ゲームで遊べる(クリア画面に遷移する)時間※引数..
	m_Timer->StartTimer(TIME);
	m_Timer->SetDebugFont(m_pDbgText);
	m_Timer->SetTimerPosition(WND_W / 2 - 15.f, WND_H / 2 - 30.f);

	////-----中間発表用-----..
	//	//ゲームで遊べる(クリア画面に遷移する)時間※引数..
	//	m_Timer->StartTimer(TIME);.
	//	m_Timer->SetDebugFont(m_pDbgText);.
	//	m_Timer->SetTimerPosition(WND_W - 96.f, WND_H - 96.f);.

}
//プレイヤー番号画像の設定..
void CGameMain::EachSettingPlayerNumber()
{
	//-----中間発表用-----..
		//プレイヤー番号の画像の設定..
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//プレイヤー1P..
		case 2:			//プレイヤー3P..
			m_pSpritePlayerIcon[i]->SetPosition(0.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		case 1:			//プレイヤー2P..
		case 3:			//プレイヤー4P..
			m_pSpritePlayerIcon[i]->SetPosition(WND_W - 256.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		default:
			break;
		}
	}
}
//倒した数画像の設定..
void CGameMain::EachSettingKillNumber()
{
	////-----中心表示用座標-----..
	//	//..
	//	m_pSpriteKillNomber->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);.
	//	m_pSpriteKillNomber->SetRotation(0.f, 0.f, 0.f);.
	//	m_pSpriteKillNomber->SetScale(1.f, 1.f, 0.f);.

	//-----中間発表用-----..
		//プレイヤー番号の画像の設定..
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//プレイヤー1P..
		case 2:			//プレイヤー3P..
			m_pSpriteKillNomber[i]->SetPosition(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		case 1:			//プレイヤー2P..
		case 3:			//プレイヤー4P..
			m_pSpriteKillNomber[i]->SetPosition(WND_W - 320.f, 0.f, 0.f);//320:256サイズの画像に文字分64を足した数..
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		default:
			break;
		}
	}
}
//倒した数画像の設定..
void CGameMain::EachSettingHitPoint()
{
	//-----4画面用-----..
		//HPの画像の設定..
	for (int i = 0; i < HP_MAX; i++)
	{
		if (i <= 0)
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2 - 128.f, 64.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
		else
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2, 64.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
	}
}