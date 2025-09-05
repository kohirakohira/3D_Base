#include "CGameTitle.h"
#include "CSoundManager.h"

//前方宣言.
#include "CSprite2D.h"

CGameTitle::CGameTitle(HWND hWnd)
	: m_hWnd(hWnd)

	, m_pSpriteTitle		( nullptr )
	, m_SpriteChoice		( nullptr )
	, m_SpritePlaySelect	( nullptr )
	, m_SpriteEndSelect		( nullptr )

	, m_pSpriteTitleImg		( nullptr )
	, m_pSpriteChoiceImg	( nullptr )
	, m_pSpritePlaySelectImg( nullptr )
	, m_pSpriteEndSelectImg	( nullptr )

	, m_KeyInput			( nullptr )

	, m_pCamera				( nullptr )

	, DrawFlag				( false )


	, m_pGround				( nullptr ) 

	, m_StaticMeshGround	( nullptr )









{
	m_SceneType = CSceneType::Title;
}

CGameTitle::~CGameTitle()
{
	m_hWnd = nullptr;
}

void CGameTitle::Update()
{
	//BGMのループ再生.
	CSoundManager::PlayLoop(CSoundManager::BGM_Title);







	//カメラの位置を決める用で仮追加.
	m_pCamera->FreeMove();








	//キー入力受付.
	m_KeyInput->Update();

	//選択肢の移動※仮.
	m_pSpriteChoiceImg->Update();
//↓-----タイトルでの演出-----↓.

	//回転させてみた.
	rad += 0.5;

	m_pGround->SetRotation(rad, rad, rad);













//↑-----タイトルでの演出-----↑.



	//デバッグに遷移.
	if (m_KeyInput->ReleaseInputKey('D') == true)
	{
		//BGMのループ停止.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::Debug;

	}
	//メインに遷移.
	if (m_KeyInput->ReleaseInputKey('M') == true)
	{
		//BGMのループ停止.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::Main;

	}
	//リザルトDrawに遷移.
	if (m_KeyInput->ReleaseInputKey('L') == true)
	{
		//BGMのループ停止.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::ResultDraw;

	}
	//リザルトwinに遷移.
	if (m_KeyInput->ReleaseInputKey('Y') == true)
	{
		//BGMのループ停止.
		CSoundManager::Stop(CSoundManager::BGM_Title);

		DrawFlag = true;

		m_SceneType = CSceneType::ResultWin;

	}

	//ゲーム設定に遷移.
	if (m_KeyInput->ReleaseInputKey('Z') == true)
	{
		if (m_pSpriteChoiceImg->GetSelectedFlag() == false)
		{
			//BGMのループ停止.
			CSoundManager::Stop(CSoundManager::BGM_Title);

			//SEの再生.
			CSoundManager::PlaySE(CSoundManager::SE_Click);

			DrawFlag = true;
			
			//ゲーム設定に遷移.
			m_SceneType = CSceneType::Setting;
		}
		else
		{
			//BGMのループ停止.
			CSoundManager::Stop(CSoundManager::BGM_Title);

			//SEの再生.
			CSoundManager::PlaySE(CSoundManager::SE_Click);

			DrawFlag = true;

			//ゲームを終了させる.
			PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		}
	}
}

void CGameTitle::Draw()
{
	if (DrawFlag == true) {
		return;
	}

	//演出用.
	//カメラの描画.
	m_pCamera->Draw();

	//地面の描画.
	m_pGround->Draw(m_pCamera->m_mView, m_pCamera->m_mProj, m_pCamera->m_Light, m_pCamera->m_Camera);


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
	//定数宣言.
	const float POS_X		= WND_W / 8.f;
	const float POS_Y		= WND_H / 1.2f;
	const float SL_POS_X	= WND_W / 4.2f;
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
		
	//キー入力の初期化.
	m_KeyInput->Init();
	m_KeyInput->SetKey({'D', 'Z', 'L', 'Y', 'M'});




	//演出用.
	//カメラの位置設定.
	m_pCamera->SetCameraPos(0.f, 0.f, 0.f);
	m_pCamera->SetLightRot(1.f, 5.f, 10.f);

	//地面の設定.
	m_pGround->SetPosition(0.f,0.f,0.f);

	m_pGround->SetScale (1.f,1.f,1.f);





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

	//カメラクラスのインスタンス作成.
	m_pCamera = std::make_shared<CCamera>();

	//キー入力.
	m_KeyInput = std::make_shared<CMultiInputKeyManager>();



	//演出用.
	//地面クラスのインスタンス生成.
	m_pGround = std::make_shared<CGround>();
	//地面のメッシュのインスタンス生成.
	m_StaticMeshGround = std::make_shared<CStaticMesh>();




}

HRESULT CGameTitle::LoadData()
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
	//選択画像のスプライト設定.
	CSprite2D::SPRITE_STATE SL_SIZE = {
		256,96,		//描画幅,高さ.
		256,96,		//元画像の幅,高さ.
		256,96,		//アニメーションをしないので、0でいい.
	};

	//タイトルスプライトの読み込み.
	m_pSpriteTitle->Init( _T("Data\\Texture\\Image\\Title.png"), WH_SIZE);
	//選択肢スプライトの読み込み.
	m_SpriteChoice->Init(_T("Data\\Texture\\UI\\Choice.png"), C_SIZE);
	//選択肢スプライトの読み込み.
	m_SpritePlaySelect->Init(_T("Data\\Texture\\UI\\PlaySelectImg.png"), SL_SIZE);
	m_SpriteEndSelect->Init(_T("Data\\Texture\\UI\\EndSelectImg.png"), SL_SIZE);

	//画像の設定.
	m_pSpriteTitleImg->AttachSprite(m_pSpriteTitle);
	//画像の設定.
	m_pSpriteChoiceImg->AttachSprite(m_SpriteChoice);
	//画像の設定.
	m_pSpritePlaySelectImg->AttachSprite(m_SpritePlaySelect);
	m_pSpriteEndSelectImg->AttachSprite(m_SpriteEndSelect);



	//演出用.
	//地面画像の読み込み.
	m_StaticMeshGround->Init(_T("Data\\Mesh\\Static\\Stage\\stage.x"));
	
	
	//地面画像の設定.
	m_pGround->AttachMesh(m_StaticMeshGround);










	return S_OK;
}