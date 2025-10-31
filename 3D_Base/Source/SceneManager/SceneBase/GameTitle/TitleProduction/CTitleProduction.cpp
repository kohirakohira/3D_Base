#include "CTitleProduction.h"

CTitleProduction::CTitleProduction()
	: m_Camera				( nullptr )
	, m_GroundMesh			( nullptr )
	, m_PlayerMesh			( nullptr )

	, m_Ground				( nullptr )
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

	//地面を動かす.
	m_Ground->Update();
}

//描画関数.
void CTitleProduction::Draw()
{
	//カメラの情報更新.
	m_Camera->Info();

	//地面を描画する.
	m_Ground->Draw(m_Camera->m_mView, m_Camera->m_mProj, m_Camera->m_Light, m_Camera->m_Camera);
}

//生成関数.
void CTitleProduction::Create()
{
	//カメラ生成.
	m_Camera = std::make_shared<CCamera>();

	//地面の生成.
	m_Ground = std::make_unique<CGround>();

	//スタティックメッシュの生成.
	m_GroundMesh = std::make_shared<CStaticMesh>();

}

//読み込み関数.
HRESULT CTitleProduction::LoadData()
{
	//地面のメッシュの読み込み.
	m_GroundMesh->Init(_T("Data\\Mesh\\Static\\Ground\\ground.x"));
	//地面メッシュのアタッチ.
	m_Ground->AttachMesh(m_GroundMesh);

	return S_OK;
}