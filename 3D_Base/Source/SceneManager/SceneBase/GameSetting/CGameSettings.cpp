#include "CGameSettings.h"

//-----外部クラス-----
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス

//前方宣言.
#include "Assets//Sprite//Sprite2D//CSprite2D.h" // 2Dスプライトクラス.

//定数宣言.
constexpr float UV_SPEED = 0.001f;


CGameSettings::CGameSettings(HWND hWnd)
	: m_hWnd(hWnd)

	, m_pSpriteSetting				( nullptr )
	, m_pSpriteSettingBackGround	( nullptr )
	, m_pSpriteStart				( nullptr )
	, m_SpriteChoice				( nullptr )

	, m_SpriteYesSelect				( nullptr )
	, m_SpriteNoSelect				( nullptr )


	, m_SpriteConnection			()
	, m_SpriteCom					()

	, m_SpriteConnectionImg			()
	, m_SpriteConnectionCOMImg		()

	, m_pSpriteSettingImg			( nullptr )
	, m_pSpriteSettingBackGroundImg ( nullptr )

	, m_pSpriteStartImg				( nullptr )

	, m_pSpriteChoiceImg			( nullptr )
	, m_pSpriteYesSelectImg			( nullptr )
	, m_pSpriteNoSelectImg			( nullptr )

	, DrawFlag						( false )

	, m_InputKey					( nullptr )

{
	//ロード画像の表示.
	FakeLoad::GetInstance().SetIsDraw(false);

	m_SceneType = CSceneType::Setting;
	m_UV = { 0.1f, 0.1f };
}

CGameSettings::~CGameSettings()
{
	m_hWnd = nullptr;
}

//動作関数.
void CGameSettings::Update()
{
	//フェイクロード画像の更新.
	FakeLoad::GetInstance().Update();

	//コントローラーの取得※0番のみ動かせる.
	CController* controller = CControllerManager::GetInstance().GetController(0);

	// コントローラーの繰り上げ処理を呼び出し
	CControllerManager::GetInstance().Reoderring();

	//キー入力受付.
	m_InputKey->Update();

	//背景の動かす速度.
	MoveBackGround();

	//背景の市松模様を動かす用.
	m_pSpriteSettingBackGroundImg->Update();

	//選択肢の移動※仮.
	m_pSpriteChoiceImg->Update();

	//シーンの遷移.
	if (controller && controller->CheckConnected() || m_InputKey != nullptr)
	{
		if (m_InputKey->ReleaseInputKey('Z') == true || controller && controller->Down(CXInput::A, true))
		{
			//BGMの停止.
			CSoundManager::Stop(CSoundManager::BGM_Title);

			if (m_pSpriteChoiceImg->GetSelectedFlag() == false)
			{
				m_SceneType = CSceneType::Title;

				//決定SEの再生.
				CSoundManager::PlaySE(CSoundManager::SE_Click);
				return;
			}
			else
			{
				m_SceneType = CSceneType::Main;

				//決定SEの再生.
				CSoundManager::PlaySE(CSoundManager::SE_Click);
				return;
			}
		}
	}
}

//描画関数.
void CGameSettings::Draw()
{
	if (DrawFlag == true) {
		return;
	}

	//フェイクロードの描画.
	FakeLoad::GetInstance().Draw();

	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//背景の描画.
	m_pSpriteSettingImg->Draw();
	m_pSpriteSettingBackGroundImg->Draw();
	m_pSpriteStartImg->Draw();



	//画像インスタンスの複製.
	for (int i = 0; i < IMAGE; i++)
	{
		// 1Pは常に表示し続ける
		m_SpriteConnectionImg[0]->Draw();

		//コントローラーの取得
		if (CControllerManager::GetInstance().GetController(i))
		{
			// 準備完了
			m_SpriteConnectionImg[i]->Draw();
		}
		else
		{
			//準備中.
			m_SpriteConnectionCOMImg[i]->Draw();
		}
	}

	m_pSpriteYesSelectImg->Draw();
	m_pSpriteNoSelectImg->Draw();
	CDirectX11::GetInstance().SetDepth(true);

	//選択肢の描画.
	m_pSpriteChoiceImg->Draw();
}

