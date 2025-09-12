#pragma once

//-----���C�u����-----
#include <iostream>
#include <DirectXMath.h>		//D3DX���g�p���Ȃ��Ă������悤�ɂ��邽��.

//�N�H�[�^�j�I��.
class Quaternions
{
public:
	Quaternions();
	~Quaternions();

	//�������֐�.
	void Init();






public:
	//�\����.
	//�p�x���烉�W�A���ɕϊ����Ă���n��.
	struct Quaternion
	{
		float Pitch;	//X����](�s�b�`).
		float Yaw;		//Y����](���[).
		float Roll;		//Z����](���[��).
	};
	Quaternion	m_Quat;	//�N�H�[�^�j�I��.

	//�l������ۑ�����ϐ�.
	DirectX::XMVECTOR m_QuaternionConversion;
	//�C�ӎ�.
	DirectX::XMVECTOR m_ArbitraryAxis;
	//���p�x����̎l����.
	DirectX::XMVECTOR m_QuaternionAxis;
	//�����ۊǗp.
	DirectX::XMVECTOR m_QtoComposition;
	//�����x�N�g��.
	DirectX::XMVECTOR m_DirectionVector;
	//��]��̃x�N�g��.
	DirectX::XMVECTOR m_VectorRotated;
	//XMVECTOR->XMFLOAT3�ϊ��p.
	DirectX::XMFLOAT3 m_Out;
	//�m�[�}���C�Y�p�F���̊�ɍ��킹�Ē������鏈��.
	DirectX::XMVECTOR m_VecNormalize;
	//����p�F2�̑Ώۂ��݂��ɉe���������ē���̊֌W����������.
	DirectX::XMVECTOR m_VecConjugate;
	//�}�g���b�N�X.
	DirectX::XMMATRIX m_RotMatrix;
	//XMVECTOR->XMFLOAT4�ϊ��p.
	DirectX::XMFLOAT4 m_Qtof;

	//�ω�����p�x.
	float m_Angle;




private:

};
