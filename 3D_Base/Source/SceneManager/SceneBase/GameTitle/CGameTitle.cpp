#include "CGameTitle.h"
//-----サウンド-----
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス

//前方宣言.
#include "Assets//Sprite//Sprite2D//CSprite2D.h" // 2Dスプライトクラス.

CGameTitle::CGameTitle(HWND hWnd)
	: m_hWnd(hWnd)

	, m_pSpriteTitle				( nullptr )
	, m_SpriteChoice				( nullptr )
	, m_SpritePlaySelect			( nullptr )
	, m_SpriteEndSelect				( nullptr )

	, m_pSpriteTitleImg				( nullptr )
	, m_pSpriteChoiceImg			( nullptr )
	, m_pSpritePlaySelectImg		( nullptr )
	, m_pSpriteEndSelectImg			( nullptr )

	, m_KeyInput					( nullptr )

	, DrawFlag						( false )

	, m_TitleProduction				( nullptr )

{
	m_SceneType = CSceneType::Title;
	//キル数の初期化.
	CGameDataManager::GetInstance().Init();
}

CGameTitle::~CGameTitle()
{
	m_hWnd = nullptr;
	m_TitleProduction.reset();
}

void CGameTitle::Update()
{
	//コントローラーの取得※0番のみ動かせる.
	CController* controller = CControllerManager::GetInstance().GetController(0);

	// コントローラーの繰り上げ処理を呼び出し
	CControllerManager::GetInstance().Reoderring();

	//フェイクロードの更新.
	FakeLoad::GetInstance().Update();

	//BGMのループ再生.
	CSoundManager::PlayLoop(CSoundManager::BGM_Title);

	//キー入力受付.
	m_KeyInput->Update();

	//選択肢の移動※仮.
	m_pSpriteChoiceImg->Update();

//↓-----タイトルでの演出-----↓.

	//タイトル演出の動作.
	m_TitleProduction->Update();

//↑-----タイトルでの演出-----↑.

	//ゲーム設定に遷移.
	if (controller && controller->CheckConnected() == true || m_KeyInput != nullptr)
	{
		if (m_KeyInput->ReleaseInputKey('Z') == true || controller && controller->Down(CXInput::A, true))
		{
			if (m_pSpriteChoiceImg->GetSelectedFlag() == false)
			{
				//決定SEの再生.
				CSoundManager::PlaySE(CSoundManager::SE_Click);

				DrawFlag = true;

				//ゲーム設定に遷移.
				m_SceneType = CSceneType::Setting;

				//ロード画像の表示.
				FakeLoad::GetInstance().SetIsDraw(true);

			}
			else
			{
				//BGMのループ停止.
				CSoundManager::Stop(CSoundManager::BGM_Title);

				//決定SEの再生.
				CSoundManager::PlaySE(CSoundManager::SE_Click);

				DrawFlag = true;


				//ゲームを終了させる.
				PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			}
		}
	}

#if 0
		////デバッグに遷移.
		//if (m_KeyInput->ReleaseInputKey('D') == true)
		//{
		//	//BGMのループ停止.
		//	CSoundManager::Stop(CSoundManager::BGM_Title);

		//	DrawFlag = true;

		//	m_SceneType = CSceneType::Debug;

		//}
		////メインに遷移.
		//if (m_KeyInput->ReleaseInputKey('M') == true)
		//{
		//	//BGMのループ停止.
		//	CSoundManager::Stop(CSoundManager::BGM_Title);

		//	DrawFlag = true;

		//	m_SceneType = CSceneType::Main;

		//}
		////リザルトDrawに遷移.
		//if (m_KeyInput->ReleaseInputKey('L') == true)
		//{
		//	//BGMのループ停止.
		//	CSoundManager::Stop(CSoundManager::BGM_Title);

		//	DrawFlag = true;

		//	m_SceneType = CSceneType::ResultDraw;

		//}
		////リザルトwinに遷移.
		//if (m_KeyInput->ReleaseInputKey('Y') == true)
		//{
		//	//BGMのループ停止.
		//	CSoundManager::Stop(CSoundManager::BGM_Title);

		//	DrawFlag = true;

		//	m_SceneType = CSceneType::ResultWin;

		//}
#endif

	if (m_KeyInput->ReleaseInputKey('1') == true)
	{
		//ゲームスタートSEの再生(仮).
		CSoundManager::PlaySE(CSoundManager::SE_GameStart);
	}

	if (m_KeyInput->ReleaseInputKey('2') == true)
	{
		//ゲーム終了SEの再生(仮).
		CSoundManager::PlaySE(CSoundManager::SE_GameEnd);
	}

	if (m_KeyInput->ReleaseInputKey('3') == true)
	{
		//シャッターSEの再生(仮).
		CSoundManager::PlaySE(CSoundManager::SE_Door);
	}

	if (m_KeyInput->ReleaseInputKey('4') == true)
	{
		//花火SEの再生(仮).
		CSoundManager::PlaySE(CSoundManager::SE_FireWork);
	}

	if (m_KeyInput->ReleaseInputKey('5') == true)
	{
		//火花SEの再生(仮).
		CSoundManager::PlaySE(CSoundManager::SE_Spark);
	}
}

