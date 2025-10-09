#include "CBoundingBox.h"

CBoundingBox::CBoundingBox()
	: m_MinPos(0, 0, 0)
	, m_MaxPos(0, 0, 0)
	, m_Position(0, 0, 0)
	, m_HalfSize(0.5f, 0.5f, 0.5f)
{
	m_Axis[0] = D3DXVECTOR3(1, 0, 0);
	m_Axis[1] = D3DXVECTOR3(0, 1, 0);
	m_Axis[2] = D3DXVECTOR3(0, 0, 1);
}

CBoundingBox::~CBoundingBox()
{
}

//---------------------------------------
// モデルから OBB を作成
//---------------------------------------
HRESULT CBoundingBox::CreateBoxForMesh(const CStaticMesh& mesh)
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	void* pVertices = nullptr;

	// 初期化
	D3DXVECTOR3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	D3DXVECTOR3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// 頂点バッファ取得
	if (FAILED(mesh.GetMesh()->GetVertexBuffer(&pVB)))
		return E_FAIL;

	// 頂点バッファロック
	if (FAILED(pVB->Lock(0, 0, &pVertices, D3DLOCK_READONLY)))
	{
		SAFE_RELEASE(pVB);
		return E_FAIL;
	}

	DWORD vertexCount = mesh.GetMesh()->GetNumVertices();
	DWORD vertexSize = mesh.GetMesh()->GetNumBytesPerVertex();

	// ローカル空間でのmin/maxを取得
	for (DWORD i = 0; i < vertexCount; i++)
	{
		D3DXVECTOR3* pPos = reinterpret_cast<D3DXVECTOR3*>((BYTE*)pVertices + i * vertexSize);

		vMin.x = std::min(vMin.x, pPos->x);
		vMin.y = std::min(vMin.y, pPos->y);
		vMin.z = std::min(vMin.z, pPos->z);

		vMax.x = std::max(vMax.x, pPos->x);
		vMax.y = std::max(vMax.y, pPos->y);
		vMax.z = std::max(vMax.z, pPos->z);
	}

	pVB->Unlock();
	SAFE_RELEASE(pVB);

	// --- ローカル空間での中心とサイズ ---
	m_MinPos = vMin;
	m_MaxPos = vMax;
	m_Position = (vMin + vMax) * 0.5f;
	m_HalfSize = (vMax - vMin) * 0.5f;

	// --- ワールド行列の情報を反映 ---
	D3DXMATRIX world = mesh.GetWorldMatrix();

	// 軸ベクトルをワールド行列から取得（スケール・回転成分）
	m_Axis[0] = D3DXVECTOR3(world._11, world._12, world._13);
	m_Axis[1] = D3DXVECTOR3(world._21, world._22, world._23);
	m_Axis[2] = D3DXVECTOR3(world._31, world._32, world._33);

	// 正規化（スケールを含む場合があるため）
	for (int i = 0; i < 3; i++)
		D3DXVec3Normalize(&m_Axis[i], &m_Axis[i]);

	// ワールド行列を使って中心を変換
	D3DXVec3TransformCoord(&m_Position, &m_Position, &world);

	return S_OK;
}

bool CBoundingBox::IsHitBox(const CBoundingBox pBBox)
{
	// 分離軸定理によるOBB判定
	const float EPSILON = 1e-6f;

	const D3DXVECTOR3* A = m_Axis;      // 自分の軸
	const D3DXVECTOR3* B = pBBox.m_Axis; // 相手の軸

	float aExtent[3] = { m_HalfSize.x, m_HalfSize.y, m_HalfSize.z };
	float bExtent[3] = { pBBox.m_HalfSize.x, pBBox.m_HalfSize.y, pBBox.m_HalfSize.z };

	// 中心差ベクトル
	D3DXVECTOR3 t = pBBox.m_Position - m_Position;

	// 自分のローカル空間に変換
	t = D3DXVECTOR3(D3DXVec3Dot(&t, &A[0]), D3DXVec3Dot(&t, &A[1]), D3DXVec3Dot(&t, &A[2]));

	// 回転行列R, 並進成分t
	float R[3][3], AbsR[3][3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = D3DXVec3Dot(&A[i], &B[j]);
			AbsR[i][j] = fabs(R[i][j]) + EPSILON;
		}
	}

	float ra, rb;

	// 1. 自分の3軸
	for (int i = 0; i < 3; i++)
	{
		ra = aExtent[i];
		rb = bExtent[0] * AbsR[i][0] + bExtent[1] * AbsR[i][1] + bExtent[2] * AbsR[i][2];
		if (fabs((&t.x)[i]) > ra + rb) return false;
	}

	// 2. 相手の3軸
	for (int i = 0; i < 3; i++)
	{
		ra = aExtent[0] * AbsR[0][i] + aExtent[1] * AbsR[1][i] + aExtent[2] * AbsR[2][i];
		rb = bExtent[i];
		float proj = fabs(t.x * R[0][i] + t.y * R[1][i] + t.z * R[2][i]);
		if (proj > ra + rb) return false;
	}

	// 3. 両者の軸の外積9方向
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			ra = aExtent[(i + 1) % 3] * AbsR[(i + 2) % 3][j] + aExtent[(i + 2) % 3] * AbsR[(i + 1) % 3][j];
			rb = bExtent[(j + 1) % 3] * AbsR[i][(j + 2) % 3] + bExtent[(j + 2) % 3] * AbsR[i][(j + 1) % 3];

			float proj = fabs(t[(i + 2) % 3] * R[(i + 1) % 3][j] - t[(i + 1) % 3] * R[(i + 2) % 3][j]);
			if (proj > ra + rb) return false;
		}
	}

	// すべての軸で分離していなければ衝突
	return true;
}
