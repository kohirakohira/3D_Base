#include "CGameResult.h"

//-----サウンド-----
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス

//-----エフェクト-----
#include "../../../Assets/Effect/CEffect.h"

//前方宣言.
#include "Assets//Sprite//Sprite2D//CSprite2D.h" // 2Dスプライトクラス.

CGameResult::CGameResult(HWND hWnd)
	: m_hWnd				( hWnd )

	, m_pSpriteResult		( nullptr )
	, m_pSpriteSelect		( nullptr )
	, m_SpriteChoice		( nullptr )

	, m_pSpriteObj			( nullptr )

	, m_pStaticMeshFighter	( nullptr )
	, m_pStaticMeshGround	( nullptr )
	, m_pStaticMeshCloud	( nullptr )

	, m_pCharacterManager	( nullptr )
	, m_pGround				( nullptr )

	, m_pCamera				( nullptr )

	, m_pSelectIcon			( nullptr )

	, DrawFlag				( false )

	, m_Key					( nullptr )

	, m_pResultProduction	( nullptr )
	, m_WinOrDraw			( )
{
	//インスタンス生成.
	m_pResultProduction = std::make_unique<CResultProduction>();
	//結果がどっちか判定.
	WinOrDrawFunction();
}

CGameResult::~CGameResult()
{
	m_hWnd = nullptr;
	m_pResultProduction = nullptr;
}

void CGameResult::Update()
{
	//コントローラーの取得※0番のみ動かせる.
	CController* controller = CControllerManager::GetInstance().GetController(0);

	// コントローラーの繰り上げ処理を呼び出し
	CControllerManager::GetInstance().Reoderring();
	
	//キーの判定.
	m_Key->Update();

	switch (m_SceneType)
	{
	case CSceneType::ResultWin:
		//BGMのループ再生.
		CSoundManager::PlayLoop(CSoundManager::BGM_Result_Win);
		//↓-----リザルトの演出-----↓.

		soundcount--;
		if (soundcount <= 0.0f)
		{
			// SEの再生
			CSoundManager::PlaySE(CSoundManager::SE_FireWork);

			soundcount = 120.f;
		}

		m_pResultProduction->WinUpdate();

		//↑-----リザルトでの演出-----↑.
		break;
	case CSceneType::ResultDraw:
		//BGMのループ再生.
		CSoundManager::PlayLoop(CSoundManager::BGM_Result_Draw);
		//↓-----リザルトでの演出-----↓.

		m_pResultProduction->DrawUpdate();

		//↑-----リザルトでの演出-----↑.
		break;
	default:
		break;
	}

	//シーンの遷移.
	if (controller && controller->CheckConnected() || m_Key != nullptr)
	{
		if (m_Key->ReleaseInputKey('Z') == true || controller && controller->Down(CXInput::A, true))
		{
			//BGMのループ停止.
			CSoundManager::Stop(CSoundManager::BGM_Result_Win);
			CSoundManager::Stop(CSoundManager::BGM_Result_Draw);

			//SEの再生.
			CSoundManager::PlaySE(CSoundManager::SE_Click);

			DrawFlag = true;

			m_SceneType = CSceneType::Title;
		}
	}
}

void CGameResult::Draw()
{
	if (DrawFlag == true) {
		return;
	}

	switch (m_SceneType)
	{
	case CSceneType::ResultWin:
		//BGMのループ再生.
		CSoundManager::PlayLoop(CSoundManager::BGM_Result_Win);
		//↓-----リザルトの演出-----↓.

		m_pResultProduction->WinDraw();

		//↑-----リザルトでの演出-----↑.
		break;
	case CSceneType::ResultDraw:
		//BGMのループ再生.
		CSoundManager::PlayLoop(CSoundManager::BGM_Result_Draw);
		//↓-----リザルトでの演出-----↓.

		m_pResultProduction->DrawDraw();

		//↑-----リザルトでの演出-----↑.
		break;
	default:
		break;
	}



	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//リザルト画像の描画.
	m_pSpriteObj->Draw();
	//選択画像の描画.
	m_pSelectIcon->Draw();
	//選択肢画像の描画.
	m_pChoiceIcon->Draw();
	CDirectX11::GetInstance().SetDepth(true);

	//エフェクトの表示.
	CDirectX11::GetInstance().SetAlphaBlend(true);
	CEffect::GetInstance().Draw(
		m_pResultProduction->GetCamera()->m_mView,
		m_pResultProduction->GetCamera()->m_mProj,
		m_pResultProduction->GetCamera()->m_Light,
		m_pResultProduction->GetCamera()->m_Camera);
	CDirectX11::GetInstance().SetAlphaBlend(false);

}

