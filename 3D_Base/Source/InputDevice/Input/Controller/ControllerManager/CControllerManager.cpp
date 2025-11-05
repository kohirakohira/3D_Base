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
	m_ConnectedCount = 0;
	for (auto& controller : m_Controller)
	{
		controller->Update();

		if (controller->CheckConnected())
		{
			m_ConnectedCount++;
		}
		else
		{
			//controller->
		}
	}
}