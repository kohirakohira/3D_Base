#define GetKey(KEY) (GetAsyncKeyState(KEY))
#undef min	//マクロ定義無効化.
#undef max	

static bool prevC = false;
static bool prevA = false;
#include "CGameMain.h"
//-----サウンド-----.
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス.

//-----DirectX-----.
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9クラス.
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス.

#include "GameObject/StaticMeshObject/Character/CharacterObject/CCharacterObject.h"

//定数宣言.
static constexpr int TIME = 120;		//時間は調整してください.
const float deltaTime = 1.0f / FPS;
const float DIALMETER = 360.0f;			//時計の回転する針に使用.
const float FLASH_TIME = 0.5f;			//点滅周期.
const float dt = 1.0f / FPS;

CGameMain::CGameMain(HWND hWnd)
	: m_hWnd						( hWnd )

	// 画像
	, m_pSprite2DTimerArrow			( nullptr )
	, m_pSprite2DTimerFrame			( nullptr )
	, m_pSprite2DTimer				( nullptr )
	, m_pSprite2DKillNomber			( nullptr )
	, m_pSprite2DHitPoint			( nullptr )
	, m_pSprite2DPlayerIcon			()
	, m_pSprite2DNumber				( nullptr )

	// 画像の設定
	, m_pSpriteTimerFrame			()
	, m_pSpriteTimer				()
	, m_pSpriteTimerArrow			()
	, m_pSpritePlayerIcon			()
	, m_pSpriteKillNomber			()
	, m_pSpriteHitPoint				()
	, m_TimerNumber					()
	, m_KillCountNumber				()

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

	// 木箱のメッシュ
	, m_pStaticMeshWoodBox			( nullptr )
	, m_pStaticMeshBackImg			( nullptr )
	, m_pBackImgObject				( nullptr )
	
	, m_pCharacterManager			()
	, m_pShotManager				()

	, m_pStage						( nullptr )
	, m_pDbgText					( nullptr )
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
	, m_pItemBoxManager				( nullptr )
	, m_pBlastManager				( nullptr )

	, m_Rot							( 0.0f )
	, m_pCollisionManager			( nullptr )
	, m_Flashing					( false )
	, m_FlashingTime				( 0.0f )

	, m_EffectTime					( 0.0f )
	, hEffect_Player_Smoke			()
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
	//色の設定.
	Effekseer::Color color = { 255, 255, 255, 255 };

	//BGMのループ再生..
	CSoundManager::PlayLoop(CSoundManager::BGM_Main);

	//コントローラーの更新.
	CControllerManager::GetInstance().Update();
	CController* controller = CControllerManager::GetInstance().GetController(0);

#ifdef ENABLE_ITEMS
	//アイテムの動作..
	m_pItemBoxManager->Update();
	// アイテムボックスマネージャーをセット
	m_pCollisionManager->SetCItemBoxManager(m_pItemBoxManager);
#endif//#ifdef ENABLE_ITEMS

	//爆風の更新処理.
	m_pBlastManager->Update();

//-----メイン演出用-----..
	
	//Iconを回転させる..
	m_Rot += 1.0f * deltaTime;
	if (m_Rot >= DIALMETER)
	{
		m_Rot = 0.0f;
	}
	
	//赤の点滅スタート.
	if (m_Timer->GetRemainingTime() <= 10.0f)
	{
		m_Flashing = true;
	}
	if (m_Flashing == true)
	{
		m_FlashingTime += deltaTime;
	}
	else
	{
		m_FlashingTime = 0.0f;
	}

//-----メイン演出用-----..

	//プレイヤー全員更新.
	m_pCharacterManager->Update();
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		m_pCharacterManager->PlayerRespawn(i);
	}

#if 0
	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		m_pCharacterManager->GetControlPlayer(index)->GetCannon()->SetShotManager(m_pShotManager);
	}
#endif
	m_pShotManager->Update();

