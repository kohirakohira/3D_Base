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
	//キル数の追加・取得.
	void AddKillCount(int playerID, int amount = 1);
	int GetKillCount(int playerID) const ;

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