#include "CBoxCollider.h"
#include "Collision//Collider//SphereCollider//CSphereCollider.h"

CBoxCollider::CBoxCollider()
	: m_BaseHalfExtents ()
{
}

CBoxCollider::~CBoxCollider()
{
}

void CBoxCollider::UpdateTransform(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const D3DXVECTOR3& scale)
{
	// CCOlliderのメンバーを更新
	m_CenterPos = pos;
	m_Rotation = rot;
	m_Scale = scale;

	// OBBのサイズにHarfLengthとスケールを計算したものを設定
	m_OBB.HarfLength.x = m_BaseHalfExtents.x * scale.x;
	m_OBB.HarfLength.y = m_BaseHalfExtents.y * scale.y;
	m_OBB.HarfLength.z = m_BaseHalfExtents.z * scale.z;

	// OBBの中心点を設定
	m_OBB.CenterPos = pos;

	// OBBの方向ベクトルを計算
	D3DXMATRIX matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot, rot.y, rot.x, rot.z);

	// 回転行列の行ベクトルからローカル軸を抽出
	m_OBB.LocalAxes[0] = D3DXVECTOR3(matRot._11, matRot._12, matRot._13);
	m_OBB.LocalAxes[1] = D3DXVECTOR3(matRot._21, matRot._22, matRot._23);
	m_OBB.LocalAxes[2] = D3DXVECTOR3(matRot._31, matRot._32, matRot._33);
}

bool CBoxCollider::CheckCollisionSphere(const CSphereCollider& sphere) const
{
	// 仮計算用の最接近点
	D3DXVECTOR3 tempP = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// OBBとSphereの当たり判定
	return CheckCollisionOBBtoSphere(sphere, &m_OBB, &tempP);
}

bool CBoxCollider::CheckCollisionBox(const CBoxCollider& box) const
{
	return CheckCollisionOBBtoOBB(&m_OBB, &box.m_OBB);
}

