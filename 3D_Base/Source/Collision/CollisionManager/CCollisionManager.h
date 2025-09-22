#pragma once
//-----���C�u����-----
#include <vector>
#include <memory>

//-----�O���N���X-----
#include "Collision//Shape//Volume//BoudingBox//CBoundingBox.h"			// �o�E���f�B���O�{�b�N�X
#include "Collision//Shape//Volume//BoundingSphere//CBoundingSphere.h"	// �o�E���f�B���O�X�t�B�A

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

	void Draw();

	void CheckAllCollisions();

	// �I�u�W�F�N�g�̃o�E���f�B���O���쐬
	void CreateBounding();

	// �o�E���f�B���O�̍��W�X�V
	void UpdateBounding();


	void SetCBody(std::shared_ptr<CBody> pBody) { m_pBody = pBody; }
private:
	std::shared_ptr<CBoundingBox>			m_pBBox;
	std::shared_ptr<CBoundingSphere>		m_pBSphere;

	std::shared_ptr<CBody>					m_pBody;
	std::shared_ptr<CCannon>				m_pCannon;
};