//初期化関数.
void CGameSettings::Init()
{
	//フェイクロード画像の設定.
	FakeLoad::GetInstance().Init();

	//定数宣言.
	const float windowYW	= WND_W / 5;			//配置をするのにちょうどいい位置※4はマジックナンバー.
	const float windowNW	= WND_W - (32 * 23);	//配置をするのにちょうどいい位置※32(サイズ) * 23個分の位置にする.
	const float POS_X		= WND_W / 1.325f;		//配置をするのにちょうどいい位置に設定※選択肢の位置を「いいえ」の位置に調整した.
	const float POS_Y		= WND_H / 1.25f;		//配置をするのにちょうどいい位置に設定※高さがちょうどよかった.
	const float IMG_POS_X	= WND_W / 5.f;
	const float IMG_POS_Y	= WND_H / 14.f;

	//タイトル画像の大きさ・位置・回転を設定.
	m_pSpriteSettingImg->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteSettingImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteSettingImg->SetScale(1.f, 1.f, 0.f);
	//タイトル画像の背景の大きさ・位置・回転を設定.
	m_pSpriteSettingBackGroundImg->SetPosition(0.f, 0.f, 0.f);
	m_pSpriteSettingBackGroundImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteSettingBackGroundImg->SetScale(1.f, 1.f, 0.f);
	//タイトル画像の背景の大きさ・位置・回転を設定.
	m_pSpriteStartImg->SetPosition(IMG_POS_X, IMG_POS_Y, 0.f);
	m_pSpriteStartImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteStartImg->SetScale(1.f, 1.f, 0.f);
	//選択肢画像の大きさ・位置・回転を設定.
	//初期位置は「いいえ」の位置.
	m_pSpriteChoiceImg->SetPosition(POS_X, POS_Y, 0.f);
	m_pSpriteChoiceImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteChoiceImg->SetScale(1.f, 1.f, 0.f);
	//選択画像の大きさ・位置・回転を設定.
	m_pSpriteYesSelectImg->SetPosition(windowYW, POS_Y, 0.f);
	m_pSpriteYesSelectImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteYesSelectImg->SetScale(1.f, 1.f, 0.f);
	//選択画像の大きさ・位置・回転を設定.
	m_pSpriteNoSelectImg->SetPosition(windowNW, POS_Y, 0.f);
	m_pSpriteNoSelectImg->SetRotation(0.f, 0.f, 0.f);
	m_pSpriteNoSelectImg->SetScale(1.f, 1.f, 0.f);

	//画像の設定.
	for (int i = 0; i < IMAGE; i++)
	{
		m_SpriteConnectionImg[i]->SetPosition(posx, posy, 0.0f);
		m_SpriteConnectionImg[i]->SetRotation(0.0f, 0.0f, 0.0f);
		m_SpriteConnectionImg[i]->SetScale(1.0f, 1.0f, 0.f);
		
		m_SpriteConnectionCOMImg[i]->SetPosition(posx, posy, 0.0f);
		m_SpriteConnectionCOMImg[i]->SetRotation(0.0f, 0.0f, 0.0f);
		m_SpriteConnectionCOMImg[i]->SetScale(1.0f, 1.0f, 0.f);

		posx += 450.0f;
	}

	//キー入力.
	m_InputKey->Init();
	//使いたいキーを引数に設定.
	m_InputKey->SetKey({'Z'});
}

//解放関数.
void CGameSettings::Destroy()
{

}

