#pragma once

//ライブラリ
#include <algorithm>

//COM 戦車用の状態遷移だけを担当するクラス
//CComPlayerから距離やターゲットの有無などの情報だけを受け取って状態を決める

class CComStateMachine
{
public:
	//COMの行動状態
	enum class State
	{
		Seek,		//探索
		Chase,		//追跡
		Attack,		//攻撃
		Evade,		//離脱
		ItemSeek,	//アイテム探索
	};

	//1フレームごとにCComPlayer側から渡してもらう観測値
	struct Observation
	{
		float dist2 = 0.0f;			//ターゲットとの距離2乗
		bool hasTarget = false;		//ターゲットがいるかどうか
		int lostSightFrames = 0;	//ターゲットを見失ってからのフレーム
	};

	CComStateMachine();

	//現在の状態と、その状態に入ってからの経過フレーム
	State GetState() const { return m_State; }

private:
	//遷移の閾値をまとめた構造体
	struct Params
	{
		//初期値はとりあえず動くようにするだけなので、あとで上書きする前提
		float attackEnter2 = 25.0f;		// 5m^2
		float attackExit2 = 49.0f;		// 7m^2
		float evadeDist2 = 4.0f;		// 2m^2
		int   loseFrames = 120;			//13
	};
};