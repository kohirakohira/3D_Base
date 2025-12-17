#include "CControllerManager.h"
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャー

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
			// 切断された瞬間だけ
			if (controller->IsJustDisconnected())
			{
				// コントローラーの切断SE
				CSoundManager::PlaySE(CSoundManager::SE_UnConnect);
			}

			controller->ControllerAmputation();
		}
	}

#ifdef _DEBUG
	//std::cout << "接続された数->" << m_ConnectedCount << std::endl;
	//std::cout << "切断された数->" << PLAYER_MAX - m_ConnectedCount << std::endl;
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

//=====コントローラーの繰り上げ処理=====
void CControllerManager::Reoderring()
{
	int nextSlot = 0;

	for (int index = 0; index < PLAYER_MAX; ++index)
	{
		CController* ctrl = m_Controller[index].get();

		// 接続されていないコントローラーは無視
		if (!ctrl->CheckConnected())
		{
			continue;
		}

		// すでに正しい位置にいる
		if (index == nextSlot)
		{
			nextSlot++;
			continue;
		}

		// --- 繰り上げを行う ---
		// swap で unique_ptr の入れ替えを行う
		std::swap(m_Controller[index], m_Controller[nextSlot]);

		nextSlot++;
	}
}
//===================================