#if 1
	//カメラ追従＆更新.砲塔基準
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (auto chara = m_pCharacterManager->GetControlPlayer(i))
		{
			const D3DXVECTOR3 camPos = chara->GetCannon()->GetPosition();	//砲塔の位置.
			float yaw = chara->GetCannon()->GetRotation().y;	//砲塔の向きY.

			if (GetAsyncKeyState(VK_F1) & 0x8000)
			{
				m_pCameras[i]->SetCameraMode(CCamera::CameraMode::Free);
			}
			if (GetAsyncKeyState(VK_F2) & 0x8000)
			{
				m_pCameras[i]->SetCameraMode(CCamera::CameraMode::ThirdPerson);
			}

			m_pCameras[i]->SetTargetPos(camPos);
			m_pCameras[i]->SetTargetRotY(yaw);
		}
		m_pCameras[i]->Update();
	}
#endif

	//定数宣言.
	const float PI = 3.14159265358979f;
	//時計の針の回転.
	float remaining = m_Timer->GetRemainingTime();
	float progress = 1.0f - (remaining / TIME);
	float angle = progress * 360.0f;
	m_pSpriteTimerArrow->SetRotation(0.f, D3DXToRadian(180.0f), D3DXToRadian(angle));

	// 壁の更新
	m_pWallTop->Update();
	m_pWallBottom->Update();
	m_pWallLeft->Update();
	m_pWallRight->Update();

	// 木箱の更新
	m_pWoodBoxTopLeft->Update();
	m_pWoodBoxTopRight->Update();
	m_pWoodBoxCenter->Update();
	m_pWoodBoxBottomLeft->Update();
	m_pWoodBoxBottomRight->Update();

	// 地面の更新
	m_pGround->Update();

	// 当たり判定の更新
	m_pCollisionManager->Update();

	//制限時間の設定.
	m_TimerNumber->SetNumber(m_Timer->GetRemainingTime(), 3);
	m_TimerNumber->Update();

	//キル数の更新.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		m_KillCountNumber[i]->Update();
	}

	//エフェクト(車体の煙)の動き.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		auto player = m_pCharacterManager->GetControlPlayer(i);
		//例外処理.
		if (player == nullptr)
		{
			continue;
		}
		//位置・回転.
		D3DXVECTOR3 pos = { m_pCharacterManager->GetPosition(i).x,
							m_pCharacterManager->GetPosition(i).y,
							m_pCharacterManager->GetPosition(i).z};
		D3DXVECTOR3 rot = { D3DXToRadian(-45.0f), m_pCharacterManager->GetRotation(i).y, D3DXToRadian(45.0f) };

		//体力が1になった瞬間.
		if (player->GetHP() == 1 && hEffect_Player_Smoke[i] == -1)
		{
			//エフェクト.
			hEffect_Player_Smoke[i] = CEffect::GetInstance().Play(CEffect::Smoke, pos);
			//エフェクトのサイズを設定.
			CEffect::GetInstance().SetScale(hEffect_Player_Smoke[i], D3DXVECTOR3{ 0.1f, 0.1f, 0.1f });
			CEffect::GetInstance().SetAlpha(hEffect_Player_Smoke[i], color);
		}
		//エフェクトの位置を設定.
		CEffect::GetInstance().SetLocation(hEffect_Player_Smoke[i], pos);
		//エフェクトの回転を設定.
		CEffect::GetInstance().SetRotation(hEffect_Player_Smoke[i], rot);

		//死亡した時.
		if (player->GetHP() <= 0)
		{
			CEffect::GetInstance().Stop(hEffect_Player_Smoke[i]);
			hEffect_Player_Smoke[i] = -1;
		}
	}

	//勝ち..
	//敗北..
	{
		if (m_Timer->GetRemainingTime() <= 0.0f)
		{
			//BGMのループ停止..
			CSoundManager::Stop(CSoundManager::BGM_Main);

			//SEのループ停止
			CSoundManager::Stop(CSoundManager::SE_Move1);
			CSoundManager::Stop(CSoundManager::SE_Move2);
			CSoundManager::Stop(CSoundManager::SE_Move3);
			CSoundManager::Stop(CSoundManager::SE_Move4);

			// 戦車が動いている時のSEを停止

			m_SceneType = CSceneType::Result;
		}
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
	auto DrawOneViewport = [&](std::shared_ptr<CCamera> camera, std::shared_ptr<CCharacterObjectBase> owner)
	{
		//カメラ更新.
		camera->Update();
		auto Camera = camera->GetCameraPosition();
		//ライト設定
		camera->SetLightPos(0.f, 100.f, 0.f);			//ライトのポジション
        camera->SetLightColor(1.f, 1.f, 1.f);			//色は通常
        camera->SetLightIntensity(300.f);				//ライトの明るさ
        camera->SetLightRange(1e18);					//影響半径
        camera->SetLightAtten(1e18, 1e18, 1e18);		//kc,kl,kq
		//スナップショットをconst参照でキャプチャ.
		D3DXMATRIX& view	= camera->m_mView;
		D3DXMATRIX& proj	= camera->m_mProj;
		LIGHT&		light	= camera->m_Light;
		CAMERA&		paramC	= camera->m_Camera;

		//プレイヤーを描画.ここで全員描く.
		for (int players = 0; players < PLAYER_MAX; ++players)
		{
			if (auto p = m_pCharacterManager->GetControlPlayer(players))
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


#ifdef ENABLE_ITEMS

		//アイテムボックスの描画.
		m_pItemBoxManager->Draw(view, proj, light, paramC);

#endif//#ifdef ENABLE_ITEMS

		////爆風の描画.
		//m_pBlastManager->Draw(view, proj, light, paramC);

		//背景の表示.
		m_pBackImgObject->Draw(view, proj, light, paramC);

		//エフェクトもここでやる.
		CEffect::GetInstance().Draw(view, proj, light, paramC);

	};
	//分割ビューのループ.
	for (int i = 0; i < VIEWS; ++i)
	{
		//ビューポート設定.
		const D3D11_VIEWPORT vp = MakeGridViewport(i, COLS, ROWS, W, H);
		pContext->RSSetViewports(1, &vp);

		//カメラ参照を取得.参照外しで実体を直接扱う.
		std::shared_ptr<CCamera> camera = m_pCameras[i];

		std::shared_ptr<CCharacterObjectBase> owner = m_pCharacterManager->GetControlPlayer(i);

		//1ビュー分を描画.
		DrawOneViewport(camera, owner);

		//前後関係無視.
		//UI.
		CDirectX11::GetInstance().SetDepth(false);
		//プレイヤー番号の描画..
		m_pSpritePlayerIcon[i]->Draw();
		m_pSpriteKillNomber[i]->Draw();

		//HPの画像の設定..
		EachSettingHitPoint(i);
		int chara_hp = m_pCharacterManager->GetControlPlayer(i)->GetHP();
		//各プレイヤーの体力分回す.
		for (int hp = 0; hp < chara_hp; hp++)
		{
			if (m_pSpriteHitPoint[i][hp]->GetIsDisplay() == true)
			{
				m_pSpriteHitPoint[i][hp]->SetRotation(0.0f, 0.0f, m_Rot);
				m_pSpriteHitPoint[i][hp]->Draw();
			}
		}

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

	//時間を割った時の余りを計算.
	float mod = fmod(m_FlashingTime, FLASH_TIME);

	//赤色に点滅するか判定.
	if (mod < FLASH_TIME * 0.5f)
	{
		if (m_Flashing == true)
		{
			//赤色の処理.
			m_pSpriteTimerFrame->SetColor(D3DXVECTOR4{ 1.0f, 0.0f, 0.0f, 1.0f });
		}
	}
	else
	{
		//通常色の処理.
		m_pSpriteTimerFrame->SetColor(D3DXVECTOR4{ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	//1画面の時の表示.
	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//タイマーの枠の描画.
	m_pSpriteTimerFrame->Draw();
	//タイマーの描画.
	m_pSpriteTimer->Draw();
	//タイマーの描画.
	m_pSpriteTimerArrow->Draw();
	//タイマー描画.
	//m_Timer->Draw();
	m_TimerNumber->Draw();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//キル数の表示.
		m_KillCountNumber[i]->SetNumber(CGameDataManager::GetInstance().GetKillCount(i), 2);
		m_KillCountNumber[i]->Draw();
	}

	CDirectX11::GetInstance().SetDepth(true);
	
}


void CGameMain::Init()
{
	//キャラにShotManagerを設定.
	m_pCharacterManager->SetShotManager(m_pShotManager);

	////キャラの情報を設定.
	//for (size_t i = 0; i < m_pCharacterManager->GetControlPlayer(); ++i)
	//{
	//	TankTuning info = { 0.08f, 0.03f, 0.03f, 0.3f };
	//	m_pCharacterManager->SetPlayerTuning(i, info);
	//}

	//カメラ位置設定.
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//キャラクターマネージャーから各プレイヤーの位置を取得.
		D3DXVECTOR3 pos = m_pCharacterManager->GetPosition(i);

		m_pCameras[i]->SetCameraPos(pos.x, pos.y, pos.z);
		m_pCameras[i]->SetLightPos(0.f, 2.f, 5.f);
	}
	//地面の大きさ設定..
	m_pStage->SetRotation(0.0f, 0.0f, 0.0f);
	m_pStage->SetScale(0.4f, 0.4f, 0.4f);

//-----UI系統の初期化-----.
	//時計の枠.
	m_pSpriteTimerFrame->SetPosition(0.0f, 0.0f, 0.0f);
	m_pSpriteTimerFrame->SetRotation(0.0f, 0.0f, 0.0f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);

	//時計本体.
	m_pSpriteTimer->SetPosition(WND_W / 2.0f - 74.0f, WND_H / 2 - 32.f, 0.0f);
	m_pSpriteTimer->SetRotation(0.0f, 0.0f, 0.0f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.0f);

	//キル数の情報.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		switch (i)
		{
		case 0 :
			m_KillCountNumber[i]->SetBasePosition(D3DXVECTOR2{ 90.0f, 35.0f });
			m_KillCountNumber[i]->SetDigitWidth(32);
			break;
		case 1 :
			m_KillCountNumber[i]->SetBasePosition(D3DXVECTOR2{ WND_W - 70.0f, 35.0f });
			m_KillCountNumber[i]->SetDigitWidth(32);
			break;
		case 2 :
			m_KillCountNumber[i]->SetBasePosition(D3DXVECTOR2{ 90.0f, WND_H - 500.0f });
			m_KillCountNumber[i]->SetDigitWidth(32);
			break;
		case 3 :
			m_KillCountNumber[i]->SetBasePosition(D3DXVECTOR2{ WND_W - 70.0f, WND_H - 500.0f });
			m_KillCountNumber[i]->SetDigitWidth(32);
			break;
		default:
			break;
		}
	}

	//制限時間画像の設定..
	EachSettingTimer();
	//プレイヤー番号画像の設定..
	EachSettingPlayerNumber();
	//倒した数画像の設定..
	EachSettingKillNumber();

	//制限時間の文字サイズ..
	m_pDbgText->SetFontSize(5.0f);

	//カメラがめり込んだ時のオブジェクト設定.
	m_pBackImgObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pBackImgObject->SetScale(7.5f, 7.5f, 7.5f);
	m_pBackImgObject->SetRotation(0.0f, 0.0f, 0.0f);

	SetPosition();

	//エフェクトの初期化.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		hEffect_Player_Smoke[i] = -1;
	}

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
	m_TimerNumber		= std::make_shared<NumberImage>();
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		m_KillCountNumber[i] = std::make_shared<NumberImage>();
	}

	//HPの分だけ生成..
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		for (int hp = 0; hp < HP_MAX; hp++)
		{
			m_pSpriteHitPoint[i][hp] = std::make_shared<CUIObject>();
		}
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
	m_pSprite2DNumber		= std::make_shared<CSprite2D>();
	//プレイヤーの分だけ生成..
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		m_pSprite2DPlayerIcon[i] = std::make_shared<CSprite2D>();
	}

	//スプライトのインスタンス作成..
	m_pSpriteGround		= std::make_unique<CSprite3D>();
	m_pSpritePlayer		= std::make_unique<CSprite3D>();
	m_pSpriteExplosion	= std::make_shared<CSprite3D>();

	//壁を外から見たときのオブジェクトのインスタンス作成.
	m_pBackImgObject = std::make_unique<CStaticMeshObject>();

	//スタティックメッシュのインスタンス作成.
	m_pStaticMeshStage		= std::make_shared<CStaticMesh>();
	m_pStaticMeshBSphere	= std::make_shared<CStaticMesh>();
	m_pStaticMeshItemBox	= std::make_shared<CStaticMesh>();

	// 戦車のメッシュ
	m_pStaticMesh_TankBodyRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonBlue	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyGreen		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonGreen	= std::make_shared<CStaticMesh>();

	// 弾のメッシュ
	m_pStaticMesh_BulletRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletBlue	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletGreen	= std::make_shared<CStaticMesh>();

	//壁のメッシュ
	m_pStaticMeshWallW = std::make_shared<CStaticMesh>();
	m_pStaticMeshWallH = std::make_shared<CStaticMesh>();

	// 地面のメッシュ
	m_pStaticMeshGround = std::make_shared<CStaticMesh>();

	// 木箱のメッシュ
	m_pStaticMeshWoodBox = std::make_shared<CStaticMesh>();

	// 背景画像のメッシュ
	m_pStaticMeshBackImg = std::make_shared<CStaticMesh>();

	// デバッグテキストのインスタンス作成
	m_pDbgText = std::make_unique<CDebugText>();

	//プレイヤーと砲塔のインスタンス生成.
	m_pCharacterManager = std::make_shared<CCharacterManager>();

	//マネージャーは一回だけ初期化
	m_pCharacterManager->Init();

	//弾クラスのインスタンス作成.
	m_pShotManager = std::make_shared<CShotManager>();
	//m_pShotManager->Initialize();

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//カメラ生成・セットアップ.
		auto camera = std::make_unique<CCamera>();
		camera->SetTargetPos(m_pCharacterManager->GetPosition(i));
		camera->SetTargetRotY(m_pCharacterManager->GetRotation(i).y);
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

	//アイテムマネージャークラスのインスタンス生成..
	m_pWoodBoxTopLeft		= std::make_shared<CStageObject>();
	m_pWoodBoxTopRight		= std::make_shared<CStageObject>();
	m_pWoodBoxCenter		= std::make_shared<CStageObject>();
	m_pWoodBoxBottomLeft	= std::make_shared<CStageObject>();
	m_pWoodBoxBottomRight	= std::make_shared<CStageObject>();

