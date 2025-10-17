#define GetKey(KEY) (GetAsyncKeyState(KEY))
#undef min	//マクロ定義無効匁E
#undef max	

#include "CGameMain.h"
//-----サウンチE----
#include "Assets//Sound//CSoundManager.h" // サウンド�Eネ�Eジャークラス
#include "Assets//Effect//CEffect.h"	//Effekseerを使ぁE��め�Eクラス

//-----DirectX-----
#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9クラス
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11クラス

//�萔�錾.
static constexpr int TIME = 90.0;

CGameMain::CGameMain(HWND hWnd)
	: m_hWnd									( hWnd )

	//画僁E
	, m_pSprite2DTimerFrame			( nullptr )
	, m_pSprite2DTimer						( nullptr )
	, m_pSprite2DKillNomber			( nullptr )
	, m_pSprite2DHitPoint					( nullptr )
	, m_pSprite2DPlayerIcon				()

	//画像�E設宁E
	, m_pSpriteTimerFrame				()
	, m_pSpriteTimer							()
	, m_pSpritePlayerIcon					()
	, m_pSpriteKillNomber				()
	, m_pSpriteHitPoint						()

	, m_pSpriteGround					( nullptr )
	, m_pSpritePlayer					( nullptr )
	, m_pSpriteExplosion				( nullptr )

	, m_pStaticMeshGround			( nullptr )
	, m_pStaticMeshBSphere			( nullptr )
	, m_pStaticMeshItemBox			( nullptr )

	// 戦軁E
	, m_pStaticMesh_TankBodyRed			( nullptr )
	, m_pStaticMesh_TankCannonRed		( nullptr )
	, m_pStaticMesh_TankBodyYellow		( nullptr )
	, m_pStaticMesh_TankCannonYellow	( nullptr )
	, m_pStaticMesh_TankBodyBlue			( nullptr )
	, m_pStaticMesh_TankCannonBlue		( nullptr )
	, m_pStaticMesh_TankBodyGreen		( nullptr )
	, m_pStaticMesh_TankCannonGreen	( nullptr )

	// 弾
	, m_pStaticMesh_BulletRed			( nullptr )
	, m_pStaticMesh_BulletYellow		( nullptr )
	, m_pStaticMesh_BulletBlue			( nullptr )
	, m_pStaticMesh_BulletGreen		( nullptr )

	// 壁E
	, m_pStaticMeshWallW				( nullptr )
	, m_pStaticMeshWallH					( nullptr )

	, m_pStcMeshObj							( nullptr )

	, m_pPlayerManager					()
	, m_pShotManager						()

	, m_pGround								( nullptr )

	, m_pDbgText								( nullptr )

	, m_StopTimeCount						( 0 )
	, m_pCameras								()

	, m_Timer										( nullptr )

	, m_pWallTop								( nullptr )
	, m_pWallBottom							( nullptr )
	, m_pWallLeft								( nullptr )
	, m_pWallRight							( nullptr )
	, m_pItemBoxManager				( nullptr )

	, m_Rot							( 0.0f )

	, time							( 0.0f )

{
	//最初�Eシーンをメインにする.
	m_SceneType = CSceneType::Main;
}

CGameMain::~CGameMain()
{
	//外部で作�EしてぁE��ので、ここでは破棁E��なぁE
	m_hWnd = nullptr;
}

void CGameMain::Update()
{
	//BGMのループ�E甁E
	CSoundManager::PlayLoop(CSoundManager::BGM_Main);

	//プレイヤー全員更新
	m_pPlayerManager->Update();


	// 弾の発封E
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
		m_pShotManager->Update();
	}

	//カメラ追従！E��新.砲塔基溁E
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(i))
		{
			const D3DXVECTOR3 camPos = player->GetCannonPosition();	//砲塔�E位置
			float yaw = player->GetCannonYaw();	//砲塔�E向きY

			m_pCameras[i]->SetTargetPos(camPos);
			m_pCameras[i]->SetTargetRotY(yaw);
		}
		m_pCameras[i]->Update();
	}

	//アイチE��の動佁E
	m_pItemBoxManager->Update();

#if 0
	//Effect制御
	{
		//エフェクト�Eインスタンスごとに忁E��なハンドル
		//※�E�つ描画して制御するなら３つ忁E��になめE
		static ::EsHandle hEffect = -1;
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			if (GetAsyncKeyState('Y') & 0x0001) {
				hEffect = CEffect::GetInstance().Play(CEffect::Test0, D3DXVECTOR3(0.f, 1.f, 0.f));

				//拡大縮封E
				CEffect::GetInstance().SetScale(hEffect, D3DXVECTOR3(0.8f, 0.8f, 0.8f));

				//回転(Y軸回転)

				CEffect::GetInstance().SetRotation(hEffect, D3DXVECTOR3(m_pPlayerManager->GetRotation(i)));

				//位置を�E設宁E
				CEffect::GetInstance().SetLocation(hEffect, D3DXVECTOR3(m_pPlayerManager->GetPosition(i)));

			}
			if (GetAsyncKeyState('T') & 0x0001) {
				CEffect::GetInstance().Stop(hEffect);
			}
		}

	}
