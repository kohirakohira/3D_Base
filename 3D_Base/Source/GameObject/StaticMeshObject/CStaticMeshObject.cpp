#include "CStaticMeshObject.h"

CStaticMeshObject::CStaticMeshObject()
	: m_pMesh			( nullptr )
	, m_pBSphere		( nullptr )
{
	m_pBSphere = std::make_shared<CBoundingSphere>();
}

CStaticMeshObject::~CStaticMeshObject()
{
	DetachMesh();
}

void CStaticMeshObject::Update()
{
	if( m_pMesh == nullptr ){
		return;
	}
}

void CStaticMeshObject::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
{
	if( m_pMesh == nullptr ){
		return;
	}

	//描画直前で座標や回転情報などを更新.
	m_pMesh->SetPosition( m_vPosition );
	m_pMesh->SetRotation( m_vRotation );
	m_pMesh->SetScale( m_vScale );

	//レンダリング.
	m_pMesh->Render( View, Proj, Light, Camera.vPosition );
}

//レイとメッシュの当たり判定
bool CStaticMeshObject::IsHitForRay(
	const RAY& pRay,
	float* pDistance,
	D3DXVECTOR3* pIntersect)
{
	D3DXVECTOR3 vAxis;		//軸ベクトル
	D3DXMATRIX	mRotationY;	//Y軸回転行列

	//Y軸回転行列を作成
	D3DXMatrixRotationY(&mRotationY, pRay.RotationY);
	//軸ベクトルを現在の回転状態に変換する
	D3DXVec3TransformCoord(&vAxis, &pRay.Axis, &mRotationY);

	//レイの始点と終点
	D3DXVECTOR3 StartPoint, EndPoint;
	StartPoint	= pRay.Position;	//レイの始点を設定
	EndPoint	= pRay.Position + (vAxis * pRay.Length);	//レイの終点を設定

	//レイを当てたいメッシュが移動している場合でも、
	//対象のWorld行列の逆行列を用いれば正しくレイが当たる
	D3DXMATRIX mWorld, mInverseWorld;
	D3DXMATRIX mTran;
	//移動処理
	D3DXMatrixTranslation(
		&mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	//回転処理
	//※この行列計算は「CStaticMesh::Render()関数」と同じにする必要あり
	D3DXMATRIX mRot, mYaw, mPitch, mRoll;
	//Y軸回転行列作成
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);
	//X軸回転行列作成
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);
	//Z軸回転行列作成
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);
	//回転行列を作成
	mRot = mYaw * mPitch * mRoll;

	//拡縮処理
	D3DXMATRIX mScale;
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	//ワールド行列計算
	//拡縮×回転×移動　※順番がとても大切！！
	mWorld = mScale * mRot * mTran;

	//逆行列を求める.
	D3DXMatrixInverse(&mInverseWorld, nullptr, &mWorld);
	//レイの始点、終点に反映.
	D3DXVec3TransformCoord(&StartPoint, &StartPoint, &mInverseWorld);
	D3DXVec3TransformCoord(&EndPoint, &EndPoint, &mInverseWorld);

	//向きと長さ(大きさ)を求める.
	D3DXVECTOR3 vDirection = EndPoint - StartPoint;

	BOOL bHit = FALSE;		//命中フラグ.
	DWORD dwIndex = 0;		//インデックス番号.
	D3DXVECTOR3 Vertex[3];	//頂点座標.
	FLOAT U = 0, V = 0;		//重心ヒット座標.

	//メッシュとレイの交差を調べる.
	D3DXIntersect(
		m_pMesh->GetMeshForRay(),	//対象メッシュ.
		&StartPoint,				//レイの始点.
		&vDirection,				//レイの向きと長さ(大きさ).
		&bHit,						//(out)判定結果.
		&dwIndex,	//(out)bHitがTRUEの時、レイの始点に最も近い面のインデックス値へのポインタ.
		&U, &V,						//(out)重心ヒット座標.
		pDistance,					//(out)メッシュとの距離.
		nullptr, nullptr );

	//無限に伸びるレイのどこかでメッシュが当たっていたら.
	if (bHit == TRUE)
	{
		//命中したとき.
		FindVerticesOnPoly(
			m_pMesh->GetMeshForRay(), dwIndex, Vertex );

		//重心座標から交点を算出.
		//ローカル交点は v0 + U*(v1 - v0) + V*(v2 - v0) で求まる.
		*pIntersect = Vertex[0] + U * (Vertex[1] - Vertex[0]) + V * (Vertex[2] - Vertex[0]);

		//モデルデータが「拡縮」「回転」「移動」していれば行列が必要.
		D3DXVec3TransformCoord(pIntersect, pIntersect, &mWorld);

		//EndPointから見た距離で1.fより小さければ当たっている.
		if (*pDistance < 1.f)
		{
			return true;	//命中している.
		}
	}
	return false;	//外れている.
}

//交差位置のポリゴンの超連を見つける.
HRESULT CStaticMeshObject::FindVerticesOnPoly(
	LPD3DXMESH pMesh, DWORD dwPolyIndex, D3DXVECTOR3* pVertices)
{
	//頂点ごとのバイト数を取得.
	DWORD dwStride = pMesh->GetNumBytesPerVertex();
	//頂点数を取得.
	DWORD dwVertexAmt = pMesh->GetNumVertices();
	//面数を取得.
	DWORD dwPolyAmt = pMesh->GetNumFaces();

	WORD* pwPoly = nullptr;

	//インデックスバッファをロック(読み込みモード).
	pMesh->LockIndexBuffer(
		D3DLOCK_READONLY,
		reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr;		//頂点(バイト型).
	FLOAT* pfVertices = nullptr;	//頂点(FLOAT型).
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	//頂点バッファ.

	//頂点情報の取得.
	pMesh->GetVertexBuffer(&VB);

	//頂点バッファのロック.
	if (SUCCEEDED(
		VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0)))
	{
		//ポリゴンの頂点1つ目を取得.
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];
		//ポリゴンの頂点2つ目を取得.
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];
		//ポリゴンの頂点3つ目を取得.
		pfVertices = reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer();	//ロック解除.
		VB->Unlock();	//ロック解除.
	}
	VB->Release();	//不要になったので解放.

	return S_OK;
}