#ifdef ENABLE_ITEMS

	// アイテムマネージャークラスのインスタンス生成
	m_pItemBoxManager = std::make_shared<CItemBoxManager>();

#endif//#ifdef ENABLE_ITEMS

	// 当たり判定マネージャークラスのインスタンス生成
	m_pCollisionManager = std::make_shared<CCollisionManager>();

	//爆風マネージャークラスのインスタンス生成.
	m_pBlastManager = std::make_shared<CBlastManager>();

	//COMの障害物判定
	BuildComObstacles();

	//m_pCharacterManager->SetShotManager(m_pShotManager);
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

	//タイマー画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		1920, 1080,		//描画幅,高さ..
		1920, 1080,		//元画像の幅,高さ..
		1920, 1080		//アニメーションをしないので、0でいい..
	};
	//タイマー枠画像のスプライト設定
	CSprite2D::SPRITE_STATE TIMER_SIZE = {
		257, 257,		//描画幅,高さ..
		257, 257,		//元画像の幅,高さ..
		257, 257		//アニメーションをしないので、0でいい..
	};
	//タイマー枠画像のスプライト設定
	CSprite2D::SPRITE_STATE ICON_SIZE = {
		256, 256,		//描画幅,高さ..
		256, 256,		//元画像の幅,高さ..
		256, 256		//アニメーションをしないので、0でいい..
	};
	//数字画像のスプライト設定
	CSprite2D::SPRITE_STATE NUM_SIZE = {
		33, 32,			//描画幅,高さ..
		330, 32,		//元画像の幅,高さ..
		33, 32			//アニメーションをしないので、0でいい..
	};

	//制限時間の枠の読み込み
	m_pSprite2DTimerFrame->Init(_T("Data\\Texture\\UI\\Timer\\TimerFrame.png"), WH_SIZE, false);
	m_pSprite2DTimer->Init(_T("Data\\Texture\\UI\\Timer\\Timer.png"), TIMER_SIZE, false);
	m_pSprite2DTimerArrow->Init(_T("Data\\Texture\\UI\\Timer\\TimerArrow.png"), TIMER_SIZE, true);
	m_pSprite2DKillNomber->Init(_T("Data\\Texture\\UI\\KillNum_remake.png"), ICON_SIZE, false);
	m_pSprite2DHitPoint->Init(_T("Data\\Texture\\UI\\Gear.png"), ICON_SIZE, true);
	m_pSprite2DNumber->Init(_T("Data\\Texture\\UI\\Timer\\number.png"), NUM_SIZE, false);

	//画像をアタッチ
	m_pSpriteTimerFrame	->AttachSprite(m_pSprite2DTimerFrame);
	m_pSpriteTimer		->AttachSprite(m_pSprite2DTimer);
	m_pSpriteTimerArrow	->AttachSprite(m_pSprite2DTimerArrow);
	m_TimerNumber		->AttachSprite(m_pSprite2DNumber);
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		m_KillCountNumber[i]->AttachSprite(m_pSprite2DNumber);
	}

	//HPの分だけアタッチ
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		for (int hp = 0; hp < HP_MAX; hp++)
		{
			m_pSpriteHitPoint[i][hp]->AttachSprite(m_pSprite2DHitPoint);
		}
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
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\OneP_ver2.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
		case 1:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TwoP_ver2.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
		case 2:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\TreeP_ver2.png"), ICON_SIZE, false);
			m_pSpritePlayerIcon[i]->AttachSprite(m_pSprite2DPlayerIcon[i]);
			break;
		case 3:
			m_pSprite2DPlayerIcon[i]->Init(_T("Data\\Texture\\UI\\PlayerNumber\\FourP_ver2.png"), ICON_SIZE, false);
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

	// 戦車.
	m_pStaticMesh_TankBodyRed->Init(		_T("Data\\Mesh\\Static\\Tank_n\\Red\\Body.x"));
	m_pStaticMesh_TankCannonRed->Init(		_T("Data\\Mesh\\Static\\Tank_n\\Red\\Cannon.x"));
	m_pStaticMesh_TankBodyYellow->Init(		_T("Data\\Mesh\\Static\\Tank_n\\Yellow\\Body.x"));
	m_pStaticMesh_TankCannonYellow->Init(	_T("Data\\Mesh\\Static\\Tank_n\\Yellow\\Cannon.x"));
	m_pStaticMesh_TankBodyBlue->Init(		_T("Data\\Mesh\\Static\\Tank_n\\Blue\\Body.x"));
	m_pStaticMesh_TankCannonBlue->Init(		_T("Data\\Mesh\\Static\\Tank_n\\Blue\\Cannon.x"));
	m_pStaticMesh_TankBodyGreen->Init(		_T("Data\\Mesh\\Static\\Tank_n\\Green\\Body.x"));
	m_pStaticMesh_TankCannonGreen->Init(	_T("Data\\Mesh\\Static\\Tank_n\\Green\\Cannon.x"));
	
	// 弾(赤)
	m_pStaticMesh_BulletRed->Init(_T("Data\\Mesh\\Static\\Bullet\\Red\\Ball.x"));
	// 弾(黄)
	m_pStaticMesh_BulletYellow->Init(_T("Data\\Mesh\\Static\\Bullet\\Yellow\\Ball.x"));
	// 弾(青)
	m_pStaticMesh_BulletBlue->Init(_T("Data\\Mesh\\Static\\Bullet\\Blue\\Ball.x"));
	// 弾(緑)
	m_pStaticMesh_BulletGreen->Init(_T("Data\\Mesh\\Static\\Bullet\\Green\\Ball.x"));

	//壁
	m_pStaticMeshWallW->Init(_T("Data\\Collision\\Wall1.x"));
	m_pStaticMeshWallH->Init(_T("Data\\Collision\\Wall2.x"));

	// 地面
	m_pStaticMeshGround->Init(_T("Data\\Collision\\Ground.x"));

	// 木箱
	m_pStaticMeshWoodBox->Init(_T("Data\\Mesh\\Static\\Block\\Block.x"));

	//壁の外側の背景画像メッシュ.
	m_pStaticMeshBackImg->Init(_T("Data\\Mesh\\Static\\OutBackImage\\BackImage.x"));

	// バウンディングスフィア(当たり判定用)
	m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Sphere.x"));


	// それぞれのプレイヤーに色にあった戦車をアタッチ
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		switch (i)
		{
		case 0:
			m_pCharacterManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyRed, m_pStaticMesh_TankCannonRed);
			break;
		case 1:
			m_pCharacterManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyYellow, m_pStaticMesh_TankCannonYellow);
			break;
		case 2:
			m_pCharacterManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			break;
		case 3:
			m_pCharacterManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
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