void CGameResult::Init()
{
	//初期化.
	m_pResultProduction->Init(m_WinOrDraw);

	//カメラの位置.
	m_pCamera->SetCameraPos(-1.5f, 1.5f, 14.f);
	m_pCamera->SetLightPos(-1.5f, 2.f, 5.f);

	//大きさを設定.
	m_pSpriteObj->SetPosition(0.0f, 0.0f, 0.0f);
	m_pSpriteObj->SetRotation(0.0f, 0.0f, 0.0f);
	m_pSpriteObj->SetScale(1.0f, 1.0f, 1.0f);

	//大きさを設定.
	m_pSelectIcon->SetPosition(WND_W / 2 - 128, WND_H / 1.5 + 128, 0.0f);
	m_pSelectIcon->SetRotation(0.0f, 0.0f, 0.0f);
	m_pSelectIcon->SetScale(1.0f, 1.0f, 1.0f);
	
	//選択肢の情報.
	m_pChoiceIcon->SetPosition(WND_W / 2 + 96, WND_H / 1.5 + 128, 0.0f);
	m_pChoiceIcon->SetRotation(0.0f, 0.0f, 0.0f);
	m_pChoiceIcon->SetScale(1.0f, 1.0f, 1.0f);

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
	m_pCharacterManager = std::make_shared<CCharacterManager>();

	//地面クラスのインスタンス作成.
	m_pGround = std::make_shared<CGround>();

	//カメラクラスのインスタンス作成.
	m_pCamera = std::make_shared<CCamera>();

	//選択画像のインスタンス生成.
	m_pSelectIcon = std::make_shared<CSelectIcon>();
	//選択肢のインスタンス生成.
	m_pChoiceIcon = std::make_shared<CChoiceImage>(m_SceneType);

	//キーインプット.
	m_Key = std::make_shared<CMultiInputKeyManager>();

	//リザルトの演出.
	m_pResultProduction->Create();

}

HRESULT CGameResult::LoadData()
{
	//リザルト演出のデータ読み取り.
	m_pResultProduction->LoadData();

	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		WND_W,WND_H,		//描画幅,高さ.
		WND_W,WND_H,		//元画像の幅,高さ.
		WND_W,WND_H			//アニメーションをしないので、0でいい.
	};
	//選択肢画像のスプライト設定.
	CSprite2D::SPRITE_STATE C_SIZE = {
		24,40,		//描画幅,高さ.
		24,40,		//元画像の幅,高さ.
		24,40,		//アニメーションをしないので、0でいい.
	};
	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE SELECT_SIZE = {
		256, 96,		//描画幅,高さ.
		256, 96,		//元画像の幅,高さ.
		256, 96			//アニメーションをしないので、0でいい.
	};

	//選択肢スプライトの読み込み.
	m_SpriteChoice->Init(_T("Data\\Texture\\UI\\Select\\Choice.png"), C_SIZE, false);

	//スタティックメッシュの読み込み
	m_pStaticMeshFighter->Init(_T("Data\\Mesh\\Static\\Fighter\\Fighter.x"));
	m_pStaticMeshGround->Init(_T("Data\\Mesh\\Static\\Ground\\ground.x"));

	m_pStaticMeshCloud->Init(_T("Data\\Mesh\\Static\\Object\\cloud.x"));

	//勝ちか引き分けの画像読み込み.
	if (m_SceneType == CSceneType::ResultWin)
	{
		//独り勝ちスプライトの読み込み.
		m_pSpriteResult->Init(_T("Data\\Texture\\Image\\Winner.png"), WH_SIZE, false);
	}
	else if (m_SceneType == CSceneType::ResultDraw)
	{
		//引き分けスプライトの読み込み.
		m_pSpriteResult->Init(_T("Data\\Texture\\Image\\Draw.png"), WH_SIZE, false);
	}

	//選択画像のスプライトの読み込み.
	m_pSpriteSelect->Init(_T("Data\\Texture\\UI\\Select\\TitleSelectImg.png"), SELECT_SIZE, false);

	//画像の設定.
	m_pSpriteObj->AttachSprite(m_pSpriteResult);

	//選択画像の設定.
	m_pSelectIcon->AttachSprite(m_pSpriteSelect);
	//選択肢画像の設定.
	m_pChoiceIcon->AttachSprite(m_SpriteChoice);

	//キャラクターにアタッチ.
	m_pCharacterManager->AttachMesh(m_pStaticMeshFighter);
	m_pGround->AttachMesh(m_pStaticMeshGround);

	return S_OK;
}

CSceneType CGameResult::WinOrDrawFunction()
{
	//判定用.
	bool IsSingleWin = CGameDataManager::GetInstance().WinOrDrawJudgment(m_WinOrDraw);

	//一人で勝っていたら.
	if (IsSingleWin == true)
	{
		m_SceneType = CSceneType::ResultWin;
		m_pResultProduction->SetIsJudge(true);
	}
	else
	{
		m_SceneType = CSceneType::ResultDraw;
		m_pResultProduction->SetIsJudge(false);
	}

	return m_SceneType;
}
