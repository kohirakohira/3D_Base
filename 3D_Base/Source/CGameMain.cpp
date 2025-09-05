#define GetKey(KEY) (GetAsyncKeyState(KEY))
#undef min	//マクロ定義無効化
#undef max	

#include "CGameMain.h"
#include "CSoundManager.h"
#include "CEffect.h"	//Effekseerを使うためのクラス
#include "CDirectX9.h"
#include "CDirectX11.h"


CGameMain::CGameMain(HWND hWnd)
	: m_hWnd					( hWnd )

	, m_pSprite2DTimerFrame			( nullptr )
	, m_pSprite2DTimer				( nullptr )
	, m_pSpriteTimerFrame			( nullptr )
	, m_pSpriteTimer				( nullptr )

	, m_pSpriteGround				( nullptr )
	, m_pSpritePlayer				( nullptr )
	, m_pSpriteExplosion			( nullptr )

	, m_pStaticMeshGround			( nullptr )
	, m_pStaticMeshBSphere			( nullptr )

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

	, m_pStcMeshObj					( nullptr )

	, m_pPlayerManager				()
	, m_pShotManager				()

	, m_pGround						( nullptr )

	, m_pDbgText					( nullptr )

	, m_StopTimeCount				( 0 )
	, m_pCameras					()

	, m_Timer						( nullptr )

{
	//最初のシーンをメインにする.
	m_SceneType = CSceneType::Main;
}

CGameMain::~CGameMain()
{
	//外部で作成しているので、ここでは破棄しない
	m_hWnd = nullptr;
}

void CGameMain::Update()
{
	//BGMのループ再生.
	CSoundManager::PlayLoop(CSoundManager::BGM_Bonus);

	//プレイヤー全員更新
	m_pPlayerManager->Update();

	// 弾の発射
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

	//カメラ追従＆更新.砲塔基準
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (auto player = m_pPlayerManager->GetControlPlayer(i))
		{
			const D3DXVECTOR3 camPos = player->GetCannonPosition();	//砲塔の位置
			float yaw = player->GetCannonYaw();	//砲塔の向きY

			m_pCameras[i]->SetTargetPos(camPos);
			m_pCameras[i]->SetTargetRotY(yaw);
		}
		m_pCameras[i]->Update();
	}