bool CBoxCollider::CheckCollisionOBBtoOBB(const OBB* A, const OBB* B)
{
	// ゼロに近い微小な量(誤差)
	/* EPSILONは、軸が完全に平行(R[i][j] = 0)または直交(R[i][j] = ± 1)に
	   近い場合に発生する浮動小数点誤差を吸収するために使用*/
	const float EPSILON = 1.175494e-37;

	/* R(回転行列):OBB'A'のローカル軸を基準とした
				  OBB'B'のローカル軸の相対的な向き*/
	float R[3][3];

	// AbsR(絶対値行列):Rの要素の絶対値に微小な値(EPSILON)を加えたもの	
	float AbsR[3][3];

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			/* R[i][j] = OBB'A'の軸 LocalAxes[i]と
			   OBB'B'の軸LocalAxes[j]の内積 */
			R[i][j] = D3DXVec3Dot(&A->LocalAxes[i], &B->LocalAxes[j]);

			// AbsR[i][j] = R[i][j]の絶対値 + 微小な値(EPSILON)
			AbsR[i][j] = fabsf(R[i][j]) + EPSILON;
		}
	}

	D3DXVECTOR3 T = A->CenterPos - B->CenterPos; // 中心Aから中心Bへのベクトル
	// TをOBB'A'のローカル座標系に変換
	T = D3DXVECTOR3(
		D3DXVec3Dot(&T, &A->LocalAxes[0]),
		D3DXVec3Dot(&T, &A->LocalAxes[1]),
		D3DXVec3Dot(&T, &A->LocalAxes[2]));

	// 投影長:あるベクトルが別のベクトルに対して投影されたときのその長さ
	// RA:軸Ai上でのOBB'A'の投影長。HarfLength[i]がそのまま投影長になる
	/* RB:軸Bi上でのOBB'B'の投影長。OBB'B'の全軸のHarfLength[j]を
		　相対回転AbsR[i][j]を使って合成することで計算する*/
	float RA, RB;

	// 軸L=A₀, L=A₁, L=A₂判定(BoxAのローカル軸)
	for (int i = 0; i < 3; ++i)
	{
		// BoxAのこの軸上での半分の長さ(サイズ)
		RA = A->HarfLength[i];

		// BoxBのこの軸上での投影長
		RB = B->HarfLength[0] * AbsR[i][0] +
			 B->HarfLength[1] * AbsR[i][1] +
			 B->HarfLength[2] * AbsR[i][2];

		// 判定： |中心間の距離 > RA + RB(BoxBの中心T[i]は既にAのローカル座標)
		if (fabsf(T[i]) > RA + RB)
		{
			// 衝突なし
			return false;
		}
	}

	// 軸L=B₀, L=B₁, L=B₂判定(BoxBのローカル軸)
	for (int i = 0; i < 3; ++i)
	{
		// BoxAのこの軸上での投影長(上記RBと計算が逆)
		RA = A->HarfLength[0] * AbsR[0][i] +
			 A->HarfLength[1] * AbsR[1][i] +
			 A->HarfLength[2] * AbsR[2][i];

		// BoxBのこの軸上での半分の長さ(サイズ)
		RB = B->HarfLength[i];

		// 判定：|中心間の距離| > RA + RB
		// TはAのローカル座標なので、中心間の距離をBのローカル軸B_iに投影し直す
		if (fabsf(T[0] * R[0][i] +
				  T[1] * R[1][i] +
			      T[2] * R[2][i]) > RA + RB)
		{
			return false;
		}
	}

	//---------------------------------
	// 軸L = Ai×Bj判定(外積による9つの軸)
	// 分離条件：|投影された中心距離|>BoxAの投影長+BoxBの投影長
	//---------------------------------

	// 軸L=A0 X B0判定
	RA = A->HarfLength[1] * AbsR[2][0] + A->HarfLength[2] * AbsR[1][0];
	RB = A->HarfLength[1] * AbsR[0][2] + A->HarfLength[2] * AbsR[0][1];
	if (fabsf(T[2] * R[1][0] - T[1] * R[2][0]) > RA + RB)
	{
		return false;
	}

	// 軸L=A0 X B1判定
	RA = A->HarfLength[1] * AbsR[2][1] + A->HarfLength[2] * AbsR[1][1];
	RB = A->HarfLength[0] * AbsR[0][2] + A->HarfLength[2] * AbsR[0][0];
	if (fabsf(T[2] * R[1][1] - T[1] * R[2][1]) > RA + RB)
	{
		return false;
	}

	// 軸L=A0 X B2判定
	RA = A->HarfLength[1] * AbsR[2][2] + A->HarfLength[2] * AbsR[1][2];
	RB = A->HarfLength[0] * AbsR[0][1] + A->HarfLength[1] * AbsR[0][0];
	if (fabsf(T[2] * R[1][2] - T[1] * R[2][2]) > RA + RB)
	{
		return false;
	}

	// 軸L=A1 X B0判定
	RA = A->HarfLength[0] * AbsR[2][0] + A->HarfLength[2] * AbsR[0][0];
	RB = A->HarfLength[1] * AbsR[1][2] + A->HarfLength[2] * AbsR[1][1];
	if (fabsf(T[0] * R[2][0] - T[2] * R[0][0]) > RA + RB)
	{
		return false;
	}

	// 軸L=A1 X B1判定
	RA = A->HarfLength[0] * AbsR[2][1] + A->HarfLength[2] * AbsR[0][1];
	RB = A->HarfLength[0] * AbsR[1][2] + A->HarfLength[2] * AbsR[1][0];
	if (fabsf(T[0] * R[2][1] - T[2] * R[0][1]) > RA + RB)
	{
		return false;
	}

	// 軸L=A1 X B2判定
	RA = A->HarfLength[0] * AbsR[2][2] + A->HarfLength[2] * AbsR[0][2];
	RB = A->HarfLength[0] * AbsR[1][1] + A->HarfLength[1] * AbsR[1][0];
	if (fabsf(T[0] * R[2][2] - T[2] * R[0][2]) > RA + RB)
	{
		return false;
	}

	// 軸L=A2 X B0判定
	RA = A->HarfLength[0] * AbsR[1][0] + A->HarfLength[1] * AbsR[0][0];
	RB = A->HarfLength[1] * AbsR[2][2] + A->HarfLength[2] * AbsR[2][1];
	if (fabsf(T[1] * R[0][0] - T[0] * R[1][0]) > RA + RB)
	{
		return false;
	}

	// 軸L=A2 X B1判定
	RA = A->HarfLength[0] * AbsR[1][1] + A->HarfLength[1] * AbsR[0][1];
	RB = A->HarfLength[0] * AbsR[2][2] + A->HarfLength[2] * AbsR[2][0];
	if (fabsf(T[1] * R[0][1] - T[0] * R[1][1]) > RA + RB)
	{
		return false;
	}

	// 軸L=A2 X B2判定
	RA = A->HarfLength[0] * AbsR[1][2] + A->HarfLength[1] * AbsR[0][2];
	RB = A->HarfLength[0] * AbsR[2][1] + A->HarfLength[1] * AbsR[2][0];
	if (fabsf(T[1] * R[0][2] - T[0] * R[1][2]) > RA + RB)
	{
		return false;
	}

	return true;
}

