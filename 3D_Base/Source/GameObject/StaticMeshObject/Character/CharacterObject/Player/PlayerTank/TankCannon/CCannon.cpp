#include "CCannon.h"
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス
#include "Collision/CollisionManager/CCollisionManager.h"

CCannon::CCannon(int inputID)
	: m_TurnSpeed				( 0.01f )	// ちっきりやりたい場合はラジアン値を設定すること(戦車で使うぞ!)
	, m_ShotCoolTime			( 120 )
	, m_ShotInterval			( 120 )
	, m_PlayerID				( inputID )
	, m_pController				()
    , m_CannonRay               ()
    , m_MuzzleOffset            ( 2.0f )
    , m_DrawRay                 ( false )
{
	//コントローラーの設定
	m_pController = CControllerManager::GetInstance().GetController(inputID);

	// コライダー作成
	m_pCollider = std::make_shared<CBoxCollider>();

}

CCannon::~CCannon()
{
}

void CCannon::Update()
{
	m_ShotCoolTime++;

	CStaticMeshObject::Update();
    
    //レイを更新
    UpdateCannonRay();

}

void CCannon::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);

	// デバッグ用レイ描画
	if (m_DrawRay)
	{
		DrawRay(View, Proj);
	}
}

void CCannon::Init()
{
    //cannon初期化
    InitCannonRay();
}

HRESULT CCannon::InitCannonRay(float len)
{
    //各パラメータ初期化
    m_CannonRay.Position = m_vPosition;
    m_CannonRay.RotationY = m_vRotation.y;
    m_CannonRay.Axis = D3DXVECTOR3(0.f, 1.f, 1.f);
    m_CannonRay.Length = len;

    //レイの生成
    m_pRayDrawer = std::make_unique<CRay>();

    //レイのInit呼び出し.
    if (FAILED(m_pRayDrawer->Init(m_CannonRay)))
    {
        return E_FAIL;
    }
    return S_OK;
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
	if (m_ShotCoolTime >= m_ShotInterval)
	{
		//弾生成.
		if (m_pShot != nullptr)
		{
			//弾の生成.
			m_pShot->Create(pos, y, flag, index);

			//クールタイムのリセット.
			m_ShotCoolTime = 0;
		}
	}
}

//レイを更新
void CCannon::UpdateCannonRay()
{
    //砲塔のY回転を砲塔にも反映
    m_CannonRay.RotationY = m_vRotation.y;

    //砲口位置を計算
    m_CannonRay.Position = GetMuzzlePosition();

    //軸ベクトル
    m_CannonRay.Axis = D3DXVECTOR3(0.f, 0.f, 1.f);

}

//前方ベクトルの取得
D3DXVECTOR3 CCannon::GetForward() const
{
    //Y回転反映
    D3DXMATRIX mRotY;
    D3DXMatrixRotationY(&mRotY, m_vRotation.y);

    //ベクトルの変換
    D3DXVECTOR3 forward(0.f, 0.f, 1.f);
    D3DXVECTOR3 result;
    D3DXVec3TransformCoord(&result, &forward, &mRotY);

    return result;
}

//砲口のポジション取得
D3DXVECTOR3 CCannon::GetMuzzlePosition() const
{
    D3DXVECTOR3 forward = GetForward();
    return m_vPosition + forward * m_MuzzleOffset;
}


//単一オブジェクトへのレイキャスト
bool CCannon::RaycastTo(CStaticMeshObject* pTarget, CannonHitRay& outResult) const
{
    if (!pTarget) return false;

    outResult.bHit = false;
    outResult.pHitObject = nullptr;

    float distance = 0.0f;
    D3DXVECTOR3 intersect;

    // 対象にレイを当てる
    if (pTarget->IsHitForRay(m_CannonRay, &distance, &intersect))
    {
        //レイの長さ内でヒットしたか
        if (distance < 1.0f)
        {
            outResult.bHit = true;
            outResult.Distance = distance * m_CannonRay.Length;  // 実際の距離に変換
            outResult.HitPoint = intersect;
            outResult.pHitObject = pTarget;
            return true;
        }
    }

    return false;
}