#if 0
	//Effect制御
	{
		//エフェクトのインスタンスごとに必要なハンドル
		//※３つ描画して制御するなら３つ必要になる
		static ::EsHandle hEffect = -1;
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			if (GetAsyncKeyState('Y') & 0x0001) {
				hEffect = CEffect::GetInstance().Play(CEffect::Test0, D3DXVECTOR3(0.f, 1.f, 0.f));

				//拡大縮小
				CEffect::GetInstance().SetScale(hEffect, D3DXVECTOR3(0.8f, 0.8f, 0.8f));

				//回転(Y軸回転)

				CEffect::GetInstance().SetRotation(hEffect, D3DXVECTOR3(m_pPlayerManager->GetRotation(i)));

				//位置を再設定
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

	// Cキー押されたら操作プレイヤー切り替え
	if (GetAsyncKeyState('C') & 0x0001)
	{
		m_pPlayerManager->SwitchActivePlayer();
	}
}



void CGameMain::Draw()
{
	auto* pContext = CDirectX11::GetInstance().GetContext();

	//画面サイズfloatで扱うD3D11_VIEWPORTの型に合わせる
	const float W = static_cast<float>(WND_W);
	const float H = static_cast<float>(WND_H);

	//2x2分割の定義
	const int COLS = 2;
	const int ROWS = 2;
	const int MAX_VIEWS = COLS * ROWS;					//分割して表示できる最大ビュー数
	const int VIEWS = std::min(PLAYER_MAX, MAX_VIEWS);	//minで小さいほうに合わせる

	//1ビューポート分を描画する処理をラムダにまとめる
	auto DrawOneViewport = [&](std::shared_ptr<CCamera> camera, std::shared_ptr<CPlayer> owner)
		{
			//カメラ更新
			camera->Update();

			//スナップショットをconst参照でキャプチャ
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

			// 弾描画
			m_pShotManager->Draw(m_pCameras[0]->m_mView, m_pCameras[0]->m_mProj, m_pCameras[0]->m_Light, m_pCameras[0]->m_Camera);

			//地面描画
			if (owner) m_pGround->SetPlayer(*owner);
			m_pGround->Draw(view, proj, light, paramC);

			//エフェクトもここでやる
		};

	//分割ビューのループ
	for (int i = 0; i < VIEWS; ++i)
	{
		//ビューポート設定
		const D3D11_VIEWPORT vp = MakeGridViewport(i, COLS, ROWS, W, H);
		pContext->RSSetViewports(1, &vp);

		//カメラ参照を取得.参照外しで実体を直接扱う
		std::shared_ptr<CCamera> camera = m_pCameras[i];

		std::shared_ptr<CPlayer> owner = m_pPlayerManager->GetControlPlayer(i);
			////////デバッグテキストの描画
			//////m_pDbgText->SetColor(0.9f, 0.6f, 0.f);	//色の設定
			//////m_pDbgText->Render(_T("ABCD"), 10, 100);

		//1ビュー分を描画
		DrawOneViewport(camera, owner);
			////////デバッグテキスト(数値入り)の描画
			//////m_pDbgText->SetColor(1.f, 0.f, 0.f);
			//////TCHAR dbgText[64];
			//////_stprintf_s(dbgText, _T("Float:%f, %f"), 1.f, 2.2f);
			//////m_pDbgText->Render(dbgText, 10, 110);
	}


	//全画面ビューポートに戻す
	D3D11_VIEWPORT fullvp = {};
	fullvp.TopLeftX = 0;		//ビューポート左上のX座標
	fullvp.TopLeftY = 0;		//ビューポート左上のY座標
	fullvp.Width	= 1920;		//ビューポートの幅.ここで今回は画面全画面を基準とする
	fullvp.Height	= 1080;		//ビューポートの高さ
	fullvp.MinDepth = 0.0f;		//深度バッファの最小値
	fullvp.MaxDepth = 1.0f;		//深度バッファの最大値
	pContext->RSSetViewports(1, &fullvp);


	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//タイトルの描画.
	m_pSpriteTimerFrame->Draw();
	m_pSpriteTimer->Draw();
	CDirectX11::GetInstance().SetDepth(true);

	//タイマー描画.
	m_Timer->Draw();


}

void CGameMain::Init()
{
	//定数宣言.
	static constexpr float TIME = 90.f;

	//カメラ位置設定.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//プレイヤーマネージャーから各プレイヤーの位置を取得
		D3DXVECTOR3 pos = m_pPlayerManager->GetPosition(i);

		m_pCameras[i]->SetCameraPos(pos.x, pos.y, pos.z);
		m_pCameras[i]->SetLightPos(0.f, 2.f, 5.f);

	}

	m_pGround->SetScale(0.4f, 0.4f, 0.4f);

	
	//制限時間枠の画像の設定.
	m_pSpriteTimerFrame->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerFrame->SetScale(1.f, 1.f, 0.f);
	//制限時間枠の画像の設定.
	m_pSpriteTimer->SetPosition(WND_W / 2.f - 74.f, WND_H / 2 - 32.f, 0.f);
	m_pSpriteTimer->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimer->SetScale(0.25f, 0.25f, 0.f);



	//制限時間の文字サイズ.
	m_pDbgText->SetFontSize(5.0f);

	//ゲームで遊べる(クリア画面に遷移する)時間※引数.
	m_Timer->StartTimer(TIME);
	m_Timer->SetDebugFont(m_pDbgText);
	m_Timer->SetTimerPosition(WND_W / 2 - 15.f, WND_H / 2 - 30.f);
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

	//UIObjectのインスタンス生成.
	m_pSpriteTimerFrame = std::make_shared<CUIObject>();
	m_pSpriteTimer		= std::make_shared<CUIObject>();

	//UI系のインスタンス生成.
	m_pSprite2DTimerFrame = std::make_shared<CSprite2D>();
	m_pSprite2DTimer	  = std::make_shared<CSprite2D>();

	//スプライトのインスタンス作成.
	m_pSpriteGround = std::make_unique<CSprite3D>();
	m_pSpritePlayer = std::make_unique<CSprite3D>();
	m_pSpriteExplosion = std::make_shared<CSprite3D>();

	//スタティックメッシュオブジェクトのインスタンス作成
	m_pStcMeshObj = std::make_unique<CStaticMeshObject>();

	//スタティックメッシュのインスタンス作成
	m_pStaticMeshGround			= std::make_shared<CStaticMesh>();
	m_pStaticMeshBSphere		= std::make_shared<CStaticMesh>();


	// 戦車のメッシュ.
	m_pStaticMesh_TankBodyRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonRed		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonYellow	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonBlue	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankBodyGreen		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonGreen	= std::make_shared<CStaticMesh>();

	// 弾のメッシュ.
	m_pStaticMesh_BulletRed			= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletYellow		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_BulletGreen		= std::make_shared<CStaticMesh>();

	//デバッグテキストのインスタンス作成
	m_pDbgText = std::make_unique<CDebugText>();

	//プレイヤーと砲塔のインスタンス生成
	m_pPlayerManager = std::make_shared<CPlayerManager>();
	
	//マネージャーは一回だけInitialize
	m_pPlayerManager->Initialize();

	//弾クラスのインスタンス作成
	m_pShotManager = std::make_shared<CShotManager>();
	m_pShotManager->Initialize(PLAYER_MAX);


	for (int i = 0; i < PLAYER_MAX; i++)
	{
		//プレイヤーiの位置を少しずつずらす
		float offsetX = (i % 2) * 12.0f;
		float offsetZ = (i / 2) * 12.0f;
		m_pPlayerManager->SetPlayerPosition(i, D3DXVECTOR3(offsetX, 0.0f, offsetZ));
		//回転を設定.
		m_pPlayerManager->SetPlayerRotation(i, D3DXVECTOR3(0.f, 0.f, 0.f));

		////砲塔の生成もする
		//auto cannon = std::make_unique<CCannon>();

		//カメラ生成・セットアップ
		auto camera = std::make_unique<CCamera>();
		camera->SetTargetPos(m_pPlayerManager->GetPosition(i));
		camera->SetTargetRotY(m_pPlayerManager->GetRotation(i).y);
		m_pCameras[i] = std::move(camera);

	}

	//地面クラスのインスタンス作成.
	m_pGround = std::make_unique<CGround>();

	//制限時間のインスタンス生成.
	m_Timer = std::make_shared<CTimer>();


}

