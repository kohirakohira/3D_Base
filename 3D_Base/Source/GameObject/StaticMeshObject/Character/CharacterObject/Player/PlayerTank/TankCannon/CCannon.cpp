#include "CCannon.h"
#include "Assets//Sound//CSoundManager.h" // サウンドマネージャークラス
#include "Collision/CollisionManager/CCollisionManager.h"

CCannon::CCannon(int inputID)
	: m_TurnSpeed				( 0.01f )	// ちっきりやりたい場合はラジアン値を設定すること(戦車で使うぞ!)
	, m_ShotCoolTime			( 120 )
	, m_ShotInterval			( 120 )
	, m_PlayerID				( inputID )
	, m_pController				()
	/*, m_CannonRay()
	, m_pRayDrawer(nullptr)
	, m_MuzzleOffset(2.0f)
	, m_bDrawRay(false)*/
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

	// レイを更新
	//UpdateCannonRay();

}

void CCannon::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	CStaticMeshObject::Draw(View, Proj, Light, Camera);

	//// デバッグ用レイ描画
	//if (m_bDrawRay)
	//{
	//	DrawRay(View, Proj);
	//}
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

#if 0
// レイの初期化
HRESULT CCannon::InitCannonRay(float length)
{
    // レイ構造体の初期化
    m_CannonRay.Position = m_vPosition;
    m_CannonRay.Axis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);  // +Z方向（前方）
    m_CannonRay.Length = length;
    m_CannonRay.RotationY = m_vRotation.y;

    // 描画用クラスの初期化
    m_pRayDrawer = std::make_unique<CRay>();
    if (FAILED(m_pRayDrawer->Init(m_CannonRay)))
    {
        return E_FAIL;
    }

    return S_OK;
}

//========================================
// レイの更新（毎フレーム）
//========================================
void CCannon::UpdateCannonRay()
{
    // 砲口位置を計算
    m_CannonRay.Position = GetMuzzlePosition();

    // 砲塔のY軸回転を反映
    m_CannonRay.RotationY = m_vRotation.y;

    // 軸ベクトル（前方方向、回転前の基準）
    m_CannonRay.Axis = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

//========================================
// 砲塔の前方ベクトルを取得
//========================================
D3DXVECTOR3 CCannon::GetForward() const
{
    // Y軸回転行列を作成
    D3DXMATRIX mRotY;
    D3DXMatrixRotationY(&mRotY, m_vRotation.y);

    // 前方ベクトル（+Z）を回転
    D3DXVECTOR3 forward(0.0f, 0.0f, 1.0f);
    D3DXVECTOR3 result;
    D3DXVec3TransformCoord(&result, &forward, &mRotY);

    return result;
}

//========================================
// 砲口のワールド座標を取得
//========================================
D3DXVECTOR3 CCannon::GetMuzzlePosition() const
{
    D3DXVECTOR3 forward = GetForward();
    return m_vPosition + forward * m_MuzzleOffset;
}

//========================================
// 単一オブジェクトへのレイキャスト
//========================================
bool CCannon::RaycastTo(CStaticMeshObject* pTarget, CannonRayResult& outResult) const
{
    if (!pTarget) return false;

    outResult.bHit = false;
    outResult.pHitObject = nullptr;

    float distance = 0.0f;
    D3DXVECTOR3 intersect;

    // 対象にレイを当てる（既存のIsHitForRayを使用）
    if (pTarget->IsHitForRay(m_CannonRay, &distance, &intersect))
    {
        // レイの長さ以内でヒットしたか（distance < 1.0fは正規化された距離）
        if (distance < 1.0f)
        {
            outResult.bHit = true;
            outResult.distance = distance * m_CannonRay.Length;  // 実際の距離に変換
            outResult.hitPoint = intersect;
            outResult.pHitObject = pTarget;
            return true;
        }
    }

    return false;
}

//========================================
// 複数オブジェクトへのレイキャスト（最も近いものを返す）
//========================================
bool CCannon::RaycastToNearest(
    const std::vector<CStaticMeshObject*>& targets,
    CannonRayResult& outResult) const
{
    outResult.bHit = false;
    outResult.distance = m_CannonRay.Length + 1.0f;
    outResult.pHitObject = nullptr;

    CannonRayResult tempResult;

    for (auto* pTarget : targets)
    {
        if (!pTarget) continue;

        if (RaycastTo(pTarget, tempResult))
        {
            // より近いヒットを優先
            if (tempResult.distance < outResult.distance)
            {
                outResult = tempResult;
            }
        }
    }

    return outResult.bHit;
}

//========================================
// 指定位置が射線上にあるか（簡易判定）
//========================================
bool CCannon::IsPositionInSight(const D3DXVECTOR3& targetPos, float toleranceAngle) const
{
    D3DXVECTOR3 muzzle = GetMuzzlePosition();
    D3DXVECTOR3 toTarget = targetPos - muzzle;
    toTarget.y = 0.0f;  // 水平面で判定

    float dist = D3DXVec3Length(&toTarget);
    if (dist < 0.001f) return true;  // ほぼ同じ位置

    // 正規化
    toTarget /= dist;

    // 砲塔の前方ベクトル
    D3DXVECTOR3 forward = GetForward();
    forward.y = 0.0f;
    D3DXVec3Normalize(&forward, &forward);

    // 内積で角度を計算
    float dot = D3DXVec3Dot(&forward, &toTarget);

    // 許容角度内か
    return dot >= std::cosf(toleranceAngle);
}

//========================================
// レイのデバッグ描画
//========================================
void CCannon::DrawRay(D3DXMATRIX& View, D3DXMATRIX& Proj)
{
    if (!m_pRayDrawer) return;

    // 現在のレイ情報で描画
    m_pRayDrawer->Render(View, Proj, m_CannonRay);
}
#endif
