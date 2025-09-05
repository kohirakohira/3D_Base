#include "CGame.h"
#include "CSoundManager.h"

//コンストラクタ.
CGame::CGame(HWND hWnd )
	: m_hWnd			( hWnd )
	, m_pSceneManager	( nullptr )
{
	//インスタンスの作成.
	m_pSceneManager = std::make_shared<CSceneManager>(m_hWnd);
}


//デストラクタ.
CGame::~CGame()
{

}

//構築.
void CGame::Create()
{
	//各シーンのCreate関数.
	m_pSceneManager->Create();
}

//ロードデータ関数.
HRESULT CGame::LoadData()
{
	//サウンドデータの読み込み
	if (CSoundManager::GetInstance()->Load(m_hWnd) == false) {
		return E_FAIL;
	}

	//各シーンでのロードデータ関数.
	m_pSceneManager->LoadData();

	return S_OK;
}

//解放関数.
void CGame::Release()
{

}


//更新処理.
void CGame::Update()
{
	//各シーンのUpdate関数.
	m_pSceneManager->Update();
}

//描画処理.
void CGame::Draw()
{
	//各シーンのDraw関数.
	m_pSceneManager->Draw();
}

//GitHub確認用