#include "CSphereCollider.h"

#include "Collision//Collider//BoxCollider//CBoxCollider.h"
#include <algorithm>

CSphereCollider::CSphereCollider()
	: m_Radius()
{
}

CSphereCollider::~CSphereCollider()
{
}

void CSphereCollider::UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale)
{
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
	//円と一番近いボックスの位置が入る.
	D3DXVECTOR3 ClosestPoint;

	D3DXVECTOR3 vLength = m_CenterPos - ClosestPoint;

	float Length = D3DXVec3Length(&vLength);

	//円と円と違い、やっていることが円と点なので半径は一つだけ.
	return Length < m_Radius;
}

void CSphereCollider::SetRotation(const D3DXVECTOR3& rotation)
{
}

void CSphereCollider::SetScale(const D3DXVECTOR3& scale)
{
}
