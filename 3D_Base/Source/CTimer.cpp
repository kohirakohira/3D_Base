#include "CTimer.h"
#include <algorithm>	// std::max 用.

//std::maxを使用するとエラーが出たため使用.
using std::max;

CTimer::CTimer()
	: m_Time			( 0 )
	, m_HitTime			()

	, m_StartTimer		()
	, m_TotalTime		( 0.f )
	, m_TimerPosition	()
{
}

CTimer::~CTimer()
{
}

void CTimer::Draw()
{
	//デバッグテキスト(スコア)の描画
	m_pDbgText->SetColor(1.f, 1.f, 1.f);
	m_Time = static_cast<int>(GetRemainingTime());
	TCHAR TimeText[16];
	_stprintf_s(TimeText, _T("%d"), m_Time);
	m_pDbgText->Render(TimeText, m_TimerPosition.x, m_TimerPosition.y);

}

void CTimer::StartTimer(float time)
{
	// タイマーの開始時間を記録する.
	m_StartTimer = std::chrono::steady_clock::now();

	//ゲーム時間を設定する.
	m_TotalTime = std::chrono::duration<float>(time);
}

float CTimer::GetRemainingTime() const
{
	//今の時間を記録する変数.
	//今の時間を取得・保持.
	std::chrono::steady_clock::time_point m_NowTime = std::chrono::steady_clock::now();

	//経過時間を記録する変数.
	//経過時間を求める.
	std::chrono::duration<float> m_ElapsedTime = m_NowTime - m_StartTimer;

	//残り時間を記録する変数.
	//残り時間を求める.
	std::chrono::duration<float> m_RemainingTime = m_TotalTime - m_ElapsedTime;

	return max(0.f, m_RemainingTime.count()); // 残り時間が負にならないようにするため、0と比較して大きい方を返す.
}

// 残り時間が0秒以下かどうかを返す（＝タイマーが終了したか）
bool CTimer::IsFinished() const
{
	return GetRemainingTime() <= 0.0f;
}

//当たったときに必要な関数.
void CTimer::HitTimer()
{
	m_HitTime = std::chrono::steady_clock::now();
}

float CTimer::GetElapsed() const
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed = now - m_HitTime;
	return elapsed.count();
}
