#include "CCannon.h"
#include "CSoundManager.h"

CCannon::CCannon(int inputID)
	: m_TurnSpeed				( 0.01f )	// ちっきりやりたい場合はラジアン値を設定すること(戦車で使うぞ!)
	, m_ShotCoolTime			( 120 )
	, m_ShotInterval			( 120 )
	, m_pCamera					( nullptr )
	, m_pInput					( nullptr )
{
	// 入力受付インスタンスの生成とセット
	m_Input = std::make_shared<CInputManager>(inputID);
	if (m_Input) {
		// 親クラス(CCharacter)の m_Input にも共有
		CCharacter::m_Input = m_Input;
	}
}

CCannon::~CCannon()
{
}

void CCannon::Update()
{
	if (m_Input)
	{
		m_Input->Update();
	}

	KeyInput();

	CCharacter::Update();
}

void CCannon::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CCharacter::Draw(View, Proj, Light, Camera);
}

void CCannon::SetCannonPosition(const D3DXVECTOR3& Pos)
{
	m_vPosition = Pos;
}

void CCannon::SetInputManager(const std::shared_ptr<CInputManager>& input)
{
	m_pInput = input;
}

// キー入力受付
void CCannon::KeyInput()
{
	// 左方向に入力検知
	if (m_Input->GetArrowKeyDirection() == CInputManager::Direction::Left ||
		m_Input->GetRightStickDirection() == CInputManager::Direction::Left)
	{
		m_vRotation.y -= m_TurnSpeed;
	}

	// 右方向に入力検知
	if (m_Input->GetArrowKeyDirection() == CInputManager::Direction::Right ||
		m_Input->GetRightStickDirection() == CInputManager::Direction::Right)
	{
		m_vRotation.y += m_TurnSpeed;
	}

	// クールタイマー更新
	if (m_ShotCoolTime < m_ShotInterval)
	{
		m_ShotCoolTime++;
		m_Shot = false;
	}
	else
	{
		if (m_Input->IsKeyboardRepeat('Z') ||
			m_Input->GetRightTrigger() == CInputManager::Trigger::RightTrigger)
		{
			m_Shot = true;		// 弾を発射
			m_ShotCoolTime = 0; // クールダウン再スタート
		}
	}
}

