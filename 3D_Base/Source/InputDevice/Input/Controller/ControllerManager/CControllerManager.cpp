#include "CControllerManager.h"

CControllerManager::CControllerManager()
	: m_ConnectedCount			( 0 )
{
	//プレイヤー分コントローラーを作成.
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		m_Controller.push_back(std::make_unique<CController>(i));
	}
}

CControllerManager::~CControllerManager()
{
}

//動作関数.
//全コントローラーの状態を確認する.
void CControllerManager::Update()
{
	//毎回カウントを初期化.
	m_ConnectedCount = 0;
	for (auto& controller : m_Controller)
	{
		controller->Update();

		if (controller->CheckConnected() == true)
		{
			m_ConnectedCount++;
		}
		else
		{
			controller->ControllerAmputation();
		}
	}

#ifdef _DEBUG
	std::cout << "接続された数->" << m_ConnectedCount << std::endl;
	std::cout << "切断された数->" << PLAYER_MAX - m_ConnectedCount << std::endl;
#endif

}

//全コントローラーの切断処理.
void CControllerManager::AllControllerAmputation()
{
	for (auto& controller : m_Controller)
	{
		controller->ControllerAmputation();
	}
	m_ConnectedCount = 0;
}
