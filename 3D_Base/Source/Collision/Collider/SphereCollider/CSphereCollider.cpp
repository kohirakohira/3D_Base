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

bool CSphereCollider::CheckCollisionSphere(const CSphereCollider& sphere) const
{
	//D3Dxで実装版.
	//２つの球体の中心間の距離を求める
	D3DXVECTOR3 vLength = m_CenterPos - sphere.GetPosition();
	//上記のベクトルから長さに変換
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

	//それぞれの位置で円に一番近い地点を入れる.
	ClosestPoint.x = std::max(box.GetMinPosition().x, std::min(box.GetMaxPosition().x, m_CenterPos.x));
	ClosestPoint.y = std::max(box.GetMinPosition().y, std::min(box.GetMaxPosition().y, m_CenterPos.y));
	ClosestPoint.z = std::max(box.GetMinPosition().z, std::min(box.GetMaxPosition().z, m_CenterPos.z));

	D3DXVECTOR3 vLength = m_CenterPos - ClosestPoint;

	float Length = D3DXVec3Length(&vLength);

	//円と円と違い、やっていることが円と点なので半径は一つだけ.
	return Length < m_Radius;
}
