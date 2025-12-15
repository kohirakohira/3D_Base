#include "CGameDataManager.h"

CGameDataManager::CGameDataManager()
	: m_KillCount		()
{
}

CGameDataManager::~CGameDataManager()
{
}

//AddKillCount(プレイヤーID, キル数).
void CGameDataManager::AddKillCount(int playerID, int amount)
{
	//指定されたプレイヤーのキル数を増やす.
	m_KillCount[playerID] += amount;
}

int CGameDataManager::GetKillCount(int playerID) const
{
	//指定されたプレイヤーのキル数を取得.
	//KillCount内でplayerIDが存在するか検索.
	auto it = m_KillCount.find(playerID);

	//見つかればキル数を返し、そうでない時は0を返す.
	if (it != m_KillCount.end())
	{
		//2つ目(キル数)の要素を返す.
		return it->second;
	}
	else
	{
		//playerIDが登録されていなければ0.
		return 0;
	}
}

//キル数を初期化.
void CGameDataManager::Init()
{
	//全初期化.
	m_KillCount.clear();
}

//同じキル数なのかを判定する関数.
bool CGameDataManager::SameKill()
{
	//最大キル数.
	int maxKill = 0;

	//最大キル数を取得.
	for (const auto& pair : m_KillCount)
	{
		if (pair.second > maxKill)
		{
			//全員の最大キル数.
			maxKill = pair.second;
		}
	}

	//最大キル数の人数.
	int maxCount = 0;

	//最大キル数の人数を数える.
	for (const auto& pair : m_KillCount)
	{
		//最大キル数が同じ人がいるか.
		if (pair.second == maxKill)
		{
			//最大キル数が同じだから加算.
			maxCount++;
		}
	}

	//もし最大キル数が同じならfalse、一人だけならtrue.
	return (maxCount == 1);
}
