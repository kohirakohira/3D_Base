#include "CSceneManager.h"

CSceneManager::CSceneManager(HWND hWnd )
	: m_pScene				( nullptr )

	, m_hWnd				( hWnd )

	, m_Flag				( false )
	
	, m_CurrentSceneType	()
	, m_NextType			()
{
	//中身がなかったらタイトルからスタート.
	if (m_pScene == nullptr)
	{
		m_CurrentSceneType = CSceneType::Title;
		Change(std::make_shared<CGameTitle>(m_hWnd));
	}
}

CSceneManager::~CSceneManager()
{
	m_hWnd	= nullptr;
}

//動作処理.
void CSceneManager::Update()
{
	//シーン切り替え.
	m_NextType = m_pScene->GetSceneType();
	if (m_NextType != m_CurrentSceneType)
	{
		m_CurrentSceneType = m_NextType;
		switch (m_NextType)
		{
		case CSceneType::Title:		//タイトル.
			Change(std::make_shared<CGameTitle>(m_hWnd));
			break;
		case CSceneType::Main:		//ゲームメイン.
			Change(std::make_shared<CGameMain>(m_hWnd));
			break;
		case CSceneType::None:
			//何もない.
			break;
		}
	}
	//中にシーンが入っていたら、そのUpdateを読み込む.
	if (m_pScene != nullptr)
	{
		m_pScene->Update();
	}

}

//描画処理.
void CSceneManager::Draw()
{
	//中にシーンが入っていたら、そのDrawを読み込む.
	if (m_pScene != nullptr)
	{
		m_pScene->Draw();
	}
}

//データのロード処理.
HRESULT CSceneManager::LoadData()
{
	if (m_pScene != nullptr)
	{
		m_pScene->LoadData();
	}
	return S_OK;
}

//インスタンス生成.
void CSceneManager::Create()
{
	if (m_pScene != nullptr)
	{
		m_pScene->Create();
	}
}

//シーンの変更処理.
void CSceneManager::Change(std::shared_ptr<CSceneBase> InScene)
{
	if (m_pScene != nullptr)
	{
		//中身を削除.
		m_pScene->Destroy();
	}

	//シーンを設定する.
	m_pScene = InScene;

	//インスタンス生成.
	m_pScene->Create();

	//データの読み込み.
	m_pScene->LoadData();

	//シーンの情報を読み込む.
	m_pScene->Init();
}
