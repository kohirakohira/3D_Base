#include "CTitleProduction.h"

CTitleProduction::CTitleProduction()
	: m_Camera				( nullptr )
	, m_BackGroundMesh		( nullptr )
	, m_SpriteGroundImage	( nullptr )

	, m_Ground				( nullptr )
	, m_pBackImgObject		( nullptr )
	, m_Player				( nullptr )
{
}

CTitleProduction::~CTitleProduction()
{
}

//動作関数.
void CTitleProduction::Update()
{
	//プレイヤーの動作.
	m_Player->Update();

	//カメラの動作.
	m_Camera->FreeMove();

	//地面の動作.
	m_Ground->Update();

}

//描画関数.
void CTitleProduction::Draw()
{
	//カメラの情報更新.
	m_Camera->Info();

	//プレイヤーの描画.
	m_Player->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);
	//背景を描画する.
	m_pBackImgObject->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);

	CDirectX11::GetInstance().SetDepth(false);
	//地面の表示.
	m_Ground->Draw();
	CDirectX11::GetInstance().SetDepth(true);
}

//生成関数.
void CTitleProduction::Create()
{
	//カメラ生成.
	m_Camera = std::make_shared<CCamera>();

	//プレイヤー生成.
	m_Player = std::make_unique<CTitleOnlyPlayer>();

	//地面の生成.
	m_Ground = std::make_unique<CImageObject>();

	//背景の生成.
	m_pBackImgObject = std::make_unique<CStaticMeshObject>();

	//スタティックメッシュの生成.
	m_SpriteGroundImage = std::make_shared<CSprite2D>();
	m_BackGroundMesh	= std::make_shared<CStaticMesh>();

}

//初期化.
void CTitleProduction::Init()
{
	//カメラの初期化.
	m_Camera->Init();
	//カメラの設定.
	m_Camera->SetLightPos(0.0f, 100.0f, 100.0f);	//位置設定.
	m_Camera->SetLightColor(1.0f, 1.0f, 1.0f);		//色の設定.
	m_Camera->SetLightIntensity(900.0f);			//ライトの強さ.
	m_Camera->SetLightRange(1000.0f);				//ライトの長さ.
	m_Camera->SetLightAtten(0.0f, 0.0f, 0.1);		//ライトの減衰.
	m_Camera->SetCameraPos(0.0f, 100.0f, 0.0f);		//カメラ位置の設定.

	//地面の設定.
	m_Ground->SetPosition(0.0f, 0.0f, 0.0f);
	m_Ground->SetRotation(D3DXToRadian(-80.0f), 0.0f, 0.0f);
	m_Ground->SetScale(7.5f, 7.5f, 7.5f);
	
	//背景の初期化.
	m_pBackImgObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pBackImgObject->SetRotation(0.0f, 0.0f, 0.0f);
	m_pBackImgObject->SetScale(28.0f, 28.0f, 28.0f);
}

//読み込み関数.
HRESULT CTitleProduction::LoadData()
{
	//タイトル画像のスプライト設定.
	CSprite2D::SPRITE_STATE WH_SIZE = {
		256,256,		//描画幅,高さ.
		256,256,		//元画像の幅,高さ.
		256,256			//アニメーションをしないので、0でいい.
	};

	//2Dスプライトの画像読み込み.
	m_SpriteGroundImage->Init(_T("Data\\Mesh\\Static\\Ground\\groundex.png"), WH_SIZE, false);
	//背景のメッシュの読み込み.
	m_BackGroundMesh->Init(_T("Data\\Mesh\\Static\\OutBackImage\\BackImage.x"));

	//地面画像のアタッチ.
	m_Ground->AttachSprite(m_SpriteGroundImage);
	//背景メッシュのアタッチ.
	m_pBackImgObject->AttachMesh(m_BackGroundMesh);

	return S_OK;
}