#endif

	//勝敗条件(確認用).
	//勝ち.
	if (GetKey('K') & 0x8000)
	{
		//BGMのループ停止.
		CSoundManager::Stop(CSoundManager::BGM_Bonus);

		m_SceneType = CSceneType::Result;
	}
	//敗北.
	//体力がなくなるか
	if (GetKey('L') & 0x8000)
	{
		//BGMのループ停止.
		CSoundManager::Stop(CSoundManager::BGM_Bonus);

		m_SceneType = CSceneType::Result;
	}

	// Cキー押されたら操作�Eレイヤー刁E��替ぁE
	if (GetKey('C') & 0x8000)
	{
		m_pPlayerManager->SwitchActivePlayer();
	}

	m_pWallTop->Update();
	m_pWallBottom->Update();
	m_pWallLeft->Update();
	m_pWallRight->Update();
}



void CGameMain::Draw()
{
	auto* pContext = CDirectX11::GetInstance().GetContext();

	//画面サイズfloatで扱ぁE3D11_VIEWPORTの型に合わせる
	const float W = static_cast<float>(WND_W);
	const float H = static_cast<float>(WND_H);

	//2x2刁E��の定義
	const int COLS = 2;		//2めEにしたら一画面、Eなら四画面.
	const int ROWS = 2;		//2めEにしたら一画面、Eなら四画面.
	const int MAX_VIEWS = COLS * ROWS;					//刁E��して表示できる最大ビュー数
	const int VIEWS = std::min(PLAYER_MAX, MAX_VIEWS);	//minで小さぁE��ぁE��合わせる

	//1ビューポ�Eト�Eを描画する処琁E��ラムダにまとめる
	auto DrawOneViewport = [&](std::shared_ptr<CCamera> camera, std::shared_ptr<CPlayer> owner)
	{
		//カメラ更新
		camera->Update();

		//スナップショチE��をconst参�Eでキャプチャ
		D3DXMATRIX& view	= camera->m_mView;
		D3DXMATRIX& proj	= camera->m_mProj;
		LIGHT&		light	= camera->m_Light;
		CAMERA&		paramC	= camera->m_Camera;

		//プレイヤーを描画.ここで全員描く
		for (int players = 0; players < PLAYER_MAX; ++players)
		{
			if (auto p = m_pPlayerManager->GetControlPlayer(players))
			{
				p->Draw(view, proj, light, paramC);
			}
		}

	//オブジェクト�E描画.
	//弾描画.
	m_pShotManager->Draw(view, proj, light, paramC);

	//地面描画
	if (owner) m_pGround->SetPlayer(*owner);
	{
		m_pGround->Draw(view, proj, light, paramC);

		//�ǂ̕\��.
		m_pWallTop->Draw(view, proj, light, paramC);
		m_pWallBottom->Draw(view, proj, light, paramC);
		m_pWallLeft->Draw(view, proj, light, paramC);
		m_pWallRight->Draw(view, proj, light, paramC);
		//エフェクトもここでめE��
	};
	//アイチE��ボックス描画.
	m_pItemBoxManager->Draw(view, proj, light, paramC);

	//4画面の時�E表示.
	//前後関係無要E
	CDirectX11::GetInstance().SetDepth(false);
	//UI.
	for (int i = 0; i < HP_MAX; i++)
	{
		m_pSpriteHitPoint[i]->Draw();
	}
	CDirectX11::GetInstance().SetDepth(true);

	//エフェクトもここでめE��

	};


	//刁E��ビューのルーチE
	for (int i = 0; i < VIEWS; ++i)
	{
		//ビューポ�Eト設宁E
		const D3D11_VIEWPORT vp = MakeGridViewport(i, COLS, ROWS, W, H);
		pContext->RSSetViewports(1, &vp);

		//カメラ参�Eを取征E参�E外しで実体を直接扱ぁE
		std::shared_ptr<CCamera> camera = m_pCameras[i];

		std::shared_ptr<CPlayer> owner = m_pPlayerManager->GetControlPlayer(i);
			////////チE��チE��チE��スト�E描画
			//////m_pDbgText->SetColor(0.9f, 0.6f, 0.f);	//色の設宁E
			//////m_pDbgText->Render(_T("ABCD"), 10, 100);

		//1ビュー刁E��描画
		DrawOneViewport(camera, owner);
			////////チE��チE��チE��スチE数値入めEの描画
			//////m_pDbgText->SetColor(1.f, 0.f, 0.f);
			//////TCHAR dbgText[64];
			//////_stprintf_s(dbgText, _T("Float:%f, %f"), 1.f, 2.2f);
			//////m_pDbgText->Render(dbgText, 10, 110);

//4画面の時�E表示.
		//前後関係無要E
		CDirectX11::GetInstance().SetDepth(false);
		//プレイヤー番号の描画.
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
		//キル数の描画.
		m_pSpriteKillNomber[i]->Draw();
		CDirectX11::GetInstance().SetDepth(true);

	}

	//全画面ビューポ�Eトに戻ぁE
	D3D11_VIEWPORT fullvp = {};
	fullvp.TopLeftX = 0;		//ビューポ�Eト左上�EX座樁E
	fullvp.TopLeftY = 0;		//ビューポ�Eト左上�EY座樁E
	fullvp.Width	= 1920;		//ビューポ�Eト�E幁Eここで今回は画面全画面を基準とする
	fullvp.Height	= 1080;		//ビューポ�Eト�E高さ
	fullvp.MinDepth = 0.0f;		//深度バッファの最小値
	fullvp.MaxDepth = 1.0f;		//深度バッファの最大値
	pContext->RSSetViewports(1, &fullvp);


////1画面の時�E表示.
	//前後関係無要E
	CDirectX11::GetInstance().SetDepth(false);
	//タイマ�Eの枠の描画.
	m_pSpriteTimerFrame->Draw();
	//タイマ�Eの描画.
	m_pSpriteTimer->Draw();
	////プレイヤー番号の描画.
	//m_pSpritePlayerIcon[0]->Draw();
	////キル数の描画.
	//m_pSpriteKillNomber[0]->Draw();
	////HPの描画.
	//for (int i = 0; i < HP_MAX; i++)
	//{
	//	m_pSpriteHitPoint[i]->Draw();
	//}
	CDirectX11::GetInstance().SetDepth(true);

	//タイマ�E描画.
	m_Timer->Draw();

	// 当たり判定描画
	m_pCollisionManager->Draw();
}




