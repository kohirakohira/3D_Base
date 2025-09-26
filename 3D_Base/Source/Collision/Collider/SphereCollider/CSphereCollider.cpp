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
	//D3Dx�Ŏ�����.
	//�Q�̋��̂̒��S�Ԃ̋��������߂�
	D3DXVECTOR3 vLength = m_CenterPos - sphere.GetPosition();
	//��L�̃x�N�g�����璷���ɕϊ�
	float Length = D3DXVec3Length(&vLength);

	//�u�Q�̋��̂̋����v���u�Q�̋��̂̂��ꂼ��̔��a�𑫂������́v���A
	//�������Ƃ������Ƃ́A���̓��m���d�Ȃ��Ă���i�Փ˂��Ă���j�Ƃ�������
	if (Length <= m_Radius + sphere.GetRadius())
	{
		return true;	//�Փ˂��Ă���
	}
	return false;	//�Փ˂��Ă��Ȃ�
}

bool CSphereCollider::CheckCollisionBox(const CBoxCollider& box) const
{
	//�~�ƈ�ԋ߂��{�b�N�X�̈ʒu������.
	D3DXVECTOR3 ClosestPoint;

	//���ꂼ��̈ʒu�ŉ~�Ɉ�ԋ߂��n�_������.
	ClosestPoint.x = std::max(box.GetMinPosition().x, std::min(box.GetMaxPosition().x, m_CenterPos.x));
	ClosestPoint.y = std::max(box.GetMinPosition().y, std::min(box.GetMaxPosition().y, m_CenterPos.y));
	ClosestPoint.z = std::max(box.GetMinPosition().z, std::min(box.GetMaxPosition().z, m_CenterPos.z));

	D3DXVECTOR3 vLength = m_CenterPos - ClosestPoint;

	float Length = D3DXVec3Length(&vLength);

	//�~�Ɖ~�ƈႢ�A����Ă��邱�Ƃ��~�Ɠ_�Ȃ̂Ŕ��a�͈����.
	return Length < m_Radius;
}
