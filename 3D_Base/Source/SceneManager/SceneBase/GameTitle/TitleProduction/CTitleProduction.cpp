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
	m_Ground->Move(3.0f);
}

//描画関数.
void CTitleProduction::Draw()
{
	//ライトの設定
	m_Camera->SetLightPos(30.f, 70.f, 0.f);	//ポジション
	m_Camera->SetLightIntensity(150.f);		//ライトの強さ
	m_Camera->SetLightRange(1e9);			//影響範囲.距離減衰
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

//初期化.
void CTitleProduction::Init()
{
	//カメラの初期化.
	m_Camera->Init();

	//地面の初期化.
	m_Ground->Init();
	
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