//インスタンス作成関数.
void CGameSettings::Create()
{
	//スプライトの生成.
	m_pSpriteSetting			= std::make_shared<CSprite2D>();
	m_pSpriteSettingBackGround	= std::make_shared<CSprite2D>();
	m_pSpriteStart				= std::make_shared<CSprite2D>();
	m_SpriteChoice				= std::make_shared<CSprite2D>();
	m_SpriteYesSelect			= std::make_shared<CSprite2D>();
	m_SpriteNoSelect			= std::make_shared<CSprite2D>();

	//スプライトオブジェクトの生成.
	m_pSpriteSettingImg = std::make_shared<CImageObject>();

	//スプライトオブジェクトの生成.
	m_pSpriteSettingBackGroundImg = std::make_shared<CImageObject>();

	//スプライトオブジェクトの生成.
	m_pSpriteStartImg = std::make_shared<CImageObject>();

	//スプライトオブジェクトの生成.
	m_pSpriteStartImg			  = std::make_shared<CImageObject>();

	//選択肢のスプライトオブジェクトの生成.
	m_pSpriteChoiceImg		= std::make_shared<CChoiceImage>(m_SceneType);
	m_pSpriteYesSelectImg	= std::make_shared<CSelectIcon>();
	m_pSpriteNoSelectImg	= std::make_shared<CSelectIcon>();

	//キー入力.
	m_InputKey = std::make_shared<CMultiInputKeyManager>();

	//接続画像のインスタンス生成.
	for (int i = 0; i < IMAGE; i++)
	{
		m_SpriteConnection.push_back(std::make_shared<CSprite2D>());
		m_SpriteCom.push_back(std::make_shared<CSprite2D>());
	}

	//画像インスタンスの複製.
	for (int i = 0; i < IMAGE; i++)
	{
		m_SpriteConnectionImg.push_back(std::make_shared<CImageObject>());
	}
	for (int i = 0; i < IMAGE; i++)
	{
		m_SpriteConnectionCOMImg.push_back(std::make_shared<CImageObject>());
	}
}

