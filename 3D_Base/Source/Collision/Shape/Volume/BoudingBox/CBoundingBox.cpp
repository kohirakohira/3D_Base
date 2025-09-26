#include "CBoundingBox.h"

CBoundingBox::CBoundingBox()
	: m_MinPos()
	, m_MaxPos()
{
}

CBoundingBox::~CBoundingBox()
{
}

HRESULT CBoundingBox::CreateBoxForMesh(const CStaticMesh& pMesh)
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;	//���_�o�b�t�@.
	void* pVertices = nullptr;	//���_.

	D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);	//Vector�̍ŏ��l�AFLT_MAX��float�̍ő�l.
	D3DXVECTOR3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);	//Vector�̍ő�l�A

	//���_�o�b�t�@���擾.
	if (FAILED(pMesh.GetMesh()->GetVertexBuffer(&pVB)))
	{
		//Mesh��Model�̒��_�o�b�t�@���擾�ł��Ȃ��ƃG���[�ʂ�(����).
		return E_FAIL;
	}

	//���b�V���̒��_�o�b�t�@�����b�N����.
	if (FAILED(pVB->Lock(0, 0, &pVertices, 0)))	//���_�o�b�t�@��GPU��p��pVertices���ꎞ�I��CPU�ł��G���悤�ɂ��Ă���.
	{
		//���_�o�b�t�@�����b�N�ł��Ȃ��Ƃ��ʂ�.
		SAFE_DELETE(pVB);
		return E_FAIL;
	}

	//���_�̐����擾.
	DWORD VertexCount = pMesh.GetMesh()->GetNumVertices();
	//�傫�����擾.
	DWORD VertexSize = pMesh.GetMesh()->GetNumBytesPerVertex();

	//�e���_�̍ŏ�,�ő�����߂Ă�(��`�͓�_�ō쐬�ł��邩�炻�ꂼ��̍ŏ�,�ő�ł���).
	for (DWORD i = 0;i < VertexCount;i++)
	{
		D3DXVECTOR3* pPos = reinterpret_cast<D3DXVECTOR3*>(
			(BYTE*)pVertices + i * VertexSize);

		vMin.x = std::min(vMin.x, pPos->x);
		vMin.y = std::min(vMin.y, pPos->y);
		vMin.z = std::min(vMin.z, pPos->z);

		vMax.x = std::max(vMax.x, pPos->x);
		vMax.y = std::max(vMax.y, pPos->y);
		vMax.z = std::max(vMax.z, pPos->z);
	}

	//�A�����b�N�ŐG��Ȃ��悤�ɂ���.
	pVB->Unlock();

	//�����g�p���Ȃ��̂�
	SAFE_RELEASE(pVB);

	m_MinPos = vMin;
	m_MaxPos = vMax;

	return S_OK;
}