void CGameMain::Init()
{
	//定数宣言.
	static constexpr float TIME = 90.f;

	//カメラ位置設宁E
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//プレイヤーマネージャーから吁E�Eレイヤーの位置を取征E
		D3DXVECTOR3 pos = m_pPlayerManager->GetPosition(i);

		m_pCameras[i]->SetCameraPos(pos.x, pos.y, pos.z);
		m_pCameras[i]->SetLightPos(0.f, 2.f, 5.f);
	}
	//地面の大きさ設宁E
	m_pGround->SetScale(0.4f, 0.4f, 0.4f);

	//アイチE��ボックスの設宁E
	m_pItemBoxManager->SetPosition(-10.f, 20.f, 0.f);
	m_pItemBoxManager->SetRotation(0.f, 0.f, 0.f);
	m_pItemBoxManager->SetScale(0.2f, 0.2f, 0.2f);

//-----中忁E��示用座樁E----.
	//制限時間枠の画像�E設宁E
	m_pSpriteTimerFrame->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
	//制限時間�Eの画像�E設宁E
	m_pSpriteTimer->SetPosition(WND_W / 2.f - 74.f, WND_H / 2 - 32.f, 0.f);
	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);
	//�������ԉ摜�̐ݒ�..
	EachSettingTimer();
	//�v���C���[�ԍ��摜�̐ݒ�..
	EachSettingPlayerNumber();
	//�|�������摜�̐ݒ�..
	EachSettingKillNumber();
	//HP�̉摜�̐ݒ�..
	EachSettingHitPoint();


////-----中間発表用-----.
//	//制限時間枠の画像�E設宁E
//	m_pSpriteTimerFrame->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
//	//制限時間�Eの画像�E設宁E
//	m_pSpriteTimer->SetPosition(WND_W - 160.f, WND_H - 96.f, 0.f);
//	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);


////-----中忁E��示用座樁E----.
//	//.
//	m_pSpritePlayerIcon->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpritePlayerIcon->SetRotation(0.f, 0.f, 0.f);
//	m_pSpritePlayerIcon->SetScale(1.f, 1.f, 0.f);

