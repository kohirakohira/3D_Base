#pragma once
//-----���C�u����-----
#include <vector>
#include <memory>

//-----�O���N���X-----
#include "Collision//Shape//Volume//BoudingBox//CBoundingBox.h"			// �o�E���f�B���O�{�b�N�X
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"	// �o�E���f�B���O�X�t�B�A

#include "Assets//DirectX//DirectX9//CDirectX9.h" // DirectX9�N���X
#include "Assets//DirectX//DirectX11//CDirectX11.h" // DirectX11�N���X

#include "Assets//Mesh//StaticMesh//CStaticMesh.h" // �X�^�e�B�b�N���b�V���N���X
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" // �X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X

#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankBody//CBody.h"		// ��ԁF�ԑ�
#include "GameObject//StaticMeshObject//Character//Player//PlayerTank//TankCannon//CCannon.h"	// ��ԁF�C��

//===================================
//	�R���W�����}�l�[�W���[�N���X
//===================================
class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();

	// ���C���[���b�V��
	void Draw();

	// �I�u�W�F�N�g�̐���
	void Create();
	//�f�[�^�̓ǂݍ���.
	HRESULT LoadData();

	// �o�E���f�B���O�̍��W�X�V
	void UpdateBounding();
	void CheckAllCollisions();

	void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
private:
	std::shared_ptr<CStaticMesh>			m_pStaticMeshBSphere;		//�o�E���f�B���O�X�t�B�A(�����蔻��p).

	std::shared_ptr<CBoundingBox>			m_pBBox;
	std::shared_ptr<CBoundingSphere>		m_pBSphere;

	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;
};