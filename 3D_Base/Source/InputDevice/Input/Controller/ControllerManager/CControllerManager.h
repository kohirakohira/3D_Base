#pragma once
//STL.
#include <iostream>
#include <vector>
#include <memory>

//コントローラークラス.
#include "InputDevice\Input\Controller\CController.h"

//シングルトン基底クラス.
#include "../../../../Assets/Data/Singleton/CSingleton.h"

//=======================================================
// コントローラーマネージャー※シングルトン化.
//=======================================================
class CControllerManager
	: public CSingleton<CControllerManager>
{
public:
	//シングルトン化.
	static CControllerManager& GetInstance()
	{
		//唯一のインスタンス.
		static CControllerManager instance;
		return instance;
	}

	//コピー・ムーブ禁止※安全対策.
	//必要性：所有権・リソース管理の安全.
	CControllerManager(const CControllerManager&) = delete;					//コンストラクタのコピー禁止.
	CControllerManager& operator = (const CControllerManager&) = delete;	//コピー代入禁止.
	CControllerManager(CControllerManager&&) = delete;						//コンストラクタのムーブ禁止.
	CControllerManager& operator = (const CControllerManager&&) = delete;	//ムーブ代入禁止.

private:
	CControllerManager();
	~CControllerManager();

public:
	//動作関数.
	void Update();

	//接続されている数を取得.
	int GetConnectedCount() const { return m_ConnectedCount; }

	//指定番号のコントローラー取得.
	CController* GetController(int index)
	{
		//プレイヤーの数分のインスタンスを取得.
		if (index < 0 || index >= PLAYER_MAX)
		{
			return nullptr;
		}

		//接続チェック.
		if (m_Controller[index] == nullptr || m_Controller[index]->CheckConnected() != true)
		{
			return nullptr;
		}

		//生ポインタを返す.
		return m_Controller[index].get();
	}

	//全コントローラーの切断処理.
	void AllControllerAmputation();

private:
	//メンバ変数.
	std::vector<std::unique_ptr<CController>>	m_Controller;	//全プレイヤー.
	int m_ConnectedCount;	//接続されているコントローラーを数える用.
};