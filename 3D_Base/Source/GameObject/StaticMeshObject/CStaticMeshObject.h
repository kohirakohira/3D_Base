#pragma once
//-----�p������N���X-----
#include "GameObject//CGameObject.h" // �Q�[���I�u�W�F�N�g�N���X
//-----���C�u����-----
#include <iostream>

//-----�O���N���X-----
#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"	// �o�E���f�B���O�X�t�B�A
#include "Collision/Shape/Volume/BoudingBox/CBoundingBox.h"				// �o�E���f�B���O�{�b�N�X
#include "Collision//Shape//Ray//CRay.h" // ���C�N���X
#include "Collision/Collider/CCollider.h"

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
	//�����œ����蔻��̈ʒu�����X�V���Ă���̂œ����蔻�肪�K�v�ȕ��͐�΂ɂ��̏ꏊ��Update��ʂ邱��.
	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera) override;

	//���b�V����ڑ�����.
	void AttachMesh(std::shared_ptr<CStaticMesh> pMesh) {
		m_pMesh = pMesh;
	}
	//���b�V����؂藣��.
	void DetachMesh() {
		m_pMesh = nullptr;
	}

	//SphereCollider���쐬����.
	void CreateSpehreCollider(float radius);
	//BoxCollider���쐬����.
	void CreateBoxCollider(D3DXVECTOR3 min, D3DXVECTOR3 max);

	//���f���ɍ��킹���o�E���f�B���O�X�t�B�A�쐬�̃��b�p�[�֐�
	HRESULT CreateBSphereForMesh(const CStaticMesh& pMesh) {
		return m_pBSphere->CreateSphereForMesh(pMesh);
	}
	//�o�E���f�B���O�{�b�N�X�쐬�̃��b�p�[�֐�.
	HRESULT CreateBBoxForMesh(const CStaticMesh& pMesh) {
		return m_pBBox->CreateBoxForMesh(pMesh);
	}

	std::shared_ptr<CStaticMesh> GetStaticMesh() { return m_pMesh; }

	//�����蔻��̌^���擾.
	std::shared_ptr<CCollider> GetCollider() const { return m_pCollider.get(); }

	float GetRadius() { return m_pBSphere->GetRadius(); }

	D3DXVECTOR3 GetMinPos() { return m_pBBox->GetMinPosition(); }
	D3DXVECTOR3 GetMaxPos() { return m_pBBox->GetMaxPosition(); }

	D3DXVECTOR3 DebugMin() { return m_vPosition + m_pBBox->GetMinPosition(); }
	D3DXVECTOR3 DebugMax() { return m_vPosition + m_pBBox->GetMaxPosition(); }

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
		D3DXVECTOR3* pVertices);

protected:
	std::shared_ptr<CStaticMesh> m_pMesh;

	std::shared_ptr<CBoundingSphere> m_pBSphere;
	std::shared_ptr<CBoundingBox> m_pBBox;

	std::shared_ptr<CCollider>	m_pCollider;
};
