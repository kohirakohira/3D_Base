#pragma once

#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"

#include "Assets//Mesh//StaticMesh//CStaticMesh.h"

//=======================================
//	�o�E���f�B���O�{�b�N�X�N���X
//=======================================

class CBoundingBox
{
public:
	CBoundingBox();
	~CBoundingBox();

	//���f���ɍ��킹���o�E���f�B���O�{�b�N�X���쐬.
	HRESULT CreateBoxForMesh(const CStaticMesh& pMesh);

	// ���f���ɍ��킹��OBB�쐬
	HRESULT CreateOBBForMesh(const CStaticMesh& pMesh);

	//���S���W���擾����
	const D3DXVECTOR3& GetMinPosition() const { return m_MinPos; }
	const D3DXVECTOR3& GetMaxPosition() const { return m_MaxPos; }

	// OBB�̒��S�Ɣ����T�C�Y�A���[�J�������擾
	const D3DXVECTOR3& GetCenter() const { return m_Center; }
	const D3DXVECTOR3& GetHalfSize() const { return m_HalfSize; }
	const D3DXVECTOR3* GetAxis() const { return m_Axis; }  // X,Y,Z��3��

private:
	D3DXVECTOR3		m_MinPos;	//�ŏ��ʒu.
	D3DXVECTOR3		m_MaxPos;	//�ő�ʒu.

	// OBB�p
	D3DXVECTOR3		m_Center;     // ���S
	D3DXVECTOR3		m_HalfSize;   // �e���̔����T�C�Y
	D3DXVECTOR3		m_Axis[3];    // ���[�J�����i�f�t�H���g�̓��[���h���j
};
