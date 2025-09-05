#include "CBody.h"

CBody::CBody(int inputID)
	: m_TurnSpeed			( 0.01f )	// ちっきりやりたい場合はラジアン値を設定すること(戦車で使うぞ!)
	, m_MoveSpeed			( 0.1f )
	, m_MoveState			( enMoveState::Stop )
	, m_pInput				( nullptr )
{
	m_vPosition.y = -0.5f;

	// 入力受付インスタンスの生成とセット
	m_Input = std::make_shared<CInputManager>(inputID);
	if (m_Input) {
		// 親クラス(CCharacter)の m_Input にも共有
		CCharacter::m_Input = m_Input;
	}
}

CBody::~CBody()
{
}

void CBody::Update()
{
	if (m_Input)
	{
		m_Input->Update();
	}

	KeyInput();		// 入力処理
	RadioControl();	// 回転・移動処理
	CCharacter::Update();
}

void CBody::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CCharacter::Draw(View, Proj, Light, Camera);
}

// ラジオ操作
void CBody::RadioControl()
{
	// Z軸ベクトル(Z+方向への単位ベクトル)
	// ※大きさ(長さ)が1のベクトルを単位ベクトルという
	D3DXVECTOR3 vecAxisZ(0.f, 0.f, 1.f);

	// Y方向の回転行列
	D3DXMATRIX mRotationY;
	// Y軸回転行列を作成
	D3DXMatrixRotationY(
		&mRotationY,		// (out)行列
		m_vRotation.y);		// プレイヤーのY方向の回転値

	// Y軸回転行列を使ってZ軸ベクトルを座標変化する
	D3DXVec3TransformCoord(
		&vecAxisZ,		// (out)Z軸ベクトル
		&vecAxisZ,		// (in)Z軸ベクトル
		&mRotationY);	//  Y軸回転行列

	// 移動状態によって処理を分ける
	switch (m_MoveState)
	{
	case enMoveState::Forward: 	// 前進
		m_vPosition += vecAxisZ * m_MoveSpeed;
		break;
	case enMoveState::Backward: 	// 後退
		m_vPosition -= vecAxisZ * m_MoveSpeed;
		break;
	default:
		break;
	}
	// 上記の移動処理が終われば停止状態にしておく
	m_MoveState = enMoveState::Stop;
}

void CBody::SetInputManager(const std::shared_ptr<CInputManager>& input)
{
	m_pInput = input;
}

void CBody::KeyInput()
{
	// 入力が無ければ処理しない
	if (!m_Input) return;

	//------------------------------------------------
	// 入力チェック
	//------------------------------------------------

	// 上方向の入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::Up ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::Up)
	{
		m_MoveState = enMoveState::Forward;
	}

	// 左上方向の入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::UpLeft ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::UpLeft)
	{
		m_vRotation.y -= m_TurnSpeed;
		m_MoveState = enMoveState::Forward;
	}

	// 右上方向の入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::UpRight ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::UpRight)
	{
		m_vRotation.y += m_TurnSpeed;
		m_MoveState = enMoveState::Forward;
	}

	// 下方向の入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::Down ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::Down)
	{
		m_MoveState = enMoveState::Backward;
	}

	// 左下方向の入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::DownLeft ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::DownLeft)
	{
		m_vRotation.y -= m_TurnSpeed;
		m_MoveState = enMoveState::Backward;
	}

	// 右下方向の入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::DownRight ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::DownRight)
	{
		m_vRotation.y += m_TurnSpeed;
		m_MoveState = enMoveState::Backward;
	}

	// 左方向に入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::Left ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::Left)
	{
		m_vRotation.y -= m_TurnSpeed;
	}

	// 右方向に入力検知
	if (m_Input->GetWASDKeyDirection() == CInputManager::Direction::Right ||
		m_Input->GetLeftStickDirection() == CInputManager::Direction::Right)
	{
		m_vRotation.y += m_TurnSpeed;
	}
	//------------------------------------------------
	//------------------------------------------------
}

