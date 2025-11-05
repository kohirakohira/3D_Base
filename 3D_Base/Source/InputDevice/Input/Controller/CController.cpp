#include "CController.h"

CController::CController(int index)
	: m_Pad				()
	, m_Index			( index )
	, m_Connected		( false )
{
	//中身を生成.
	m_Pad = std::make_unique<CXInput>(0);
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