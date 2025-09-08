#pragma once
#include "GameObject//CGameObject.h" /* �p���N���X || �Q�[���I�u�W�F�N�g�N���X */
#include <iostream>

//-----�O���N���X-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"
#include "Collision//Shape//Ray//CRay.h" // ���C�N���X

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
	void AttachMesh(std::shared_ptr<CStaticMesh> pMesh ){
		m_pMesh = pMesh;
	}
	//���b�V����؂藣��.
	void DetachMesh(){
		m_pMesh = nullptr;
	}
	//�o�E���f�B���O�X�t�B�A�擾
	std::shared_ptr<CBoundingSphere> GetBSphere() const {
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
		const RAY& pRay,			//���C�\����
		float* pDistance,			//(out)����
		D3DXVECTOR3* pIntersect);	//(out)�����_

private:
	//�����ʒu�̃|���S���̒��A��������.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMesh,
		DWORD dwPolyIndex,
		D3DXVECTOR3* pVertices );

protected:
	std::shared_ptr<CStaticMesh>		m_pMesh;
	std::shared_ptr<CBoundingSphere>	m_pBSphere;

};
