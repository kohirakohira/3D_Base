#pragma once

#include "CStaticMeshObject.h"

/**************************************************
*	�e�N���X
**/
class CShot
	: public CStaticMeshObject
{
public:
	CShot();
	virtual ~CShot() override;

	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//�e���Đݒ�
	void Reload(const D3DXVECTOR3& Pos, float RotY);

	void SetDisplay(bool disp) { m_Display = disp; }

protected:
	bool		m_Display;			//�\���ؑ�
	D3DXVECTOR3	m_MoveDirection;	//�ړ�����
	float		m_MoveSpeed;		//�ړ����x
	int			m_DisplayTime;		//�\�����鎞��
};
