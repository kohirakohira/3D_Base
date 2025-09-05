#include "CZako.h"

CZako::CZako()
	: m_AnimNo		()
	, m_AnimTime	()
	, m_AnimSpeed	( 0.0002 )
	, m_BonePos		()
{
}

CZako::~CZako()
{
}

void CZako::Update()
{
	m_pMesh->SetAnimSpeed(m_AnimSpeed);

	//�A�j���[�V�����ؑ�
	if (GetAsyncKeyState('N') & 0x8000)
	{
		m_AnimNo = 2;		//�o��A�j���[�V����
		m_AnimTime = 0.0;	//�A�j���[�V�����o�ߎ��ԏ�����
		m_pMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);
	}
	if (m_AnimNo == 2)
	{
		//�A�j���[�V�����o�ߎ��Ԃ����Z
		m_AnimTime += m_AnimSpeed;

		//�o��A�j���[�V�����̏I�����Ԃ𒴂����H
		if (m_pMesh->GetAnimPeriod(m_AnimNo) < m_AnimTime)
		{
			m_AnimNo = 0;		//�ҋ@�A�j���[�V������
			m_AnimTime = 0.0;	//�A�j���[�V�����o�ߎ��ԏ�����
			m_pMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);
		}
	}
	if (m_AnimNo == 0)
	{
		//�A�j���[�V�����o�ߎ��Ԃ����Z
		m_AnimTime += m_AnimSpeed;

		//�o��A�j���[�V�����̏I�����Ԃ𒴂����H
		if (m_pMesh->GetAnimPeriod(m_AnimNo) < m_AnimTime)
		{
			m_AnimNo = 1;		//�����A�j���[�V������
			m_AnimTime = 0.0;	//�A�j���[�V�����o�ߎ��ԏ�����
			m_pMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);
		}
	}
	//�{�[�����W�̎擾
	m_pMesh->GetPosFromBone("entotu_Joint", &m_BonePos);
}

void CZako::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	m_pMesh->SetAnimSpeed(m_AnimSpeed);
	CSkinMeshObject::Draw(View, Proj, Light, Camera);
}
