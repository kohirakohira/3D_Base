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
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;	//頂点バッファ.
	void* pVertices = nullptr;	//頂点.

	D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);	//Vectorの最小値、FLT_MAXはfloatの最大値.
	D3DXVECTOR3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);	//Vectorの最大値、

	//頂点バッファを取得.
	if (FAILED(pMesh.GetMesh()->GetVertexBuffer(&pVB)))
	{
		//MeshのModelの頂点バッファを取得できないとエラー通る(多分).
		return E_FAIL;
	}

	//メッシュの頂点バッファをロックする.
	if (FAILED(pVB->Lock(0, 0, &pVertices, 0)))	//頂点バッファはGPU専用でpVerticesが一時的にCPUでも触れるようにしている.
	{
		//頂点バッファがロックできないとき通る.
		SAFE_DELETE(pVB);
		return E_FAIL;
	}

	//頂点の数を取得.
	DWORD VertexCount = pMesh.GetMesh()->GetNumVertices();
	//大きさを取得.
	DWORD VertexSize = pMesh.GetMesh()->GetNumBytesPerVertex();

	//各頂点の最小,最大を求めてる(矩形は二点で作成できるからそれぞれの最小,最大でいい).
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

	//アンロックで触れないようにする.
	pVB->Unlock();

	//もう使用しないので
	SAFE_RELEASE(pVB);

	m_MinPos = vMin;
	m_MaxPos = vMax;

	return S_OK;
}