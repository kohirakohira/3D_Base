#include "CBody.h"
#include "Collision/CollisionManager/CCollisionManager.h"

CBody::CBody(int inputID)
	: m_TurnSpeed			(0.01f)	// ちっきりやりたい場合はラジアン値を設定すること(戦車で使うぞ!)
	, m_MoveSpeed			(0.1f)
	, m_MoveState			(enMoveState::Stop)
	, m_Death				(false)
	, m_RespawnCoolTime		(120)
	, m_RespawnTime			(0)
	, m_pController			()
{
	m_vPosition.y = -0.5f;

	//コントローラーの設定
	m_pController = CControllerManager::GetInstance().GetController(inputID);

	m_pCollider = std::make_shared<CBoxCollider>();
}

CBody::~CBody()
{
}

void CBody::Init()
{
}

void CBody::Update()
{
	// Y座標を固定
	m_vPosition.y = 0;

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
	auto& tunign = GetTuning();
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
		m_vPosition += vecAxisZ * tunign.moveSpeed;
		break;
	case enMoveState::Backward: 	// 後退
		m_vPosition -= vecAxisZ * tunign.moveSpeed;
		break;
	default:
		break;
	}
	// 上記の移動処理が終われば停止状態にしておく
	m_MoveState = enMoveState::Stop;
}

void CBody::PushBack(const D3DXVECTOR3& push)
{
	m_vPosition += push;
}

void CBody::CreateBounding(std::shared_ptr<CStaticMesh> pBody)
{
	CreateBBoxForMesh(*pBody);
}

void CBody::AddRotationY(float value)
{
	m_vRotation.y += value;
}

void CBody::KeyInput()
{
	auto& tunign = GetTuning();

	float DeadZone = 0.2f; // スティックのデッドゾーン

	// 上方向の入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::Up)
	{
		m_MoveState = enMoveState::Forward;
	}

	// 左上方向の入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::UpLeft)
	{
		m_vRotation.y -= tunign.turretTurnSpeed;
		m_MoveState = enMoveState::Forward;
	}

	// 右上方向の入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::UpRight)
	{
		m_vRotation.y += tunign.turretTurnSpeed;
		m_MoveState = enMoveState::Forward;
	}

	// 下方向の入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::Down)
	{
		m_MoveState = enMoveState::Backward;
	}

	// 左下方向の入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::DownLeft)
	{
		m_vRotation.y -= tunign.turretTurnSpeed;
		m_MoveState = enMoveState::Backward;
	}

	// 右下方向の入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::DownRight)
	{
		m_vRotation.y += tunign.turretTurnSpeed;
		m_MoveState = enMoveState::Backward;
	}

	// 左方向に入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::Left)
	{
		m_vRotation.y -= tunign.turretTurnSpeed;
	}

	// 右方向に入力検知
	if (m_pController->GetLeftStickDirection(DeadZone) == CController::Direction::Right)
	{
		m_vRotation.y += tunign.turretTurnSpeed;
	}
}