//-----中間発表用-----.
	//プレイヤー番号の画像�E設宁E
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//プレイヤー1P.
		case 2:			//プレイヤー3P.
			m_pSpritePlayerIcon[i]->SetPosition(0.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		case 1:			//プレイヤー2P.
		case 3:			//プレイヤー4P.
			m_pSpritePlayerIcon[i]->SetPosition(WND_W - 256.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		default:
			break;
		}
	}

////-----中忁E��示用座樁E----.
//	//.
//	m_pSpriteKillNomber->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpriteKillNomber->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteKillNomber->SetScale(1.f, 1.f, 0.f);

//-----中間発表用-----.
	//プレイヤー番号の画像�E設宁E
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//プレイヤー1P.
		case 2:			//プレイヤー3P.
			m_pSpriteKillNomber[i]->SetPosition(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		case 1:			//プレイヤー2P.
		case 3:			//プレイヤー4P.
			m_pSpriteKillNomber[i]->SetPosition(WND_W - 320.f, 0.f, 0.f);//320:256サイズの画像に斁E���E64を足した数.
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		default:
			break;
		}
	}


////-----中忁E��示用座樁E----.
//	//.
//	m_pSpriteHitPoint->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);
//	m_pSpriteHitPoint->SetRotation(0.f, 0.f, 0.f);
//	m_pSpriteHitPoint->SetScale(1.f, 1.f, 0.f);

//-----中間発表用-----.
	//HPの画像�E設宁E
	for (int i = 0; i < HP_MAX; i++)
	{
		if (i <= 0)
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2 - 128.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
		else
		{
			m_pSpriteHitPoint[i]->SetPosition(WND_W / 2 , 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteHitPoint[i]->SetScale(0.5f, 0.5f, 0.5f);
		}
	}

	//制限時間�E斁E��サイズ.
	m_pDbgText->SetFontSize(5.0f);

//-----中忁E��示用座樁E----.
	//ゲームで遊べめEクリア画面に遷移する)時間※引数.
	m_Timer->StartTimer(TIME);
	m_Timer->SetDebugFont(m_pDbgText);
	m_Timer->SetTimerPosition(WND_W / 2 - 15.f, WND_H / 2 - 30.f);

	SetPosition();

////-----中間発表用-----.
//	//ゲームで遊べめEクリア画面に遷移する)時間※引数.
//	m_Timer->StartTimer(TIME);
//	m_Timer->SetDebugFont(m_pDbgText);
//	m_Timer->SetTimerPosition(WND_W - 96.f, WND_H - 96.f);

}

void CGameMain::Destroy()
{

}

