#include "CGameResult.h"
#include "CSoundManager.h"

//前方宣言.
#include "CSprite2D.h"

CGameResult::CGameResult(HWND hWnd)
	: m_hWnd				( hWnd )

	, m_pSpriteResult		( nullptr )
	, m_pSpriteSelect		( nullptr )
	, m_SpriteChoice		( nullptr )

	, m_pSpriteObj			( nullptr )

	, m_pStaticMeshFighter	( nullptr )
	, m_pStaticMeshGround	( nullptr )
	, m_pStaticMeshCloud	( nullptr )

	, m_pPlayerManager		( nullptr )
	, m_pGround				( nullptr )

	, m_pCamera				( nullptr )

	, m_pSelectIcon			( nullptr )

	, DrawFlag				( false )

	, m_Key					( nullptr )
{
	//結果がどっちか判定.
	WinOrDrawFunction();
}

CGameResult::~CGameResult()
{
	m_hWnd = nullptr;
}

void CGameResult::Update()
{
	//BGMのループ再生.
	CSoundManager::PlayLoop(CSoundManager::BGM_Title);


	//↓-----タイトルでの演出-----↓.
	m_Key->Update();


	//↑-----タイトルでの演出-----↑.


	if (m_Key->ReleaseInputKey('Z') == true)
	{
		//BGMのループ停止.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		//SEの再生.
		CSoundManager::PlaySE(CSoundManager::SE_Click);

		DrawFlag = true;

		m_SceneType = CSceneType::Title;
	}
}

void CGameResult::Draw()
{
	if (DrawFlag == true) {
		return;
	}

	//カメラの描画.
	m_pCamera->Draw();

	//プレイヤーの描画.
	m_pPlayerManager->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);

	//地面の描画.
	m_pGround->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);


	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//リザルト画像の描画.
	m_pSpriteObj->Draw();
	//選択画像の描画.
	m_pSelectIcon->Draw();
	CDirectX11::GetInstance().SetDepth(true);
}

void CGameResult::Init()
{
	//カメラの位置.
	m_pCamera->SetCameraPos(-1.5f, 1.5f, 14.f);
	m_pCamera->SetLightPos(-1.5f, 2.f, 5.f);
	//位置の設定.
	m_pPlayerManager->SetPosition(0.f, 1.f, 6.f);

	//大きさを設定.
	m_pSpriteObj->SetPosition(WND_W / 2 - 640, 0.f, 0.f);
	m_pSpriteObj->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteObj->SetScale(0.7f, 0.7f, 0.f);

	//大きさを設定.
	m_pSelectIcon->SetPosition(WND_W / 2 - 128, WND_H / 1.5 + 128, 0.f);
	m_pSelectIcon->SetRotation(0.f, 0.f, 0.f);
	m_pSelectIcon->SetScale(1.f, 1.f, 1.f);


	//キー設定.
	m_Key->Init();
	m_Key->SetKey({ 'Z' });


}

void CGameResult::Destroy()
{

}

void CGameResult::Create()
{
	//スプライトの生成.
	m_pSpriteResult = std::make_shared<CSprite2D>();
	m_pSpriteSelect = std::make_shared<CSprite2D>();
	m_SpriteChoice	= std::make_shared<CSprite2D>();

	//スプライトオブジェクトの生成.
	m_pSpriteObj = std::make_shared<CUIObject>();

	//スタティックメッシュのインスタンス作成
	m_pStaticMeshFighter = std::make_shared<CStaticMesh>();
	m_pStaticMeshGround = std::make_shared<CStaticMesh>();
	m_pStaticMeshCloud = std::make_shared<CStaticMesh>();

	//キャラクタークラスのインスタンス作成
	m_pPlayerManager = std::make_shared<CPlayerManager>();

	//地面クラスのインスタンス作成.
	m_pGround = std::make_shared<CGround>();
	//地面に設定する.
	m_pGround->SetPlayer(*m_pPlayerManager);

	//カメラクラスのインスタンス作成.
	m_pCamera = std::make_shared<CCamera>();

	//選択画像のインスタンス生成.
	m_pSelectIcon = std::make_shared<CSelectIcon>();
	//選択肢のインスタンス生成.
	m_pChoiceIcon = std::make_shared<CChoiceImage>(m_SceneType);

	//キーインプット.
	m_Key = std::make_shared<CMultiInputKeyManager>();



}

HRESULT CGameResult::LoadData()
{
	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		WND_W,WND_H,		//描画幅,高さ.
		WND_W,WND_H,		//元画像の幅,高さ.
		WND_W,WND_H			//アニメーションをしないので、0でいい.
	};
	//選択肢画像のスプライト設定.
	CSprite2D::SPRITE_STATE C_SIZE = {
		16,32,		//描画幅,高さ.
		16,32,		//元画像の幅,高さ.
		16,32,		//アニメーションをしないので、0でいい.
	};
	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE SELECT_SIZE = {
		256, 96,		//描画幅,高さ.
		256, 96,		//元画像の幅,高さ.
		256, 96			//アニメーションをしないので、0でいい.
	};

	//選択肢スプライトの読み込み.
	m_SpriteChoice->Init(_T("Data\\Texture\\UI\\Choice.png"), C_SIZE);

	//スタティックメッシュの読み込み
	m_pStaticMeshFighter->Init(_T("Data\\Mesh\\Static\\Fighter\\Fighter.x"));
	m_pStaticMeshGround->Init(_T("Data\\Mesh\\Static\\Ground\\ground.x"));

	m_pStaticMeshCloud->Init(_T("Data\\Mesh\\Static\\Object\\cloud.x"));

	//勝ちか引き分けの画像読み込み.
	if (m_SceneType == CSceneType::ResultWin)
	{
		//独り勝ちスプライトの読み込み.
		m_pSpriteResult->Init(_T("Data\\Texture\\Image\\Clear.png"), WH_SIZE);
	}
	else if (m_SceneType == CSceneType::ResultDraw)
	{
		//引き分けスプライトの読み込み.
		m_pSpriteResult->Init(_T("Data\\Texture\\Image\\Over.png"), WH_SIZE);
	}

	//選択画像のスプライトの読み込み.
	m_pSpriteSelect->Init(_T("Data\\Texture\\UI\\TitleSelectImg.png"), SELECT_SIZE);

	//画像の設定.
	m_pSpriteObj->AttachSprite(m_pSpriteResult);

	//選択画像の設定.
	m_pSelectIcon->AttachSprite(m_pSpriteSelect);
	//選択肢画像の設定.
	m_pChoiceIcon->AttachSprite(m_SpriteChoice);

	//キャラクターにアタッチ.
	m_pPlayerManager->AttachMesh(m_pStaticMeshFighter);
	m_pGround->AttachMesh(m_pStaticMeshGround);

	return S_OK;
}

CSceneType CGameResult::WinOrDrawFunction()
{
	//仮で作ってみた.
	//正直考え直したほうがいいかも?

	//一人で勝っていたら.
	if (NoMajic <= 0)
	{
		m_SceneType = CSceneType::ResultWin;
	}
	//同じキル数なら.
	if (NoMajic > 0)
	{
		m_SceneType = CSceneType::ResultDraw;
	}
	return m_SceneType;
}
