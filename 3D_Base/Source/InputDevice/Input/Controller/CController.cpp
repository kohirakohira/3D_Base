#include "CController.h"

CController::CController(int index)
	: m_Pad				()
	, m_Index			( index )
	, m_Connected		( false )
{
	//中身を生成.
	m_Pad = std::make_unique<CXInput>(index);
}

CController::~CController()
{
	//もし中身があるなら消す.
	if (m_Pad)
	{
		m_Pad->EndProc();
	}
}

//動作関数.
//USBの遅延があるので、Updateは毎回呼ぶべき！！.
void CController::Update()
{
	m_Connected = m_Pad->Update();
}

//接続判定.
bool CController::CheckConnected() const
{
	return m_Connected;
}

//切断処理.
void CController::ControllerAmputation()
{
	if (m_Connected != true)
	{
		m_Pad->EndProc();
	}
}

bool CController::Down(CXInput::KEY key, bool just) const
{
	return m_Pad->IsDown(key, just);
}

bool CController::Up(CXInput::KEY key) const
{
	return m_Pad->IsUp(key);
}

bool CController::Repeat(CXInput::KEY key) const
{
	return m_Pad->IsRepeat(key);
}

//スティックの方向を反映させる※左.
CController::Direction CController::GetLeftStickDirection(float value)
{
	float x = m_Pad->GetLeftStickXNormalized();
	float y = m_Pad->GetLeftStickYNormalized();

	return GetDirectionFromXY(x, y, value);
}
//スティックの方向を反映させる※右.
CController::Direction CController::GetRightStickDirection(float value)
{
	float x = m_Pad->GetRightStickXNormalized();
	float y = m_Pad->GetRightStickYNormalized();

	return GetDirectionFromXY(x, y, value);
}

//XY座標から方向判定する関数.
CController::Direction CController::GetDirectionFromXY(float x, float y, float value)
{
	//デッドゾーン※入力範囲.
	if (std::fabs(x) < value && std::fabs(y) < value)
	{
		return CController::Direction::None;
	}

	//上.
	if (y > value)
	{
		//左上か右上を判定.
		if (x > value)	return CController::Direction::UpRight;
		if (x < -value)	return CController::Direction::UpLeft;
		return CController::Direction::Up;
	}
	//下.
	if (y < -value)
	{
		//左下か右下を判定.
		if (x > value)	return CController::Direction::DownRight;
		if (x < -value)	return CController::Direction::DownLeft;
		return CController::Direction::Down;
	}
	//左右のみ.
	if (x > value)	return CController::Direction::Right;
	if (x < -value) return CController::Direction::Left;

	//どの条件にも当てはまらないとき.
	return CController::Direction::None;
}

float CController::GetLeftStickX() const
{
	return m_Pad->GetLeftStickXNormalized();
}

float CController::GetLeftStickY() const
{
	return m_Pad->GetLeftStickYNormalized();
}

float CController::GetRightStickX() const
{
	return m_Pad->GetRightStickXNormalized();
}

float CController::GetRightStickY() const
{
	return m_Pad->GetRightStickYNormalized();
}

float CController::GetLeftTrigger() const
{
	return m_Pad->GetLeftTriggerNormalized();
}

float CController::GetRightTrigger() const
{
	return m_Pad->GetRightTriggerNormalized();
}

//振動.
void CController::SetVibration(WORD left, WORD right)
{
	m_Pad->SetVibration(left, right);
}