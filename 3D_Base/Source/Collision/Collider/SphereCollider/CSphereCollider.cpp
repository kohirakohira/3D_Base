#include "CSphereCollider.h"

#include "Collision//Collider//BoxCollider//CBoxCollider.h"
#include <algorithm>

CSphereCollider::CSphereCollider()
	: m_Radius		(0.0f)
{
	// 初期化
	m_Sphere.CenterPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Sphere.Radius = 0.0f;
}

CSphereCollider::~CSphereCollider()
{
}

void CSphereCollider::UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale)
{
	// CCollider のメンバーを更新
	m_CenterPos = pos;
	m_Rotation  = rot;
	m_Scale		= scale;

	//// 半径を計算 (最も大きなスケールを適用)
	//float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
	//m_Radius = m_BaseRadius * maxScale;

	// 構造体メンバーの更新
	m_Sphere.CenterPos = m_CenterPos;
	m_Sphere.Radius	   = m_Radius;
}

bool CSphereCollider::CheckCollisionSphere(const CSphereCollider& sphere) const
{
	//D3Dxで実装版.
	//２つの球体の中心間の距離を求める※ベクトル.
	D3DXVECTOR3 vLength = m_CenterPos - sphere.GetPosition();
	//上記のベクトルから長さに変換※スカラー値.
	float Length = D3DXVec3Length(&vLength);

	//「２つの球体の距離」が「２つの球体のそれぞれの半径を足したもの」より、
	//小さいということは、球体同士が重なっている（衝突している）ということ
	if (Length <= m_Radius + sphere.GetRadius())
	{
		return true;	//衝突している
	}
	return false;	//衝突していない
}

bool CSphereCollider::CheckCollisionBox(const CBoxCollider& box) const
{
	return box.CheckCollisionSphere(*this);
}

void CSphereCollider::SetRadius(float radius)
{
	m_Radius = radius;
	m_Sphere.Radius = radius;
}
