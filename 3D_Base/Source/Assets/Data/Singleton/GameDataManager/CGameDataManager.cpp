#include "CGameDataManager.h"

CGameDataManager::CGameDataManager()
	: m_KillCount		()
{
}

CGameDataManager::~CGameDataManager()
{
}

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