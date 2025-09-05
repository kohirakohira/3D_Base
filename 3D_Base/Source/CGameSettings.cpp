#include "CGameSettings.h"
#include "CSoundManager.h"

//前方宣言.
#include "CSprite2D.h"

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

	, m_pSpriteSettingImg			( nullptr )
	, m_pSpriteSettingBackGroundImg ( nullptr )

	, m_pSpriteStartImg				( nullptr )

	, m_pSpriteChoiceImg			( nullptr )
	, m_pSpriteYesSelectImg			( nullptr )
	, m_pSpriteNoSelectImg			( nullptr )

	, DrawFlag						( false )

	, m_InputKey					( nullptr )

{
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
	//定数宣言.
	constexpr float UV_SPEED = 0.001f;



	//BGMのループ再生.
	CSoundManager::PlayLoop(CSoundManager::BGM_Title);

	//背景の動かす速度.
	m_UV.x += UV_SPEED;
	m_UV.y -= UV_SPEED;
	//背景の模様を動かす値を設定.
	m_pSpriteSettingBackGround->SetUVInfomation(m_UV, true);

	//キー入力受付.
	m_InputKey->Update();

	//背景の動かす速度.
	MoveBackGround();

	//背景の市松模様を動かす用※画像の編集必須!!.
	m_pSpriteSettingBackGroundImg->Update();

	//選択肢の移動※仮.
	m_pSpriteChoiceImg->Update();

	//シーンの遷移.
	if (m_InputKey->ReleaseInputKey('Z') == true)
	{
		if (m_pSpriteChoiceImg->GetSelectedFlag() == false)
		{
			//BGMの停止.
			CSoundManager::Stop(CSoundManager::BGM_Title);

			m_SceneType = CSceneType::Title;
			
			return;
		}
		else
		{
			//BGMの停止.
			CSoundManager::Stop(CSoundManager::BGM_Title);

			m_SceneType = CSceneType::Main;

			return;
		}
	}
}

//描画関数.
void CGameSettings::Draw()
{
	if (DrawFlag == true) {
		return;
	}

	//前後関係無視.
	CDirectX11::GetInstance().SetDepth(false);
	//背景の描画.
	m_pSpriteSettingImg->Draw();
	m_pSpriteSettingBackGroundImg->Draw();
	m_pSpriteStartImg->Draw();
	m_pSpriteYesSelectImg->Draw();
	m_pSpriteNoSelectImg->Draw();
	CDirectX11::GetInstance().SetDepth(true);

	//選択肢の描画.
	m_pSpriteChoiceImg->Draw();
}

//初期化関数.
void CGameSettings::Init()
{
	//定数宣言.
	const float windowYW	= WND_W / 4;			//配置をするのにちょうどいい位置※4はマジックナンバー.
	const float windowNW	= WND_W - (32 * 23);	//配置をするのにちょうどいい位置※32(サイズ) * 23個分の位置にする.
	const float POS_X		= WND_W / 1.37f;		//配置をするのにちょうどいい位置に設定※選択肢の位置を「いいえ」の位置に調整した.
	const float POS_Y		= WND_H / 1.2f;			//配置をするのにちょうどいい位置に設定※高さがちょうどよかった.
	const float IMG_POS_X	= WND_W / 4.f;
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


}

//データの読み込み.
HRESULT CGameSettings::LoadData()
{
	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		WND_W,WND_W,		//描画幅,高さ.
		WND_W,WND_W,		//元画像の幅,高さ.
		WND_W,WND_W			//アニメーションをしないので、0でいい.
	};
	//開始する？画像のスプライト設定.
	CSprite2D::SPRITE_STATE ST_SIZE = {
		960,160,			//描画幅,高さ.
		960,160,			//元画像の幅,高さ.
		960,160,			//アニメーションをしないので、0でいい.
	};
	//選択肢画像のスプライト設定.
	CSprite2D::SPRITE_STATE C_SIZE = {
		16,32,				//描画幅,高さ.
		16,32,				//元画像の幅,高さ.
		16,32,				//アニメーションをしないので、0でいい.
	};
	//選択肢画像のスプライト設定.
	CSprite2D::SPRITE_STATE S_SIZE = {
		256,96,				//描画幅,高さ.
		256,96,				//元画像の幅,高さ.
		256,96,				//アニメーションをしないので、0でいい.
	};

	//定数宣言.
	constexpr float ALPHA_TOP = 1.f;
	constexpr float ALPHA_BACK = 0.3f;


	//タイトルスプライトの読み込み.
	m_pSpriteSetting->Init(_T("Data\\Texture\\Image\\Setting.png"), WH_SIZE);
	m_pSpriteSetting->SetAlpha(ALPHA_TOP);
	//タイトル模様スプライトの読み込み.
	m_pSpriteSettingBackGround->Init(_T("Data\\Texture\\Image\\BackGroundImage.png"), WH_SIZE);
	m_pSpriteSettingBackGround->SetAlpha(ALPHA_BACK);
	//開始する？スプライトの読み込み.
	m_pSpriteStart->Init(_T("Data\\Texture\\UI\\Message.png"), ST_SIZE);
	//選択肢スプライトの読み込み.
	m_SpriteChoice->Init(_T("Data\\Texture\\UI\\Choice.png"), C_SIZE);
	//選択スプライトの読み込み.
	m_SpriteYesSelect->Init(_T("Data\\Texture\\UI\\Yes.png"), S_SIZE);
	m_SpriteNoSelect->Init(_T("Data\\Texture\\UI\\No.png"), S_SIZE);

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



	return S_OK;
}

//背景を動かす関数.
void CGameSettings::MoveBackGround()
{
	m_UV.x += UV_SPEED;
	m_UV.y -= UV_SPEED;
	//背景の模様を動かす値を設定.
	m_pSpriteSettingBackGround->SetUVInfomation(m_UV, true);
}