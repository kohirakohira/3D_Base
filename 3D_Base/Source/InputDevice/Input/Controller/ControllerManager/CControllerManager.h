#pragma once
//STL.
#include <iostream>
#include <vector>
#include <memory>

//コントローラークラス.
#include "InputDevice\Input\Controller\CController.h"

//=======================================================
// コントローラーマネージャー.
//=======================================================
class CControllerManager
{
public:
	CControllerManager();
	~CControllerManager();

	//動作関数.
	void Update();

	//接続されている数を取得.
	int GetConnectedCount() const { return m_ConnectedCount; }

private:
	//メンバ変数.
	std::vector<std::unique_ptr<CController>>	m_Controller;	//全プレイヤー.
	int m_ConnectedCount;	//接続されているコントローラーを数える用.
};