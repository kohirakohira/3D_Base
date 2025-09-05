#pragma once
#include "CSkinMeshObject.h"

/**************************************************
*	�U�R�N���X
**/
class CZako
	: public CSkinMeshObject
{
public:
	CZako();
	virtual ~CZako() override;

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	void SetAnimSpeed(double speed) { m_AnimSpeed = speed; }

protected:
	int				m_AnimNo;			//�A�j���[�V�����ԍ�
	double			m_AnimTime;			//�A�j���[�V�����o�ߎ���
	double			m_AnimSpeed;		//�A�j���[�V�������x
	D3DXVECTOR3		m_BonePos;			//�{�[�����W

};