void CGameMain::Create()
{
	//Effectクラス
	CEffect::GetInstance().Create(
		CDirectX11::GetInstance().GetDevice(),
		CDirectX11::GetInstance().GetContext());

	//UIObjectのインスタンス生�E.
	m_pSpriteTimerFrame = std::make_shared<CUIObject>();
	m_pSpriteTimer		= std::make_shared<CUIObject>();
	//HPの刁E��け生戁E
	for (int i = 0; i < HP_MAX; i++)
	{
		m_pSpriteHitPoint[i] = std::make_shared<CUIObject>();
	}
	//プレイヤーの刁E��け生戁E
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		m_pSpritePlayerIcon[i] = std::make_shared<CUIObject>();
	}
	//キル数の刁E��け生戁E
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		m_pSpriteKillNomber[i] = std::make_shared<CUIObject>();
	}

	//UI系のインスタンス生�E.
	m_pSprite2DTimerFrame	= std::make_shared<CSprite2D>();
	m_pSprite2DTimer		= std::make_shared<CSprite2D>();
	m_pSprite2DTimerArrow	= std::make_shared<CSprite2D>();
	m_pSprite2DKillNomber	= std::make_shared<CSprite2D>();
	m_pSprite2DHitPoint		= std::make_shared<CSprite2D>();
	//プレイヤーの刁E��け生戁E
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		m_pSprite2DPlayerIcon[i] = std::make_shared<CSprite2D>();
	}

	//スプライト�Eインスタンス作�E.
	m_pSpriteGround = std::make_unique<CSprite3D>();
	m_pSpritePlayer = std::make_unique<CSprite3D>();
	m_pSpriteExplosion = std::make_shared<CSprite3D>();

	//スタチE��チE��メチE��ュオブジェクト�Eインスタンス作�E
	m_pStcMeshObj = std::make_unique<CStaticMeshObject>();

	//スタチE��チE��メチE��ュのインスタンス作�E
	m_pStaticMeshGround			= std::make_shared<CStaticMesh>();
	m_pStaticMeshBSphere		= std::make_shared<CStaticMesh>();
	m_pStaticMeshItemBox		= std::make_shared<CStaticMesh>();

	// 戦車�EメチE��ュ.
	m_pStaticMesh_TankBodyRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonBlue	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyGreen		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonGreen	= std::make_shared<CStaticMesh>();

	// 弾のメチE��ュ.
	m_pStaticMesh_BulletRed			= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletYellow		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletGreen		= std::make_shared<CStaticMesh>();

	// 壁�EメチE��ュ
	m_pStaticMeshWallW				= std::make_shared<CStaticMesh>();
	m_pStaticMeshWallH				= std::make_shared<CStaticMesh>();

	//チE��チE��チE��スト�Eインスタンス作�E
	m_pDbgText = std::make_unique<CDebugText>();

	//プレイヤーと砲塔�Eインスタンス生�E
	m_pPlayerManager = std::make_shared<CPlayerManager>();
	
	//マネージャーは一回だけInitialize
	m_pPlayerManager->Initialize();

	//弾クラスのインスタンス作�E
	m_pShotManager = std::make_shared<CShotManager>();
	m_pShotManager->Initialize(PLAYER_MAX);


	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//プレイヤーiの位置を少しずつずらぁE
		float offsetX = (i % 2) * 12.0f;
		float offsetZ = (i / 2) * 12.0f;
		m_pPlayerManager->SetPlayerPosition(i, D3DXVECTOR3(offsetX, 0.0f, offsetZ));
		//回転を設宁E
		m_pPlayerManager->SetPlayerRotation(i, D3DXVECTOR3(0.f, 0.f, 0.f));

		////砲塔�E生�EもすめE
		//auto cannon = std::make_unique<CCannon>();

		//カメラ生�E・セチE��アチE�E
		auto camera = std::make_unique<CCamera>();
		camera->SetTargetPos(m_pPlayerManager->GetPosition(i));
		camera->SetTargetRotY(m_pPlayerManager->GetRotation(i).y);
		m_pCameras[i] = std::move(camera);

	}

	//地面クラスのインスタンス作�E.
	m_pGround = std::make_unique<CGround>();

	//制限時間�Eインスタンス生�E.
	m_Timer = std::make_shared<CTimer>();

	// 壁クラスのインスタンス生�E
	m_pWallTop		= std::make_shared<CWall>();
	m_pWallBottom	= std::make_shared<CWall>();
	m_pWallLeft		= std::make_shared<CWall>();
	m_pWallRight	= std::make_shared<CWall>();
	//アイチE��マネージャークラスのインスタンス生�E.
	m_pItemBoxManager = std::make_shared<CItemBoxManager>();
	m_pItemBoxManager->Create();

	m_pCollisionManager = std::make_shared<CCollisionManager>();
	m_pCollisionManager->Create();
	m_pCollisionManager->CreateTank(PLAYER_MAX);
}