#ifdef ENABLE_ITEMS

	//アイテムボックスマネージャーにメッシュを設定
	m_pItemBoxManager->AttachMesh(m_pStaticMeshItemBox);

#endif//#ifdef ENABLE_ITEMS

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

	//背景画像を設定.
	m_pBackImgObject->AttachMesh(m_pStaticMeshBackImg);

	// 当たり判定マネージャーに必要なクラスをセット
	// 爆風用の弾をセット
	m_pCollisionManager->SetBlastMesh(m_pStaticMesh_BulletRed);
	// 壁をセット
	m_pCollisionManager->SetCStageWall(m_pWallTop, m_pWallBottom, m_pWallLeft, m_pWallRight);
	// 地面をセット
	m_pCollisionManager->SetCStageGround(m_pGround);
	// 木箱をセット
	m_pCollisionManager->SetCStageWoodBox(m_pWoodBoxTopLeft, m_pWoodBoxTopRight, m_pWoodBoxCenter, m_pWoodBoxBottomLeft, m_pWoodBoxBottomRight);
	// 弾をセット
	m_pCollisionManager->SetCShotManager(m_pShotManager);
	// キャラクターマネージャーをセット
	m_pCollisionManager->SetCCharacterManager(m_pCharacterManager);
	//爆風マネージャーを設定.
	m_pCollisionManager->SetCBlastManager(m_pBlastManager);

	//弾マネージャーを設定.
	m_pCharacterManager->SetShotManager(m_pShotManager);
	//
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

	// プレイヤーの初期座標設定
	m_pCharacterManager->SetStartPosition();

	//COMに渡す障害物の情報
	BuildComObstacles();
	m_pCharacterManager->SetComObstacles(&m_ComObstacles);

}

