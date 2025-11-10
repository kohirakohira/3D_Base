#include "CXInput.h"
#include <crtdbg.h>

//値を範囲内に収める関数.
template<typename T>
T Clamp( T val, T min, T max )
{
	return ( val < min ) ? min : ( max < val ) ? max : val;
}

//KEY列挙体に対応したXINPUT_GAMEPADのテーブル.
const WORD KEY_TABLE[CXInput::MAX] =
{
	XINPUT_GAMEPAD_DPAD_UP,			//方向パッド:上.
	XINPUT_GAMEPAD_DPAD_DOWN,		//方向パッド:下.
	XINPUT_GAMEPAD_DPAD_LEFT,		//方向パッド:左.
	XINPUT_GAMEPAD_DPAD_RIGHT,		//方向パッド:右.
	XINPUT_GAMEPAD_START,			//ボタン:スタート.
	XINPUT_GAMEPAD_BACK,			//ボタン:バック.
	XINPUT_GAMEPAD_LEFT_THUMB,		//ボタン:左スティック.
	XINPUT_GAMEPAD_RIGHT_THUMB,		//ボタン:右スティック.
	XINPUT_GAMEPAD_LEFT_SHOULDER,	//ボタン:LB.
	XINPUT_GAMEPAD_RIGHT_SHOULDER,	//ボタン:RB.
	XINPUT_GAMEPAD_A,				//ボタン:A.
	XINPUT_GAMEPAD_B,				//ボタン:B.
	XINPUT_GAMEPAD_X,				//ボタン:X.
	XINPUT_GAMEPAD_Y,				//ボタン:Y.
};

//-------------------------------------------------.
//	コンストラクタ.
//-------------------------------------------------.
CXInput::CXInput( DWORD padId )
	: m_padId			( padId )
	, m_state			()
	, m_stateOld		()
	, m_vibration		()
	, m_connect			( false )
	, m_TestButton		( 0 )
	, m_TestConnected	( false )
	, m_TestLT			( 0 )
	, m_TestLX			( 0 )
	, m_TestLY			( 0 )
	, m_TestMode		( false )
	, m_TestRT			( 0 )
	, m_TestRX			( 0 )
	, m_TestRY			( 0 )
{
}

//-------------------------------------------------.
//	デストラクタ.
//-------------------------------------------------.
CXInput::~CXInput()
{
	EndProc();
}

//-------------------------------------------------.
//	キー入力の更新.
//-------------------------------------------------.
bool CXInput::Update()
{
	//キー入力情報
	m_stateOld = m_state;
	if (m_TestMode)
	{
		ZeroMemory(&m_state, sizeof(m_state));

		m_connect = m_TestConnected;

		if (m_connect)
		{
			m_state.Gamepad.wButtons		= m_TestButton;
			m_state.Gamepad.sThumbLY		= m_TestLY;
			m_state.Gamepad.sThumbLX		= m_TestLX;
			m_state.Gamepad.sThumbRX		= m_TestRX;
			m_state.Gamepad.sThumbRY		= m_TestRX;
			m_state.Gamepad.bLeftTrigger	= m_TestLT;
			m_state.Gamepad.bRightTrigger	= m_TestRT;
		}
		return m_connect;
	}

	//キー情報を更新する前に退避.
	m_stateOld = m_state;
	if( UpdateStatus() == false ){
		return false;
	}
	return true;
}

//-------------------------------------------------.
//	終了処理.
//-------------------------------------------------.
void CXInput::EndProc()
{
	SetVibration( 0, 0 );
}

//-------------------------------------------------.
//	押した.Just=trueにすることで押した瞬間の取得が可能.
//-------------------------------------------------.
bool CXInput::IsDown( KEY key, bool Just )
{
	WORD GamePad = GenerateGamePadValue( key );

	bool ret = false;

	if( IsKeyCore( GamePad, m_state ) == true )
	{
		ret = true;

		if (Just == true) {
			//今回入力で前回未入力→押した瞬間.
			if (IsKeyCore(GamePad, m_stateOld) == false) {
				ret = true;
			}
			else {
				ret = false;
			}
		}
	}
	return ret;
}

//-------------------------------------------------.
//	離した.
//-------------------------------------------------.
bool CXInput::IsUp( KEY key )
{
	WORD GamePad = GenerateGamePadValue( key );

	//前回入力で今回未入力→離した瞬間.
	if( IsKeyCore( GamePad, m_stateOld ) == true &&
		IsKeyCore( GamePad, m_state ) == false )
	{
		return true;
	}
	return false;
}

