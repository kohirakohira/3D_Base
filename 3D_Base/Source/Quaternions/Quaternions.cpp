#include "Quaternions.h"

Quaternions::Quaternions()
	: m_QuaternionConversion			()
	, m_ArbitraryAxis					()
	, m_QuaternionAxis					()
	, m_QtoComposition					()
	, m_DirectionVector					()
	, m_VectorRotated					()
	, m_Out								()
	, m_VecNormalize					()
	, m_VecConjugate					()
	, m_RotMatrix						()
	, m_Qtof							()
	, m_Angle							( 0.f )
{
	m_Quat = { 0.f, 0.f, 0.f};
}

Quaternions::~Quaternions()
{
}

//�������֐�.
void Quaternions::Init()
{
	//�e�p�x��ݒ�(x,y,z).
	m_Quat = { 0.f, 0.f, 0.f };

	//�O�̉�]����̎l����(�N�H�[�^�j�I��)�ɕϊ�.
	//�p�x�́u�K�p�����v�͏d�v!!.
	//Q1.
	m_QuaternionConversion = DirectX::XMQuaternionRotationRollPitchYaw(
		DirectX::XMConvertToRadians(m_Quat.Pitch),
		DirectX::XMConvertToRadians(m_Quat.Yaw),
		DirectX::XMConvertToRadians(m_Quat.Roll));

	//Y���̃x�N�g���w��.
	m_ArbitraryAxis = DirectX::XMVectorSet( 0.f, 1.f, 0.f, 0.f );

	//�p�x�ݒ�(Y����90���񂷃N�H�[�^�j�I���̍쐬).
	m_Angle = 90.0f;

	//�N�H�[�^�j�I���̍쐬(��(�x�N�g��)�Ɗp�x(���W�A���l)���g�p).
	//Q2.
	m_QuaternionAxis = DirectX::XMQuaternionRotationAxis( m_ArbitraryAxis, DirectX::XMConvertToRadians(m_Angle));

	//��̃N�H�[�^�j�I��������(�|���Z).
	//�|���Z�̏��Ԃ���]�̏��Ԃ����߂�.
	//m_QtoComposition = Q2 * Q1.
	m_QtoComposition = DirectX::XMQuaternionMultiply(m_QuaternionAxis, m_QuaternionConversion);

	//X���̃x�N�g���w��.
	m_DirectionVector = DirectX::XMVectorSet(1.f, 0.f, 0.f, 0.f);

	//�x�N�g������]������.
	//�l������]�������I�ɍs��.
	m_VectorRotated = DirectX::XMVector3Rotate(m_DirectionVector, m_QtoComposition);

	//XMVECTOR->XMFLOAT3�ϊ�.
	DirectX::XMStoreFloat3(&m_Out, m_VectorRotated);

	//���K��.
	m_VecNormalize	= DirectX::XMQuaternionNormalize(m_QtoComposition);
	//����.
	m_VecConjugate	= DirectX::XMQuaternionConjugate(m_VecNormalize);
	//�l��������]�s��ɕϊ�.
	m_RotMatrix		= DirectX::XMMatrixRotationQuaternion(m_VecNormalize);
	//������.
	//���p�x���l����.
	//�x�N�g�����񂷁F�x�N�g�����l�����Ɋg��.
	//�l�������m�̐�(Hamilton��).
	//(x1,y1,z1,w1) * (x2,y2,z2,w2) =
	//( w1*x2 + x1*w2 + y1*z2 - z1*y2,
	//  w1*y2 - x1*z2 + y1*w2 + z1*x2,
	//  w1*z2 + x1*y2 - y1*x2 + z1*w2,
	//  w1*w2 - x1*x2 - y1*y2 - z1*z2 )
	//������Q�l->https://chatgpt.com/share/68c23e80-28c4-8001-9fb0-b597b8909197#:~:text=%E5%86%85%E9%83%A8%E7%9A%84%E3%81%AA,y2%20%2D%20z1*z2%20

	//XMVECTOR->XMFLOAT3�ϊ�.
	//(x,y,z,w) �̏��Ŏ��o���_�ɒ���.
	DirectX::XMStoreFloat4(&m_Qtof, m_VecNormalize);

}
