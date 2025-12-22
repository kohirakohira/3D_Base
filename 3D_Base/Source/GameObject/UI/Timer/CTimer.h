#pragma once
#include <iostream>
#include <chrono>		//タイマーを使用する.

//-----デバックテキスト-----
#include "Assets//DebugText//CDebugText.h"		// デバッグテキストクラス

class CTimer
{
public:
	CTimer();
	~CTimer();

	//時間表示.
	void Draw();

	//デバッグ表示(時間)のインスタンス設定.
	void SetDebugFont(std::shared_ptr<CDebugText> debug)
	{
		m_pDbgText = debug;
	}

	//表示位置を設定するようの関数.
	void SetTimerPosition(float x, float y)
	{
		m_TimerPosition.x = x;
		m_TimerPosition.y = y;
	}


	//タイマーの開始.
	void StartTimer(int time);

	//残り時間の取得.
	int GetRemainingTime() const;

	//タイマーが終了したかどうかを返す.
	bool IsFinished() const;

	//ゲーム開始時に必要な関数.
	void CountDown();

	//当たってからの経過時間(無敵時間ととらえてもいい).
	int GetElapsed() const;

public:
	//ゲーム内の時間を保持する変数.
	int m_Time;

	//ゲーム内での表示位置.
	struct Pos
	{
		float x;
		float y;
	}m_TimerPosition;

	// ゲーム開始のに必要な変数.
	std::chrono::steady_clock::time_point	m_StartGameTime;
	// steady_clockが表す特定の時点を扱う型。

	//タイマーの開始時間を記録する.
	std::chrono::steady_clock::time_point	m_StartTimer;

	//デバッグテキスト.
	std::shared_ptr<CDebugText>				m_pDbgText;

	//::duration：時間の長さを表すための型.
	std::chrono::duration<int>				m_TotalTime;	//タイマーの最大時間を記録(ゲーム時間)する.
};