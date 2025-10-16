#include "GameDebug.h"
#include "CSoundManager.h"

//前方宣言.
#include "CSprite2D.h"

CGameDebug::CGameDebug(HWND hWnd)
	: m_hWnd							( hWnd )

	, m_pSpriteDebugIcon				( nullptr )
	, m_pSpriteTimer					( nullptr )

	, m_pSpriteObj						( nullptr )
	, m_pSpriteTimerObj					( nullptr )

	, m_pStaticMeshGround				( nullptr )

	, m_pGround							( nullptr )

	, m_pCamera							( nullptr )

	, DrawFlag							( false )





	, m_pStaticMeshItemBox				( nullptr )

	, m_pItemBoxManager					( nullptr )

	, m_pStaticMesh_TankBodyRed			( nullptr )
	, m_pStaticMesh_TankCannonRed		( nullptr )

	, m_pStaticMesh_TankBodyBlue		( nullptr )
	, m_pStaticMesh_TankCannonBlue		( nullptr )

	//, m_pPlayerManager					( nullptr )

	, m_pTimer							( nullptr )

	, m_pDbgText						( nullptr )

{
	m_SceneType = CSceneType::Debug;
}

CGameDebug::~CGameDebug()
{
	m_hWnd = nullptr;
}

void CGameDebug::Update()
{
	//BGMのループ再生.
	CSoundManager::PlayLoop(CSoundManager::BGM_Title);

	//カメラの移動.
	m_pCamera->FreeMove();
	//アイテムのマネージャー.
	m_pItemBoxManager->Update();

	////プレイヤーのマネージャー.
	//m_pPlayerManager->Update();

	//// Cキー押されたら操作プレイヤー切り替え
	//if (GetAsyncKeyState('C') & 0x0001)
	//{
	//	m_pPlayerManager->SwitchActivePlayer();
	//}

}

void CGameDebug::Draw()
{
	if (DrawFlag == true) {
		return;
	}

	//カメラの描画.
	m_pCamera->Draw();

	//地面の描画.
	m_pGround->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);

	//アイテムボックスの描画.
	m_pItemBoxManager->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);

	////プレイヤーの描画.
	//m_pPlayerManager->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);

	//タイマー描画.
	m_pTimer->Draw();

	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//デバッグアイコンの描画.
	m_pSpriteObj->Draw();
	m_pSpriteTimerObj->Draw();
	CDirectX11::GetInstance().SetDepth(true);
}

void CGameDebug::Init()
{
	//制限時間の定数宣言.
	static constexpr float TIME = 90.f;


	//地面の位置設定.
	m_pGround->SetPosition(0.f, -1.f, 0.f);
	m_pGround->SetRotation(0.f, 0.f, 0.f);
	//カメラの位置.
	m_pCamera->SetCameraPos(30.f, 20.f, 50.f);
	m_pCamera->SetLightPos(-1.5f, 2.f, 5.f);

	//タイトル画像の大きさを設定.
	m_pSpriteObj->SetPosition(0.f, -32.f, 0.f);
	m_pSpriteObj->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteObj->SetScale(1.f, 1.f, 0.f);


	//タイトル画像の大きさを設定.
	m_pSpriteTimerObj->SetPosition(WND_W / 2 - 128, WND_H / 2 - 128, 0.f);
	m_pSpriteTimerObj->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTimerObj->SetScale(0.3f, 0.3f, 0.f);








	//検証用.
	m_pItemBoxManager->SetPosition(0.f, 20.f, 0.f);
	m_pItemBoxManager->SetRotation(0.f, 0.f, 0.f);
	m_pItemBoxManager->SetScale(0.2f, 0.2f, 0.2f);
	m_pItemBoxManager->SetItemInfo();



	//制限時間の文字サイズ.
	m_pDbgText->SetFontSize(5.0f);


	//ゲームで遊べる(クリア画面に遷移する)時間※引数.
	m_pTimer->StartTimer(TIME);
	m_pTimer->SetDebugFont(m_pDbgText);
	m_pTimer->SetTimerPosition(WND_W / 2, 0);



}

void CGameDebug::Destroy()
{

}

