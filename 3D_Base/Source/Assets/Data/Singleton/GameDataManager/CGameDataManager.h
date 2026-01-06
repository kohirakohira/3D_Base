#pragma once
//-----STL-----.
#include <iostream>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <vector>
#include <utility>

//-----基底クラス-----.
#include "../CSingleton.h"

//引き分け判断用.
struct DrawResult
{
	int Kill;					//引き分け時のキル数.
	std::vector<int> players;	//引き分けしているプレイヤーID.
};

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

	//1位の取得.
	std::pair<int, int> GetTopCharacter();

	//順位を決めて取得する関数.
	std::array<int, PLAYER_MAX> GetRanking();

	//同じキル数なのかを判定する関数.
	bool SameKill();

	//勝ちor引き分け・情報取得関数※引数：.
	bool WinOrDrawJudgment(DrawResult outDraw);
	
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