#include "CTitleProduction.h"

CTitleProduction::CTitleProduction()
	: m_Camera				( nullptr )
	, m_GroundMesh			( nullptr )
	, m_PlayerMesh			( nullptr )

	, m_SpriteGround		( nullptr )
	, m_SpriteObjGround		( nullptr )
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

}

//描画関数.
void CTitleProduction::Draw()
{
	//カメラの情報更新.
	m_Camera->Info();

	//地面を描画する.
	m_SpriteObjGround->Draw(m_Camera->m_mView, m_Camera->m_mProj);
}

//生成関数.
void CTitleProduction::Create()
{
	//カメラ生成.
	m_Camera = std::make_shared<CCamera>();

	//地面の生成.
	m_SpriteGround = std::make_unique<CSprite3D>();

	//スタティックメッシュの生成.
	m_GroundMesh = std::make_shared<CStaticMesh>();

	//地面オブジェクトの生成.
	m_SpriteObjGround = std::make_unique<CSpriteObject>();

}

//読み込み関数.
HRESULT CTitleProduction::LoadData()
{
	//スプライトの読み込みサイズ.
	CSprite3D::SPRITE_STATE GROUND
	{

	};

	//地面のメッシュの読み込み.
	m_GroundMesh->Init(_T("Data\\Mesh\\Static\\Ground\\ground.x"));
	//スプライトの読み込み.
	m_SpriteGround->Init(CDirectX11::GetInstance(), _T(""), GROUND);

	//地面メッシュのアタッチ.
	m_SpriteObjGround->AttachSprite(*m_SpriteGround);

	return S_OK;
}