bool CBoxCollider::CheckCollisionOBBtoSphere(const CSphereCollider& sphere, const OBB* box, D3DXVECTOR3* tempP)
{
	// OBBと球の最近接点を求める
	ClosestPointOBB(&sphere.GetPosition(), box, tempP);

	// 最近接点と球の中心とのベクトルを求める
	D3DXVECTOR3 v = *tempP - sphere.GetPosition();

	// 最近接点と球の中心との距離の二乗が、球の半径の二乗以下なら衝突
	return D3DXVec3Dot(&v, &v) <= sphere.GetRadius()* sphere.GetRadius();
}

void CBoxCollider::ClosestPointOBB(const D3DXVECTOR3* centerPoint, const OBB* box, D3DXVECTOR3* closePoint)
{
	// 入力点 *centerPointと中心 box->CenterPosの差分ベクトル d
	D3DXVECTOR3 d = *centerPoint - box->CenterPos;

	// 最近接点 *closePointの初期値を OBB の中心に設定
	*closePoint = box->CenterPos;

	// 各軸に投影して、OBBの範囲内に制限する
	float dist;
	for (int i = 0; i < 3; ++i)
	{
		// 点からOBBへのベクトルdを各軸に投影
		dist = D3DXVec3Dot(&d, &box->LocalAxes[i]);

		// 投影長をOBBの範囲内に制限
		if (dist > box->HarfLength[i])
		{
			/* 投影距離が長すぎる場合、
			   Boxの境界 -HarfLength[i] に制限*/
			dist = box->HarfLength[i];
		}
		if (dist < -box->HarfLength[i])
		{
			/* 投影距離が短すぎる場合、
			   Boxの境界 -HarfLength[i] に制限*/
			dist = -box->HarfLength[i];
		}

		// 制限された距離分だけ移動
		*closePoint += dist * box->LocalAxes[i];
	}
}

void CBoxCollider::SetPosition(const D3DXVECTOR3& pos)
{
	m_CenterPos = pos;
}

void CBoxCollider::SetBaseHalfExtents(const D3DXVECTOR3& min_local, const D3DXVECTOR3& max_local)
{
	// OBBで使用する「半分のサイズ」を計算して、基底サイズとして保持する
	m_BaseHalfExtents.x = fabsf(max_local.x - min_local.x) * 0.5f;
	m_BaseHalfExtents.y = fabsf(max_local.y - min_local.y) * 0.5f;
	m_BaseHalfExtents.z = fabsf(max_local.z - min_local.z) * 0.5f;

	// OBBの現在のサイズも初期化
	m_OBB.HarfLength = m_BaseHalfExtents;
}