//-------------------------------------------------.
//	押し続けている.
//-------------------------------------------------.
bool CXInput::IsRepeat( KEY key )
{
	WORD GamePad = GenerateGamePadValue( key );

	//前回入力で今回入力→押し続けている.
	if( IsKeyCore( GamePad, m_stateOld ) == true &&
		IsKeyCore( GamePad, m_state ) == true )
	{
		return true;
	}
	return false;
}

float CXInput::GetLeftStickXNormalized() const
{
	//値を取得.
	float value = static_cast<float>(m_state.Gamepad.sThumbLX);

	//デッドゾーン処理.
	const float DEADZONE = static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	if (fabs(value) < DEADZONE)
	{
		return 0.0f;
	}
	//範囲補正(-32767.0f～32767.0f → -1.0～1.0).
	return value / 32767.0f;
}

float CXInput::GetLeftStickYNormalized() const
{
	//値を取得.
	float value = static_cast<float>(m_state.Gamepad.sThumbLY);

	//デッドゾーン処理.
	const float DEADZONE = static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	if (fabs(value) < DEADZONE)
	{
		return 0.0f;
	}
	//範囲補正(-32767.0f～32767.0f → -1.0～1.0).
	return value / 32767.0f;
}

float CXInput::GetRightStickXNormalized() const
{
	//値を取得.
	float value = static_cast<float>(m_state.Gamepad.sThumbLY);

	//デッドゾーン処理.
	const float DEADZONE = static_cast<float>(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	if (fabs(value) < DEADZONE)
	{
		return 0.0f;
	}
	//範囲補正(-32767.0f～32767.0f → -1.0～1.0).
	return value / 32767.0f;
}

float CXInput::GetRightStickYNormalized() const
{
	//値を取得.
	float value = static_cast<float>(m_state.Gamepad.sThumbLX);

	//デッドゾーン処理.
	const float DEADZONE = static_cast<float>(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	if (fabs(value) < DEADZONE)
	{
		return 0.0f;
	}
	//範囲補正(-32767.0f～32767.0f → -1.0～1.0).
	return value / 32767.0f;
}

float CXInput::GetLeftTriggerNormalized() const
{
	//値を取得.
	float value = static_cast<float>(m_state.Gamepad.bLeftTrigger);
	//微入力を無視.
	const float DEADZONE = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
	if (value < DEADZONE)
	{
		return 0.0f;
	}
	return value / 255.0f;
}

float CXInput::GetRightTriggerNormalized() const
{
	//値を取得.
	float value = static_cast<float>(m_state.Gamepad.bRightTrigger);
	//微入力を無視.
	const float DEADZONE = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
	if (value < DEADZONE)
	{
		return 0.0f;
	}
	return value / 255.0f;
}

//-------------------------------------------------.
//	振動設定.
//-------------------------------------------------.	
bool CXInput::SetVibration( WORD LMotorSpd, WORD RMotorSpd )
{
	m_vibration.wLeftMotorSpeed = Clamp( LMotorSpd, VIBRATION_MIN, VIBRATION_MAX );
	m_vibration.wRightMotorSpeed = Clamp( RMotorSpd, VIBRATION_MIN, VIBRATION_MAX );

	if( ERROR_SUCCESS == XInputSetState(
		m_padId, &m_vibration ) ){
		return true;
	}
	return false;
}




//-------------------------------------------------.
//	キー入力の更新.
//-------------------------------------------------.
bool CXInput::UpdateStatus()
{
#if 0
	m_connect = false;
	if( ERROR_SUCCESS == XInputGetState(
		m_padId,
		&m_state ) )
	{
		m_connect = true;
		return true;
	}
	return false;
#endif
	//未接続の時にクリア
	m_connect = (XInputGetState(m_padId, &m_state) == ERROR_SUCCESS);

	if (!m_connect)
	{
		ZeroMemory(&m_state, sizeof(m_state));
	}
	return m_connect;
}



//-------------------------------------------------.
//	IsKeyDown,Up,Repeat内で使用する関数.
//-------------------------------------------------.
bool CXInput::IsKeyCore( WORD GamePad, const XINPUT_STATE& State )
{
	if( ( State.Gamepad.wButtons & GamePad ) != 0 ){
		return true;
	}
	return false;
}

//-------------------------------------------------.
//	KEY列挙体を対応したXINPUT_GAMEPADの値に変換.
//-------------------------------------------------.
WORD CXInput::GenerateGamePadValue( KEY key )
{
	// 範囲外の場合はアサート出す.
	if( key < KEY::FIRST || KEY::LAST < key ){
		_ASSERT_EXPR( false, L"キーの範囲外を指定" );
	}
	return KEY_TABLE[key];
}