void CGameTitle::Draw()
{

	if (DrawFlag == true) {
		return;
	}

	//フェイクロードの描画.
	FakeLoad::GetInstance().Draw();

	//タイトル演出を表示.
	m_TitleProduction->Draw();

	//演出用.
	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//タイトルの描画.
	m_pSpriteTitleImg->Draw();
	//選択肢の描画.
	m_pSpritePlaySelectImg->Draw();
	m_pSpriteEndSelectImg->Draw();
	CDirectX11::GetInstance().SetDepth(true);

	//選択肢の描画.
	m_pSpriteChoiceImg->Draw();
}

void CGameTitle::Init()
{	
	//フェイクロード画像の設定.
	FakeLoad::GetInstance().Init();

	//定数宣言.
	const float POS_X		= WND_W / 8.f;
	const float POS_Y		= WND_H / 1.2f;
	const float SL_POS_X	= WND_W / 4.3f;
	const float SL_POS_Y	= WND_H / 1.5f;

	//大きさを設定.
	m_pSpriteTitleImg->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteTitleImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteTitleImg->SetScale(0.5f, 0.5f, 0.f);
	//選択肢画像の大きさを設定.
	m_pSpriteChoiceImg->SetPosition(SL_POS_X, SL_POS_Y, 0.f);
	m_pSpriteChoiceImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteChoiceImg->SetScale(1.f, 1.f, 0.f);
	//選択肢画像の大きさを設定.
	m_pSpritePlaySelectImg->SetPosition(POS_X, SL_POS_Y, 0.f);
	m_pSpritePlaySelectImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpritePlaySelectImg->SetScale(1.f, 1.f, 0.f);
	//選択肢画像の大きさを設定.
	m_pSpriteEndSelectImg->SetPosition(POS_X, POS_Y, 0.f);
	m_pSpriteEndSelectImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteEndSelectImg->SetScale(1.f, 1.f, 0.f);
		
	//タイトルの演出.
	m_TitleProduction->Init();

	//キー入力の初期化.
	m_KeyInput->Init();
	m_KeyInput->SetKey({'D', 'Z', 'L', 'Y', 'M', '1', '2', '3', '4', '5', });
}

void CGameTitle::Destroy()
{

}

void CGameTitle::Create()
{
	//スプライトの生成.
	m_pSpriteTitle		= std::make_shared<CSprite2D>();
	m_SpriteChoice		= std::make_shared<CSprite2D>();
	m_SpritePlaySelect	= std::make_shared<CSprite2D>();
	m_SpriteEndSelect	= std::make_shared<CSprite2D>();

	//スプライトオブジェクトの生成.
	m_pSpriteTitleImg		= std::make_shared<CUIObject>();
	//選択肢のスプライトオブジェクトの生成.
	m_pSpriteChoiceImg		= std::make_shared<CChoiceImage>(m_SceneType);
	//選択肢のスプライトオブジェクトの生成.
	m_pSpritePlaySelectImg	= std::make_shared<CUIObject>();
	m_pSpriteEndSelectImg	= std::make_shared<CUIObject>();

	//タイトル演出クラス.
	m_TitleProduction = std::make_unique <CTitleProduction> ();
	m_TitleProduction->Create();

	//キー入力.
	m_KeyInput = std::make_shared<CMultiInputKeyManager>();

}

HRESULT CGameTitle::LoadData()
{
	//フェイクロード画像の読み込み.
	FakeLoad::GetInstance().LoadData();

	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		WND_W,WND_H,		//描画幅,高さ.
		WND_W,WND_H,		//元画像の幅,高さ.
		WND_W,WND_H			//アニメーションをしないので、0でいい.
	};
	//選択肢画像のスプライト設定.
	CSprite2D::SPRITE_STATE C_SIZE = {
		40,80,		//描画幅,高さ.
		40,80,		//元画像の幅,高さ.
		40,80,		//アニメーションをしないので、0でいい.
	};
	//選択画像のスプライト設定.
	CSprite2D::SPRITE_STATE SL_SIZE = {
		256,96,		//描画幅,高さ.
		256,96,		//元画像の幅,高さ.
		256,96,		//アニメーションをしないので、0でいい.
	};

	//演出用のデータを読み込む.
	m_TitleProduction->LoadData();

	//タイトルスプライトの読み込み.
	m_pSpriteTitle->Init( _T("Data\\Texture\\Image\\Title.png"), WH_SIZE, false);
	//選択肢スプライトの読み込み.
	m_SpriteChoice->Init(_T("Data\\Texture\\UI\\Select\\Choice.png"), C_SIZE, false);
	//選択肢スプライトの読み込み.
	m_SpritePlaySelect->Init(_T("Data\\Texture\\UI\\Select\\PlaySelectImg.png"), SL_SIZE, false);
	m_SpriteEndSelect->Init(_T("Data\\Texture\\UI\\Select\\EndSelectImg.png"), SL_SIZE, false);

	//画像の設定.
	m_pSpriteTitleImg->AttachSprite(m_pSpriteTitle);
	//画像の設定.
	m_pSpriteChoiceImg->AttachSprite(m_SpriteChoice);
	//画像の設定.
	m_pSpritePlaySelectImg->AttachSprite(m_SpritePlaySelect);
	m_pSpriteEndSelectImg->AttachSprite(m_SpriteEndSelect);
	
	return S_OK;
}