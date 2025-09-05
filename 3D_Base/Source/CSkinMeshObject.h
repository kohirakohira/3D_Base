#pragma once

#include "GameObject//CGameObject.h" // �Q�[���I�u�W�F�N�g�N���X
#include "CSkinMesh.h"
#include "BoundingSphere\\CBoundingSphere.h"

/************************************************************
*	�X�L�����b�V���I�u�W�F�N�g�N���X.
**/
class CSkinMeshObject
	: public CGameObject // �Q�[���I�u�W�F�N�g�N���X���p��
{
public:
	CSkinMeshObject();
	virtual ~CSkinMeshObject() override;

	//CGameObject�ŏ������z�֐��̐錾������Ă�̂ł�����Œ�`������.
	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera ) override;

	//���b�V����ڑ�����.
	void AttachMesh(CSkinMesh& pMesh);
	//���b�V����؂藣��.
	void DetachMesh();

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

protected:
	CSkinMesh*					m_pMesh;
	CBoundingSphere*			m_pBSphere;
	LPD3DXANIMATIONCONTROLLER	m_pAnimCtrl;	//�A�j���[�V�����R���g���[��
};