void CGameMain::CreateBounding()
{
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//各プレイヤーの当たり判定作成
		switch (i)
		{
		case 0:
			m_pCharacterManager->CreateBounding(i, m_pStaticMesh_TankBodyRed, m_pStaticMesh_TankCannonRed);
			break;
		case 1:
			m_pCharacterManager->CreateBounding(i, m_pStaticMesh_TankBodyYellow, m_pStaticMesh_TankCannonYellow);
			break;
		case 2:
			m_pCharacterManager->CreateBounding(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
			break;
		case 3:
			m_pCharacterManager->CreateBounding(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			break;
		}
		//各プレイヤーの当たり判定作成.
		m_pCharacterManager->CreateCollider(i);
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

	// プレイヤーの初期座標設定
	m_pCharacterManager->SetStartPosition();

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

	//タイマーの数字の情報.
	m_TimerNumber->SetBasePosition(D3DXVECTOR2{ WND_W / 2, WND_H / 2 - 16 });
	m_TimerNumber->SetDigitWidth(32);

	// カウントダウン設定
	m_Timer->CountDown();
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
void CGameMain::EachSettingHitPoint(int i)
{
	int chara_hp	= m_pCharacterManager->GetControlPlayer(i)->GetHP();
	int chara_MaxHP = m_pCharacterManager->GetControlPlayer(i)->GetMaxHP();

	//全てのHPアイコンを非表示.
	for (int hp = 0; hp < chara_MaxHP; hp++)
	{
		m_pSpriteHitPoint[i][hp]->SetIsDisplay(false);
	}

	//HPの画像の設定.
	for (int hp = 0; hp < chara_hp; hp++)
	{
		auto hp_sprite = m_pSpriteHitPoint[i][hp];
		hp_sprite->SetIsDisplay(true);

		if (hp <= 0)
		{
			hp_sprite->SetPosition(WND_W / 2 - 128.f, 128.f, 0.f);
		}
		else
		{
			hp_sprite->SetPosition(WND_W / 2 + 64.f, 128.f, 0.f);
		}
		hp_sprite->SetRotation(0.f, 0.f, 0.f);
		hp_sprite->SetScale(-0.8f, 0.8f, 0.5f);
	}
}
 

void CGameMain::BuildComObstacles()
{
	m_ComObstacles.clear();

	auto addObstacle = [&](float x, float z, float radius)
		{
			CComPlayer::SimpleObstacle o;
			o.pos = D3DXVECTOR3(x, 0.0f, z);
			o.radius = radius;
			m_ComObstacles.push_back(o);
		};

	// 壁を複数の円でカバーする
	const float wallRadius = 6.0f;		//壁円の半径
	const float wallOffset = 30.0f;		//壁のZX座標

	// 壁上壁下
	for (float x = -24.0f; x <= 24.0f; x += 8.0f)
	{
		addObstacle(x, wallOffset, wallRadius);   // 壁上
		addObstacle(x, -wallOffset, wallRadius);  // 壁下
	}

	for (float z = -24.f; z <= 24.f; z += 8.0f)
	{
		addObstacle(-wallOffset, z, wallRadius);
		addObstacle(wallOffset, z, wallRadius);
	}

	// 木箱
	auto addBoxObstacle = [&](const std::shared_ptr<CStaticMeshObject>& obj, float radius)
		{
			if (!obj) return;
			CComPlayer::SimpleObstacle o;
			o.pos = obj->GetPosition();
			o.pos.y = 0.0f;
			o.radius = radius;
			m_ComObstacles.push_back(o);
		};

	addBoxObstacle(m_pWoodBoxTopLeft, 3.0f);
	addBoxObstacle(m_pWoodBoxTopRight, 3.0f);
	addBoxObstacle(m_pWoodBoxCenter, 3.0f);
	addBoxObstacle(m_pWoodBoxBottomLeft, 3.0f);
	addBoxObstacle(m_pWoodBoxBottomRight, 3.0f);
}