#pragma once
//-----STL-----.
#include <iostream>
#include <unordered_map>

//-----基底クラス-----.
#include "../CSingleton.h"

//=================================================================
//		データ保存クラス※シングルトン化.
//=================================================================
class CGameDataManager
	: public CSingleton<CGameDataManager>
{
public:
	//キル数の追加.
	void AddKillCount(int playerID, int amount = 1);
	//キル数の取得.
	int GetKillCount(int playerID) const ;
	//キル数を初期化.
	void Init();

private:
	//friend：クラスや関数に「private/protectedメンバへのアクセス権」を与えることができる.
	friend class CSingleton<CGameDataManager>;
	CGameDataManager();
	~CGameDataManager();

private:
	//保存データ.
	//m_KillCount：(プレイヤーID,キル数).
	std::unordered_map<int, int> m_KillCount;
};