HRESULT CGameMain::LoadData()
{
	//チE��チE��チE��スト�E読み込み.
	if (FAILED(m_pDbgText->Init(CDirectX11::GetInstance())))
	{
		return E_FAIL;
	}

	//Effectクラス
	if (FAILED(CEffect::GetInstance().LoadData())) {
		return E_FAIL;
	}

	//タイマ�E画像�Eスプライト設宁E
	CSprite2D::SPRITE_STATE WH_SIZE = {
		1920, 1080,		//描画幁E高さ.
		1920, 1080,		//允E��像�E幁E高さ.
		1920, 1080		//アニメーションをしなぁE�Eで、EでぁE��.
	};
	//タイマ�E枠画像�Eスプライト設宁E
	CSprite2D::SPRITE_STATE TIMER_SIZE = {
		256, 256,		//描画幁E高さ.
		256, 256,		//允E��像�E幁E高さ.
		256, 256		//アニメーションをしなぁE�Eで、EでぁE��.
	};
	//タイマ�E枠画像�Eスプライト設宁E
	CSprite2D::SPRITE_STATE ICON_SIZE = {
		256, 256,		//描画幁E高さ.
		256, 256,		//允E��像�E幁E高さ.
		256, 256		//アニメーションをしなぁE�Eで、EでぁE��.
	};
	//制限時間�E枠の読み込み.
	m_pSprite2DTimerFrame	->Init(_T("Data\\Texture\\UI\\TimerFrame.png"), WH_SIZE);
	m_pSprite2DTimer		->Init(_T("Data\\Texture\\UI\\Timer.png"), TIMER_SIZE);
	m_pSprite2DKillNomber	->Init(_T("Data\\Texture\\UI\\KillNum.png"), ICON_SIZE);
	m_pSprite2DHitPoint		->Init(_T("Data\\Texture\\UI\\HP.png"), ICON_SIZE);

	//画像をアタチE��.
	m_pSpriteTimerFrame	->AttachSprite(m_pSprite2DTimerFrame);
	m_pSpriteTimer		->AttachSprite(m_pSprite2DTimer);
	//HPの刁E��けアタチE��.
	for (int i = 0; i < HP_MAX; i++)
	{
		m_pSpriteHitPoint[i]->AttachSprite(m_pSprite2DHitPoint);
	}
	//キル数の刁E��けアタチE��.
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		m_pSpriteKillNomber[i]->AttachSprite(m_pSprite2DKillNomber);
	}
	//プレイヤーの刁E��けアタチE��.
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

	//地面スプライト�E構造佁E
	CSprite3D::SPRITE_STATE SSGround;
	SSGround.Disp.w = 1.f;
	SSGround.Disp.h = 1.f;
	SSGround.Base.w = 256.f;
	SSGround.Base.h = 256.f;
	SSGround.Stride.w = 256.f;
	SSGround.Stride.h = 256.f;
	//地面スプライト�E読み込み.
	m_pSpriteGround->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Ground.png"), SSGround);

	//プレイヤースプライト�E構造佁E
	CSprite3D::SPRITE_STATE SSPlayer =
	{ 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };
	//プレイヤースプライト�E読み込み.
	m_pSpritePlayer->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Player.png"), SSPlayer);

	//爁E��スプライト�E構造佁E
	CSprite3D::SPRITE_STATE SSExplosion =
	{ 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };
	//爁E��スプライト�E読み込み.
	m_pSpriteExplosion->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\explosion.png"), SSExplosion);

	//--------------------------------------------------------------------------
	// 	   画像�E読み込み.
	//--------------------------------------------------------------------------
	//スタチE��チE��メチE��ュの読み込み
	m_pStaticMeshGround->Init(_T("Data\\Mesh\\Static\\Stage\\stage.x"));
	m_pStaticMeshItemBox->Init(_T("Data\\Mesh\\Static\\ItemBox\\ItemBox.x"));

	// 戦軁E赤)
	m_pStaticMesh_TankBodyRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Body\\Body.x"));
	m_pStaticMesh_TankCannonRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Cannon\\Cannon.x"));

	// 戦軁E黁E
	m_pStaticMesh_TankBodyYellow->Init(_T("Data\\Mesh\\Static\\Tank\\Yellow\\Body\\Body.x"));
	m_pStaticMesh_TankCannonYellow->Init(_T("Data\\Mesh\\Static\\Tank\\Yellow\\Cannon\\Cannon.x"));

	// 戦軁E靁E
	m_pStaticMesh_TankBodyBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Body\\Body.x"));
	m_pStaticMesh_TankCannonBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Cannon\\Cannon.x"));

	// 戦軁E緁E
	m_pStaticMesh_TankBodyGreen->Init(_T("Data\\Mesh\\Static\\Tank\\Green\\Body\\Body.x"));
	m_pStaticMesh_TankCannonGreen->Init(_T("Data\\Mesh\\Static\\Tank\\Green\\Cannon\\Cannon.x"));
	
	// 弾(赤)
	m_pStaticMesh_BulletRed->Init(_T("Data\\Mesh\\Static\\Bullet\\Red\\Ball.x"));
	// 弾(黁E
	m_pStaticMesh_BulletYellow->Init(_T("Data\\Mesh\\Static\\Bullet\\Yellow\\Ball.x"));
	// 弾(靁E
	m_pStaticMesh_BulletBlue->Init(_T("Data\\Mesh\\Static\\Bullet\\Blue\\Ball.x"));
	// 弾(緁E
	m_pStaticMesh_BulletGreen->Init(_T("Data\\Mesh\\Static\\Bullet\\Green\\Ball.x"));
	
	// 壁E
	m_pStaticMeshWallW->Init(_T("Data\\Mesh\\Static\\Wall\\Wall1.x"));
	m_pStaticMeshWallH->Init(_T("Data\\Mesh\\Static\\Wall\\Wall2.x"));

	//バウンチE��ングスフィア(当たり判定用).
	m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Sphere.x"));


	// それぞれのプレイヤーに色にあった戦車をアタチE��
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		switch (i)
		{
		case 0:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyRed, m_pStaticMesh_TankCannonRed);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletRed);
			break;
		case 1:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyYellow, m_pStaticMesh_TankCannonYellow);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletYellow);
			break;
		case 2:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletGreen);
			break;
		case 3:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletBlue);
			break;
		default:
			break;
		}


	}

	//スタチE��チE��メチE��ュを設宁E
	m_pGround->AttachMesh(m_pStaticMeshGround);

	//アイチE��ボックスマネージャーにメチE��ュを設宁E
	m_pItemBoxManager->AttachMesh(m_pStaticMeshItemBox);

	////バウンチE��ングスフィアの作�E.
	//m_pPlayer->CreateBSphareForMesh(*m_pStaticMeshBSphere);

	m_pWallTop->AttachMesh(m_pStaticMeshWallW);
	m_pWallBottom->AttachMesh(m_pStaticMeshWallW);
	m_pWallLeft->AttachMesh(m_pStaticMeshWallH);
	m_pWallRight->AttachMesh(m_pStaticMeshWallH);

	CreateBounding();

	return S_OK;
}

