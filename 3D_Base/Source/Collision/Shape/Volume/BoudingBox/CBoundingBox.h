#pragma once
//-----�O���N���X-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X

//================================================
//	�o�E���f�B���O�{�b�N�X�N���X (OBB)
//================================================
class CBoundingBox
{
public:
	CBoundingBox();
	~CBoundingBox();

	//���f���ɍ��킹���o�E���f�B���O�{�b�N�X���쐬.
	HRESULT CreateBoxForMesh(const CStaticMesh& pMesh);

	//���S���W���擾����
	const D3DXVECTOR3& GetMinPosition() const { return m_MinPos; }
	const D3DXVECTOR3& GetMaxPosition() const { return m_MaxPos; }

private:
	D3DXVECTOR3		m_MinPos;	//�ŏ��ʒu.
	D3DXVECTOR3		m_MaxPos;	//�ő�ʒu.
};