void CGameDebug::Create()
{
	//スプライトの生成.
	m_pSpriteDebugIcon	= std::make_shared<CSprite2D>();
	m_pSpriteTimer		= std::make_shared<CSprite2D>();

	//スプライトオブジェクトの生成.
	m_pSpriteObj		= std::make_shared<CUIObject>();
	m_pSpriteTimerObj	= std::make_shared<CUIObject>();

	//スタティックメッシュのインスタンス作成
	m_pStaticMeshGround	= std::make_shared<CStaticMesh>();

	//地面クラスのインスタンス作成.
	m_pGround			= std::make_shared<CGround>();

	//カメラクラスのインスタンス作成.
	m_pCamera			= std::make_shared<CCamera>();






	//検証用.
	//スタティックメッシュのインスタンス生成.
	m_pStaticMeshItemBox	= std::make_shared<CStaticMesh>();
	//アイテムボックスのインスタンス生成.
	m_pItemBoxManager		= std::make_shared<CItemBoxManager>();
	m_pItemBoxManager->Create();

	//戦車のインスタンス生成(赤と青).
	m_pStaticMesh_TankBodyRed	= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonRed = std::make_shared<CStaticMesh>();

	m_pStaticMesh_TankBodyBlue		= std::make_shared<CStaticMesh>();
	m_pStaticMesh_TankCannonBlue	= std::make_shared<CStaticMesh>();

	////プレイヤーマネージャーのインスタンス生成.
	//m_pPlayerManager = std::make_shared<CPlayerManager>();
	//m_pPlayerManager->Initialize();


	//デバッグテキストのインスタンス作成]
	m_pDbgText = std::make_unique<CDebugText>();

	//タイマーのインスタンス作成.
	m_pTimer = std::make_shared<CTimer>();

}

HRESULT CGameDebug::LoadData()
{
	//デバッグテキストの読み込み.
	if (FAILED(m_pDbgText->Init(CDirectX11::GetInstance())))
	{
		return E_FAIL;
	}
	//デバッグ画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		128,128,		//描画幅,高さ.
		128,128,		//元画像の幅,高さ.
		128,128			//アニメーションをしないので、0でいい.
	};
	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE TIMER_SIZE = {
		256,256,		//描画幅,高さ.
		256,256,		//元画像の幅,高さ.
		256,256			//アニメーションをしないので、0でいい.
	};

	//スタティックメッシュの読み込み
	m_pStaticMeshGround	->Init(_T("Data\\Mesh\\Static\\Ground\\ground.x"));

	//デバッグスプライトの読み込み.
	m_pSpriteDebugIcon->Init(_T("Data\\Texture\\Image\\InDebug.png"), WH_SIZE);
	//タイマースプライトの読み込み.
	m_pSpriteTimer->Init(_T("Data\\Texture\\Image\\Timer.png"), TIMER_SIZE);


	//画像の設定.
	m_pSpriteObj->AttachSprite(m_pSpriteDebugIcon);
	m_pSpriteTimerObj->AttachSprite(m_pSpriteTimer);



	//地面の画像設定.
	m_pGround->AttachMesh(m_pStaticMeshGround);








	//検証用.
	//スタティックメッシュの読み込み.
	m_pStaticMeshItemBox->Init(_T("Data\\Mesh\\Static\\ItemBox\\ItemBox.x"));
	//アイテムボックスの画像設定.
	m_pItemBoxManager->AttachMesh(m_pStaticMeshItemBox);


	//デバック時にも戦車を検証できるようにしておきたい
	m_pStaticMesh_TankBodyRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Body\\Body.x"));
	m_pStaticMesh_TankCannonRed->Init(_T("Data\\Mesh\\Static\\Tank\\Red\\Cannon\\Cannon.x"));

	// 戦車(青)
	m_pStaticMesh_TankBodyBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Body\\Body.x"));
	m_pStaticMesh_TankCannonBlue->Init(_T("Data\\Mesh\\Static\\Tank\\Blue\\Cannon\\Cannon.x"));
	////プレイヤーマナージャーにアタッチ.
	//m_pPlayerManager->AttachMeshesToPlayer(0, m_pStaticMesh_TankBodyRed,  m_pStaticMesh_TankCannonRed);
	//m_pPlayerManager->AttachMeshesToPlayer(1, m_pStaticMesh_TankBodyBlue, m_pStaticMesh_TankCannonBlue);












	return S_OK;
}