void CGameMain::SetPosition()
{
	// 上�E壁�E初期座標を設宁E
	m_pWallTop->SetPosition(0, 0, 30);
	m_pWallTop->SetRotation(0, 0, 0);

	// 下�E壁�E初期座標を設宁E
	m_pWallBottom->SetPosition(0, 0, -30);
	m_pWallBottom->SetRotation(0, 0, 0);

	// 左の壁�E初期座標を設宁E
	m_pWallLeft->SetPosition(-30, 0, 0);
	m_pWallLeft->SetRotation(0, 0, 0);

	// 右の壁�E初期座標を設宁E
	m_pWallRight->SetPosition(30, 0, 0);
	m_pWallRight->SetRotation(0, 0, 0);
}

<<<<<<< HEAD
void CGameMain::CreateBounding()
{
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		//�v���C���[�̃o�E���f�B���O�̍쐬.
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
		//�v���C���[�̓����蔻����쐬.
		m_pPlayerManager->CreateCollider(i);
	}
	
	//��̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallTop->CreateBBoxForMesh(*m_pStaticMeshWallW);
	//��̕ǂ̓����蔻���ݒ�.
	m_pWallTop->CreateBoxCollider(m_pWallTop->GetMinPos(), m_pWallTop->GetMaxPos());

	//���̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallBottom->CreateBBoxForMesh(*m_pStaticMeshWallW);
	//���̕ǂ̓����蔻���ݒ�.
	m_pWallBottom->CreateBoxCollider(m_pWallBottom->GetMinPos(), m_pWallBottom->GetMaxPos());

	//���̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallLeft->CreateBBoxForMesh(*m_pStaticMeshWallH);
	//���̕ǂ̓����蔻���ݒ�.
	m_pWallLeft->CreateBoxCollider(m_pWallLeft->GetMinPos(), m_pWallLeft->GetMaxPos());

	//�E�̕ǂ̃o�E���f�B���O�̍쐬.
	m_pWallRight->CreateBBoxForMesh(*m_pStaticMeshWallH);
	//�E�̕ǂ̓����蔻���ݒ�.
	m_pWallRight->CreateBoxCollider(m_pWallRight->GetMinPos(), m_pWallRight->GetMaxPos());
}

void CGameMain::Collision()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// i �Ԃ̃v���C���[���擾
		auto player = m_pPlayerManager->GetControlPlayer(i);
		auto Coll = player->GetBody()->GetCollider();
		//if (!player) continue; // ���݂��Ȃ��v���C���[�̓X�L�b�v

		D3DXVECTOR3 push(0.0f, 0.0f, 0.0f);

		if (Coll && m_pWallTop->GetCollider() &&
			Coll->CheckCollision(*m_pWallTop->GetCollider()))
		{
			push.z -= 0.1f;
		}
		if (Coll && m_pWallBottom->GetCollider() &&
			Coll->CheckCollision(*m_pWallBottom->GetCollider()))
		{
			push.z += 0.1f;
		}
		if (Coll && m_pWallLeft->GetCollider() &&
			Coll->CheckCollision(*m_pWallLeft->GetCollider()))
		{
			push.x += 0.1f;
		}
		if (Coll && m_pWallRight->GetCollider() &&
			Coll->CheckCollision(*m_pWallRight->GetCollider()))
		{
			push.x -= 0.1f;
		}
		player->GetBody()->PushBack(push);
		//m_pPlayerManager->SetPushBackPosision(i, push);
	}
}

//��ʂ��O���b�h�ɕ��������Ƃ��Aidx�Ԗڂ̃}�X�ɑΉ�����
//D3D11_VIEWPORT���쐬���ĕԂ��֐�
=======
//画面をグリチE��に刁E��したとき、idx番目のマスに対応すめE
//D3D11_VIEWPORTを作�Eして返す関数
>>>>>>> d04a749823a25d337a5f847dae7ae588acfa3550
D3D11_VIEWPORT CGameMain::MakeGridViewport(int idx, int cols, int rows, float totalW, float totalH)
{
	//どのマスか、�E・行を算�E
	const int col = idx % cols;	//列番号
	const int row = idx / cols;	//行番号

	//ビューポ�Eトを作�E
	D3D11_VIEWPORT vp{};					//ゼロ初期匁E
	vp.TopLeftX = (totalW / cols) * col;	//左丁E0,0)からのオフセチE��
	vp.TopLeftY = (totalH / rows) * row;
	vp.Width = totalW / cols;
	vp.Height = totalH / rows;
	vp.MinDepth = 0.0f;						//深度バッファの最小値設宁E
	vp.MaxDepth = 1.0f;						//深度バッファの最大値設宁E
	return vp;
}


