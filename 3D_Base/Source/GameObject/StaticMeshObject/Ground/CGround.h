#pragma once
#include "GameObject//StaticMeshObject//CStaticMeshObject.h" /* �p���N���X || �X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X */

//-----�O���N���X-----
#include "GameObject//StaticMeshObject//Character//CCharacter.h" // �L�����N�^�[�N���X

/*********************************************************
*	�n�ʃN���X
**/
class CGround
	: public CStaticMeshObject // �X�^�e�B�b�N���b�V���I�u�W�F�N�g�N���X���p��
{
public:
	CGround();
	virtual ~CGround();

	virtual void Update() override;

	//�v���C���[��ݒ�.
	void SetPlayer( CCharacter& pPlayer ) { m_pPlayer = &pPlayer; }

protected:
	CCharacter* m_pPlayer;

};