#include "CBody.h"
#include "Collision/CollisionManager/CCollisionManager.h"

#include "InputDevice/Input/Controller/ControllerManager/CControllerManager.h"


CBody::CBody(int inputID)
	: m_TurnSpeed			(0.01f)
	, m_MoveSpeed			(0.1f)
	, m_MoveState			(enMoveState::Stop)
	, m_Death				(false)
	, m_RespawnCoolTime		(120)
	, m_RespawnTime			(0)
{
	m_vPosition.y = -0.5f;

	//当たり判定の作成.
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

	RadioControl();	// 回転・移動処理
	CStaticMeshObject::Update();
}

void CBody::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
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
	case enMoveState::Forward: 		// 前進
		m_vPosition += vecAxisZ * m_Tuning.moveSpeed;
		break;
	case enMoveState::Backward: 	// 後退
		m_vPosition -= vecAxisZ * m_Tuning.moveSpeed;
		break;
	default:
		break;
	}
	//// 上記の移動処理が終われば停止状態にしておく
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