CSceneType CGameMain::GetSceneType() const
{
	return m_SceneType;
}


//-----�e�ݒ���֐���-----..

//�������ԉ摜�̐ݒ�..
void CGameMain::EachSettingTimer()
{
	//�萔�錾..
	static constexpr int TIME = 90.0;

	//-----���S�\���p���W-----..
		//�������Ԙg�̉摜�ݒ�..
	m_pSpriteTimerFrame->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
	//�������ԉ~�̉摜�ݒ�..
	m_pSpriteTimer->SetPosition(WND_W / 2.f - 74.f, WND_H / 2 - 32.f, 0.f);
	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);
	//���v�̐j�̉摜�ݒ�..
	m_pSpriteTimerArrow->SetPosition(WND_W / 2.f - 42.f, WND_H / 2, 0.f);
	m_pSpriteTimerArrow->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerArrow->SetScale(0.25f, 0.25f, 0.f);

	//-----���S�\���p���W-----..
	//�Q�[���ŗV�ׂ�(�N���A��ʂɑJ�ڂ���)���ԁ�����..
	m_Timer->StartTimer(TIME);
	m_Timer->SetDebugFont(m_pDbgText);
	m_Timer->SetTimerPosition(WND_W / 2 - 15.f, WND_H / 2 - 30.f);

	////-----���Ԕ��\�p-----..
	//	//�Q�[���ŗV�ׂ�(�N���A��ʂɑJ�ڂ���)���ԁ�����..
	//	m_Timer->StartTimer(TIME);.
	//	m_Timer->SetDebugFont(m_pDbgText);.
	//	m_Timer->SetTimerPosition(WND_W - 96.f, WND_H - 96.f);.

}
//�v���C���[�ԍ��摜�̐ݒ�..
void CGameMain::EachSettingPlayerNumber()
{
	//-----���Ԕ��\�p-----..
		//�v���C���[�ԍ��̉摜�̐ݒ�..
	for (int i = 0; i < PLAYERNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//�v���C���[1P..
		case 2:			//�v���C���[3P..
			m_pSpritePlayerIcon[i]->SetPosition(0.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		case 1:			//�v���C���[2P..
		case 3:			//�v���C���[4P..
			m_pSpritePlayerIcon[i]->SetPosition(WND_W - 256.f, WND_H - 256.f, 0.f);
			m_pSpritePlayerIcon[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpritePlayerIcon[i]->SetScale(1.f, 1.f, 0.f);
			break;
		default:
			break;
		}
	}
}
//�|�������摜�̐ݒ�..
void CGameMain::EachSettingKillNumber()
{
	////-----���S�\���p���W-----..
	//	//..
	//	m_pSpriteKillNomber->SetPosition(WND_W / 2.f - 84.f, WND_H / 2.f - 64.f, 0.f);.
	//	m_pSpriteKillNomber->SetRotation(0.f, 0.f, 0.f);.
	//	m_pSpriteKillNomber->SetScale(1.f, 1.f, 0.f);.

	//-----���Ԕ��\�p-----..
		//�v���C���[�ԍ��̉摜�̐ݒ�..
	for (int i = 0; i < KILLNUM_MAX; i++)
	{
		switch (i)
		{
		case 0:			//�v���C���[1P..
		case 2:			//�v���C���[3P..
			m_pSpriteKillNomber[i]->SetPosition(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		case 1:			//�v���C���[2P..
		case 3:			//�v���C���[4P..
			m_pSpriteKillNomber[i]->SetPosition(WND_W - 320.f, 0.f, 0.f);//320:256�T�C�Y�̉摜�ɕ�����64�𑫂�����..
			m_pSpriteKillNomber[i]->SetRotation(0.f, 0.f, 0.f);
			m_pSpriteKillNomber[i]->SetScale(0.7f, 0.7f, 0.7f);
			break;
		default:
			break;
		}
	}
}
//�|�������摜�̐ݒ�..
void CGameMain::EachSettingHitPoint()
{
	//-----4��ʗp-----..
		//HP�̉摜�̐ݒ�..
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