//複数オブジェクトへのレイキャスト
bool CCannon::RaycastToNearest(const std::vector<CStaticMeshObject*>& targets, CannonHitRay& outResult) const
{
    //初期状態
    outResult.bHit = false; 
    outResult.Distance = m_CannonRay.Length + 1.f;  
    outResult.pHitObject = nullptr;

    CannonHitRay tempResult;

    for (auto* pTarget : targets)
    {
        if (!pTarget) continue;

        //単一オブジェクト
        if (RaycastTo(pTarget, tempResult))
        {
            // より近いヒットを優先
            if (tempResult.Distance < outResult.Distance)
            {
                outResult = tempResult;
            }
        }
    }

    return outResult.bHit;
}

//射線上かどうか
bool CCannon::IsPositionInSight(const D3DXVECTOR3& targetPos, float toleranceAngle) const
{
    D3DXVECTOR3 muzzle = GetMuzzlePosition();   //砲口の位置を取得 
    
    //ターゲットへのベクトル
    D3DXVECTOR3 toTarget = targetPos - muzzle;
    toTarget.y = 0.0f;  

    float dist = D3DXVec3Length(&muzzle);
    if (dist < 0.001f) return true;  // ほぼ同じ位置

    // 正規化
    muzzle /= dist;

    // 砲塔の前方ベクトル
    D3DXVECTOR3 forward = GetForward();
    forward.y = 0.0f;
    D3DXVec3Normalize(&forward, &forward);

    // 内積で角度を計算
    float dot = D3DXVec3Dot(&forward, &muzzle);

    // 許容角度内か
    return dot >= Util::Wrap(toleranceAngle);   //sinfでもいい
}

bool CCannon::HasObstacleInFireLine(const D3DXVECTOR3& targetPos, const std::vector<CStaticMeshObject*>& obstacles) const
{
    D3DXVECTOR3 muzzle = GetMuzzlePosition();
    D3DXVECTOR3 toTarget = targetPos - muzzle;
    float targetDist = D3DXVec3Length(&toTarget);

    if (targetDist < 0.001f) return false;

    //射線レイを設定
    RAY fireRay;
    fireRay.Position = muzzle;
    fireRay.RotationY = m_vRotation.y;
    fireRay.Axis = D3DXVECTOR3(0.f, 0.f, 1.f);
    fireRay.Length = targetDist;    //ターゲットまでの距離

    // 各障害物をチェック
    for (auto* pObstacle : obstacles)
    {
        if (!pObstacle) continue;

        float hitDist = 0.0f;
        D3DXVECTOR3 hitPoint;

        if (pObstacle->IsHitForRay(fireRay, &hitDist, &hitPoint))
        {
            // ターゲットより手前でヒットなら障害物あり
            float actualHitDist = hitDist * fireRay.Length;
            if (actualHitDist < targetDist * 0.95f)  // 少し余裕を持たせる
            {
                return true;
            }
        }
    }

    return false;
}

bool CCannon::CanFireAt(const D3DXVECTOR3& targetPos, const std::vector<CStaticMeshObject*>& obstacles, float toleranceAngleDeg) const
{
    // 射線上にいない
    if (!IsPositionInSight(targetPos, toleranceAngleDeg))
    {
        return false;
    }

    // 障害物がある
    if (HasObstacleInFireLine(targetPos, obstacles))
    {
        return false;
    }

    return true;
}

//レイ描画
void CCannon::DrawRay(D3DXMATRIX& View, D3DXMATRIX& Proj)
{
    if (!m_pRayDrawer) return;

    // 現在のレイ情報で描画
    m_pRayDrawer->Render(View, Proj, m_CannonRay);
}
