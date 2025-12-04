#include "CTitleProduction.h"

CTitleProduction::CTitleProduction()
	: m_Camera				( nullptr )
	, m_BackGroundMesh		( nullptr )

	, m_SpriteGround		( nullptr )
	, m_SpriteObjGround		( nullptr )
	, m_pBackImgObject		( nullptr )

	, m_UV					( 0.0f, 0.0f )
{
}

CTitleProduction::~CTitleProduction()
{
}

//動作関数.
void CTitleProduction::Update()
{
	//カメラの動作.
	m_Camera->Update();
	//m_Camera->FreeMove();

}

//描画関数.
void CTitleProduction::Draw()
{
	//カメラの情報更新.
	m_Camera->Info();

	//地面を動かす.
	MoveGround();

	//地面を描画する.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
	//背景を描画する.
	m_pBackImgObject->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);

}

//生成関数.
void CTitleProduction::Create()
{
	//カメラ生成.
	m_Camera = std::make_shared<CCamera>();

	//地面の生成.
	m_SpriteGround = std::make_unique<CSprite3D>();

	//地面オブジェクトの生成.
	m_SpriteObjGround = std::make_unique<CSpriteObject>();

	//背景の生成.
	m_pBackImgObject = std::make_unique<CStaticMeshObject>();

	//スタティックメッシュの生成.
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
	m_Camera->SetLightIntensity(1000.0f);			//ライトの強さ.
	m_Camera->SetLightRange(10000.0f);				//ライトの長さ.
	m_Camera->SetLightAtten(0.0f, 0.0f, 0.1);		//ライトの減衰.
	m_Camera->SetCameraPos(0.0f, 100.0f, 0.0f);		//カメラ位置の設定.

	//地面の設定.
	D3DXVECTOR3 angle = { 0.0f, 0.0f, 0.0f };
	angle.x = 90.0f * PI / 180.0f;
	angle.y = 45.0f * PI / 180.0f;
	m_SpriteObjGround->SetRotation(angle);
	m_SpriteObjGround->SetPosition(0.0f, -100.0f, 200.0f);
	m_SpriteObjGround->SetScale(3.0f, 3.0f, 3.0f);

	//背景の初期化.
	m_pBackImgObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pBackImgObject->SetRotation(0.0f, 0.0f, 0.0f);
	m_pBackImgObject->SetScale(50.0f, 50.0f, 50.0f);
}

//読み込み関数.
HRESULT CTitleProduction::LoadData()
{
	//スプライトの読み込みサイズ.
	CSprite3D::SPRITE_STATE GROUND = 
	{
		256,256,
		256,256,
		256,256
	};

	//スプライトの読み込み.
	m_SpriteGround->Init(CDirectX11::GetInstance(), _T("Data\\Mesh\\Static\\Ground\\groundex.bmp"), GROUND);

	//背景のメッシュの読み込み.
	m_BackGroundMesh->Init(_T("Data\\Mesh\\Static\\OutBackImage\\BackImage.x"));

	//地面メッシュのアタッチ.
	m_SpriteObjGround->AttachSprite(*m_SpriteGround);
	//背景メッシュのアタッチ.
	m_pBackImgObject->AttachMesh(m_BackGroundMesh);

	return S_OK;
}

//地面の動き.
void CTitleProduction::MoveGround()
{
	//定数宣言.
	const float UV_SPEED = 0.001f;

	m_UV.x = 0.0f;
	m_UV.y += UV_SPEED;

	m_SpriteGround->SetUVInfomation( m_UV, true );
}