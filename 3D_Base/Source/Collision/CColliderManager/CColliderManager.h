#pragma once
//-----���C�u����-----
#include <vector>
#include <memory>
#include <iostream>

//-----�O���N���X-----
#include "Collision//Collider//CCollider.h" // �R���C�_�[�N���X

class CColliderManager
{
public:
	CColliderManager();
	~CColliderManager();

	//CCollider�ŐV���������蔻��̂�����̂��쐬�����Ƃ��Ɏg�p����.
	void AddCollider(std::unique_ptr<CCollider> collider);

	//�����蔻������ׂčs��.
	void CheckAllCollisions()const;

private:
	//���j�[�N�|�C���^CColider�^��Vector(�ϒ��z��)�ɂ��Ă�.
	std::vector< std::unique_ptr<CCollider> > m_pCollider;
};