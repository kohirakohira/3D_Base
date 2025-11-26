#include "CCannon.h"
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス
#include "Collision/CollisionManager/CCollisionManager.h"

CCannon::CCannon(int inputID)
	: m_TurnSpeed				( 0.01f )	// ちっきりやりたい場合はラジアン値を設定すること(戦車で使うぞ!)
	, m_ShotCoolTime			( 120 )
	, m_ShotInterval			( 120 )
	, m_PlayerID				( inputID )
	, m_pController				()
{
	//コントローラーの設定
	m_pController = CControllerManager::GetInstance().GetController(inputID);

	// コライダー作成
	m_pCollider = std::make_shared<CBoxCollider>();
	//m_pRay = std::make_shared<CRay>();
}

CCannon::~CCannon()
{
}

void CCannon::Update()
{
	if (m_ShotCoolTime < m_ShotInterval)
	{
		m_ShotCoolTime++;
	}

	//KeyInput();

	CStaticMeshObject::Update();

	//auto ray = CStaticMeshObject::GetRayY();	//Y軸レイ取得
}

void CCannon::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);
}

void CCannon::Init()
{
}

void CCannon::SetCannonPosition(const D3DXVECTOR3& Pos)
{
	m_vPosition = Pos;
}

void CCannon::PushBack(const D3DXVECTOR3& push)
{
	m_vPosition += push;
}

void CCannon::CreateBounding(std::shared_ptr<CStaticMesh> pCannon)
{
	CreateBBoxForMesh(*pCannon);
}

void CCannon::Reload(D3DXVECTOR3 pos, float y, bool flag, int index)
{
	//クールタイムがインターバルより小さいとき.
	if (m_ShotCoolTime < m_ShotInterval)
	{
		return;
	}

	//弾生成.
	if (m_pShot == nullptr)
	{
		m_pShot->Create(pos, y, flag, index);
	}

	//クールタイムのリセット.
	m_ShotCoolTime = 0;
}
