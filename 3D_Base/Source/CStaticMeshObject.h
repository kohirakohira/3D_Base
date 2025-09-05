#pragma once

#include "GameObject//CGameObject.h"
#include "CStaticMesh.h"
#include "CBoundingSphere.h"
#include "CRay.h"

/************************************************************
*	�X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X.
**/
class CStaticMeshObject
	: public CGameObject
{
public:
	CStaticMeshObject();
	virtual ~CStaticMeshObject() override;

	//CGameObject�ŏ������z�֐��̐錾������Ă�̂ł�����Œ�`������.
	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera ) override;

	//���b�V����ڑ�����.
	void AttachMesh( CStaticMesh& pMesh ){
		m_pMesh = &pMesh;
	}
	//���b�V����؂藣��.
	void DetachMesh(){
		m_pMesh = nullptr;
	}
	//�o�E���f�B���O�X�t�B�A�擾
	CBoundingSphere* GetBSphere() const {
		return m_pBSphere;
	}
	//���f���ɍ��킹���o�E���f�B���O�X�t�B�A�쐬�̃��b�p�[�֐�
	HRESULT CreateBSphereForMesh(const CStaticMesh& pMesh) {
		return m_pBSphere->CreateSphereForMesh(pMesh);
	}
	//�o�E���f�B���O�X�t�B�A���I�u�W�F�N�g�ʒu�ɍ��킹��
	//�����f���̌��_�����S�̏ꍇ��z��
	void UpdateBSpherePos() {
		m_pBSphere->SetPosition(m_vPosition);
	}

	//���C�ƃ��b�V���̓����蔻��
	bool IsHitForRay(
		const RAY& pRay,					//���C�\����
		float* pDistance,					//(out)����
		D3DXVECTOR3* pIntersect,			//(out)�����_
		D3DXVECTOR3* pNormal = nullptr);	//(out)�@��(�x�N�g��)

	//�ǂ���̈ʒu���v�Z����
	void CalculatePositionFromWall(CROSSRAY* pCrossRay);

private:
	//�����ʒu�̃|���S���̒��_��������
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices);	//(out)���_���

	//��]�l�����i�P���ȏサ�Ă��鎞�̒����j
	void ClampDirection(float* dir);	//�ċA�֐�

protected:
	CStaticMesh*		m_pMesh;
	CBoundingSphere*	m_pBSphere;
};