HRESULT CGameMain::LoadData()
{
	//デバッグテキストの読み込み.
	if (FAILED(m_pDbgText->Init(CDirectX11::GetInstance())))
	{
		return E_FAIL;
	}

	//Effectクラス
	if (FAILED(CEffect::GetInstance().LoadData())) {
		return E_FAIL;
	}

	//タイマー画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		1920, 1080,		//描画幅,高さ.
		1920, 1080,		//元画像の幅,高さ.
		1920, 1080		//アニメーションをしないので、0でいい.
	};
	//タイマー枠画像のスプライト設定.
	CSprite2D::SPRITE_STATE TIMER_SIZE = {
		256, 256,		//描画幅,高さ.
		256, 256,		//元画像の幅,高さ.
		256, 256		//アニメーションをしないので、0でいい.
	};
	//制限時間の枠の読み込み.
	m_pSprite2DTimerFrame->Init(_T("Data\\Texture\\Image\\TimerFrame.png"), WH_SIZE);
	m_pSprite2DTimer->Init(_T("Data\\Texture\\Image\\Timer.png"), TIMER_SIZE);

	//画像をアタッチ.
	m_pSpriteTimerFrame->AttachSprite(m_pSprite2DTimerFrame);
	m_pSpriteTimer->AttachSprite(m_pSprite2DTimer);

	//地面スプライトの構造体
	CSprite3D::SPRITE_STATE SSGround;
	SSGround.Disp.w = 1.f;
	SSGround.Disp.h = 1.f;
	SSGround.Base.w = 256.f;
	SSGround.Base.h = 256.f;
	SSGround.Stride.w = 256.f;
	SSGround.Stride.h = 256.f;
	//地面スプライトの読み込み.
	m_pSpriteGround->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Ground.png"), SSGround);

	//プレイヤースプライトの構造体
	CSprite3D::SPRITE_STATE SSPlayer =
	{ 1.f, 1.f, 64.f, 64.f, 64.f, 64.f };
	//プレイヤースプライトの読み込み.
	m_pSpritePlayer->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\Player.png"), SSPlayer);

	//爆発スプライトの構造体
	CSprite3D::SPRITE_STATE SSExplosion =
	{ 1.f, 1.f, 256.f, 256.f, 32.f, 32.f };
	//爆発スプライトの読み込み.
	m_pSpriteExplosion->Init(CDirectX11::GetInstance(),
		_T("Data\\Texture\\explosion.png"), SSExplosion);

	//--------------------------------------------------------------------------
	// 	   画像の読み込み.
	//--------------------------------------------------------------------------
	//スタティックメッシュの読み込み
	m_pStaticMeshGround->Init(_T("Data\\Mesh\\Static\\Stage\\stage.x"));

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
	
	//バウンディングスフィア(当たり判定用).
	m_pStaticMeshBSphere->Init(_T("Data\\Collision\\Sphere.x"));


	// それぞれのプレイヤーに色にあった戦車をアタッチ
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
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletBlue);
			break;
		case 3:
			m_pPlayerManager->AttachMeshesToPlayer(i, m_pStaticMesh_TankBodyGreen, m_pStaticMesh_TankCannonGreen);
			m_pShotManager->AttachMeshToPlayerShot(i, m_pStaticMesh_BulletGreen);
			break;
		default:
			break;
		}
	}


	//スタティックメッシュを設定
	m_pGround->AttachMesh(m_pStaticMeshGround);
	////バウンディングスフィアの作成.
	//m_pPlayer->CreateBSphareForMesh(*m_pStaticMeshBSphere);

	return S_OK;
}

//画面をグリッドに分割したとき、idx番目のマスに対応する
//D3D11_VIEWPORTを作成して返す関数
D3D11_VIEWPORT CGameMain::MakeGridViewport(int idx, int cols, int rows, float totalW, float totalH)
{
	//どのマスか、列・行を算出
	const int col = idx % cols;	//列番号
	const int row = idx / cols;	//行番号

	//ビューポートを作成
	D3D11_VIEWPORT vp{};					//ゼロ初期化
	vp.TopLeftX = (totalW / cols) * col;	//左上(0,0)からのオフセット
	vp.TopLeftY = (totalH / rows) * row;
	vp.Width = totalW / cols;
	vp.Height = totalH / rows;
	vp.MinDepth = 0.0f;						//深度バッファの最小値設定
	vp.MaxDepth = 1.0f;						//深度バッファの最大値設定
	return vp;
}


CSceneType CGameMain::GetSceneType() const
{
	return m_SceneType;
}