//データの読み込み.
HRESULT CGameSettings::LoadData()
{
	//フェイクロード画像の読み込み.
	FakeLoad::GetInstance().LoadData();

	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		WND_W,WND_W,		//描画幅,高さ.
		WND_W,WND_W,		//元画像の幅,高さ.
		WND_W,WND_W			//アニメーションをしないので、0でいい.
	};
	//開始する？画像のスプライト設定.
	CSprite2D::SPRITE_STATE ST_SIZE = {
		1216,224,			//描画幅,高さ.
		1216,224,			//元画像の幅,高さ.
		1216,224,			//アニメーションをしないので、0でいい.
	};
	//選択肢画像のスプライト設定.
	CSprite2D::SPRITE_STATE C_SIZE = {
		40,80,		//描画幅,高さ.
		40,80,		//元画像の幅,高さ.
		40,80,		//アニメーションをしないので、0でいい.
	};
	//選択肢のスプライト設定.
	CSprite2D::SPRITE_STATE Select_SIZE = {
		320,160,		//描画幅,高さ.
		320,160,		//元画像の幅,高さ.
		320,160,		//アニメーションをしないので、0でいい.
	};
	//接続確認画像のスプライト設定.
	CSprite2D::SPRITE_STATE S_SIZE = {
		352,384,	//描画幅,高さ.
		352,384,	//元画像の幅,高さ.
		352,384,	//アニメーションをしないので、0でいい.
	};

	//定数宣言.
	constexpr float ALPHA_TOP = 1.f;
	constexpr float ALPHA_BACK = 0.3f;


	//接続確認画像の読み込み.
	for (int i = 0; i < IMAGE; i++)
	{
		switch (i)
		{
		case 0:
			m_SpriteConnection[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\1P_Ready.png"), S_SIZE, false);
			break;
		case 1:
			m_SpriteConnection[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\2P_Ready.png"), S_SIZE, false);
			break;
		case 2:
			m_SpriteConnection[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\3P_Ready.png"), S_SIZE, false);
			break;
		case 3:
			m_SpriteConnection[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\4P_Ready.png"), S_SIZE, false);
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < IMAGE; i++)
	{
		switch (i)
		{
		case 0 :
			m_SpriteCom[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\COM_Yellow.png"), S_SIZE, false);
			break;
		case 1 :
			m_SpriteCom[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\COM_Yellow.png"), S_SIZE, false);
			break;
		case 2 :
			m_SpriteCom[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\COM_Green.png"), S_SIZE, false);
			break;
		case 3 :
			m_SpriteCom[i]->Init(_T("Data\\Texture\\UI\\SettingImg\\COM_Blue.png"), S_SIZE, false);
			break;
		default:
			break;
		}
	}

	//タイトルスプライトの読み込み.
	m_pSpriteSetting->Init(_T("Data\\Texture\\Image\\Setting.png"), WH_SIZE, false);
	m_pSpriteSetting->SetAlpha(ALPHA_TOP);
	//タイトル模様スプライトの読み込み.
	m_pSpriteSettingBackGround->Init(_T("Data\\Texture\\Image\\BackGroundImage.png"), WH_SIZE, false);
	m_pSpriteSettingBackGround->SetAlpha(ALPHA_BACK);
	//開始する？スプライトの読み込み.
	m_pSpriteStart->Init(_T("Data\\Texture\\UI\\Select\\Message.png"), ST_SIZE, false);
	//選択肢スプライトの読み込み.
	m_SpriteChoice->Init(_T("Data\\Texture\\UI\\Select\\Choice.png"), C_SIZE, false);
	//選択スプライトの読み込み.
	m_SpriteYesSelect->Init(_T("Data\\Texture\\UI\\Select\\Yes.png"), Select_SIZE, false);
	m_SpriteNoSelect->Init(_T("Data\\Texture\\UI\\Select\\No.png"), Select_SIZE, false);

	//画像の設定(設定画像).
	m_pSpriteSettingImg->AttachSprite(m_pSpriteSetting);
	//画像の設定(設定画像の背景).
	m_pSpriteSettingBackGroundImg->AttachSprite(m_pSpriteSettingBackGround);
	//画像の設定(開始する？画像).
	m_pSpriteStartImg->AttachSprite(m_pSpriteStart);
	//画像の設定(選択肢).
	m_pSpriteChoiceImg->AttachSprite(m_SpriteChoice);
	//画像の設定(選択※Yes&No).
	m_pSpriteYesSelectImg->AttachSprite(m_SpriteYesSelect);
	m_pSpriteNoSelectImg->AttachSprite(m_SpriteNoSelect);


	//画像設定.
	for (int i = 0; i < IMAGE; i++)
	{
		switch (i)
		{
		case 0:
			m_SpriteConnectionImg[i]->AttachSprite(m_SpriteConnection[i]);
			m_SpriteConnectionCOMImg[i]->AttachSprite(m_SpriteCom[i]);
			break;
		case 1:
			m_SpriteConnectionImg[i]->AttachSprite(m_SpriteConnection[i]);
			m_SpriteConnectionCOMImg[i]->AttachSprite(m_SpriteCom[i]);
			break;
		case 2:
			m_SpriteConnectionImg[i]->AttachSprite(m_SpriteConnection[i]);
			m_SpriteConnectionCOMImg[i]->AttachSprite(m_SpriteCom[i]);
			break;
		case 3:
			m_SpriteConnectionImg[i]->AttachSprite(m_SpriteConnection[i]);
			m_SpriteConnectionCOMImg[i]->AttachSprite(m_SpriteCom[i]);
			break;
		default:
			break;
		}
	}

	return S_OK;
}

//背景を動かす関数.
void CGameSettings::MoveBackGround()
{
	//定数宣言.
	const float UV_SPEED = 0.001f;

	m_UV.x += UV_SPEED;
	m_UV.y -= UV_SPEED;
	//背景の模様を動かす値を設定.
	m_pSpriteSettingBackGround->SetUVInfomation(m_UV, true);
}