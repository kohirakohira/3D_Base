#include "CBoundingBox.h"

CBoundingBox::CBoundingBox()
	: m_MinPos()
	, m_MaxPos()
	, m_Center(0, 0, 0)
	, m_HalfSize(0.5f, 0.5f, 0.5f)
{
	m_Axis[0] = D3DXVECTOR3(1, 0, 0);
	m_Axis[1] = D3DXVECTOR3(0, 1, 0);
	m_Axis[2] = D3DXVECTOR3(0, 0, 1);
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

HRESULT CBoundingBox::CreateOBBForMesh(const CStaticMesh& pMesh)
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	void* pVertices = nullptr;

	D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	if (FAILED(pMesh.GetMesh()->GetVertexBuffer(&pVB)))
		return E_FAIL;

	if (FAILED(pVB->Lock(0, 0, &pVertices, 0)))
	{
		SAFE_RELEASE(pVB);
		return E_FAIL;
	}

	DWORD VertexCount = pMesh.GetMesh()->GetNumVertices();
	DWORD VertexSize = pMesh.GetMesh()->GetNumBytesPerVertex();

	for (DWORD i = 0;i < VertexCount;i++)
	{
		D3DXVECTOR3* pPos = reinterpret_cast<D3DXVECTOR3*>((BYTE*)pVertices + i * VertexSize);

		vMin.x = std::min(vMin.x, pPos->x);
		vMin.y = std::min(vMin.y, pPos->y);
		vMin.z = std::min(vMin.z, pPos->z);

		vMax.x = std::max(vMax.x, pPos->x);
		vMax.y = std::max(vMax.y, pPos->y);
		vMax.z = std::max(vMax.z, pPos->z);
	}

	pVB->Unlock();
	SAFE_RELEASE(pVB);

	// 中心と半分サイズを設定
	m_Center = (vMin + vMax) * 0.5f;
	m_HalfSize = (vMax - vMin) * 0.5f;

	// モデルのワールド行列を取得
	D3DXMATRIX world = pMesh.GetWorldMatrix();

	// ワールド行列の回転・スケール成分から軸を作る
	m_Axis[0] = D3DXVECTOR3(world._11, world._12, world._13); // X軸
	m_Axis[1] = D3DXVECTOR3(world._21, world._22, world._23); // Y軸
	m_Axis[2] = D3DXVECTOR3(world._31, world._32, world._33); // Z軸

	// 正規化（スケール成分を含んでいる場合があるので）
	for (int i = 0; i < 3; i++)
		D3DXVec3Normalize(&m_Axis[i], &m_Axis[i]);

	return S_OK;
}
