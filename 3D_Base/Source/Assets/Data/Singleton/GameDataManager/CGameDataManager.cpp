#include "CGameDataManager.h"

CGameDataManager::CGameDataManager()
	: m_KillCount		()
{
	m_KillCount[0] = 99;
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
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		m_KillCount[i] = 0;
	}
}

//1位の取得.
std::pair<int, int> CGameDataManager::GetTopCharacter()
{
	//プレイヤーIDが0からスタートするので-1
	//プレイヤーIDの保持用.
	int maxPlayerID = -1;
	//キル数の最大値の保持用.
	int maxKills = 0;

	for (const auto& [playerID, kills] : m_KillCount) {
		//キャラのキル数が最大キル数より多いとき.
		if (kills > maxKills) {
			//キル数とプレイヤーIDを保持する.
			maxKills = kills;
			maxPlayerID = playerID;
		}
	}
	return { maxPlayerID, maxKills };
}

//順位を決めて取得する関数.
std::array<int, PLAYER_MAX> CGameDataManager::GetRanking()
{
	//結果を格納する変数※.fill：指定された値で埋める.
	std::array<int, PLAYER_MAX> result{};
	result.fill(-1);
	//選択済みか判別するための変数s.
	std::array<bool, PLAYER_MAX> selected{};
	selected.fill(false);

	//プレイヤーの順位決め.
	for (int rank = 0; rank < PLAYER_MAX; rank++)
	{
		//プレイヤーの中で一番キル数が多いID保持用.
		int firstID		= -1;
		//その時のキル数保持用.
		int firstKill	= -1;

		for (auto& [id, kill] : m_KillCount)
		{
			//例外処理.
			if (id < 0 || id >= PLAYER_MAX)
			{
				continue;
			}
			//選択されていたらスキップ.
			if (selected[id])
			{
				continue;
			}
			//未選択、または今までよりキル数が多ければ更新する.
			if (firstID == -1 || kill > firstKill)
			{
				firstID		= id;
				firstKill	= kill;
			}
		}
		//順位決定.
		if (firstID != -1)
		{
			result[rank] = firstID;
			selected[firstID] = true;	//選択済みにする.
		}
	}

	return result;
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
