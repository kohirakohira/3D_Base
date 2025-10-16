#include "CBoundingBox.h"

CBoundingBox::CBoundingBox()
{
}

CBoundingBox::~CBoundingBox()
{
}

HRESULT CBoundingBox::CreateBoxForMesh(const CStaticMesh& pMesh)
{
    LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
    void* pVertices = nullptr;
    D3DXVECTOR3 vMin, vMax;

    if (FAILED(pMesh.GetMesh()->GetVertexBuffer(&pVB)))
        return E_FAIL;
    if (FAILED(pVB->Lock(0, 0, &pVertices, 0)))
    {
        SAFE_RELEASE(pVB);
        return E_FAIL;
    }

    // AABBをまず計算する（最小と最大）
    D3DXComputeBoundingBox(
        static_cast<D3DXVECTOR3*>(pVertices),
        pMesh.GetMesh()->GetNumVertices(),
        D3DXGetFVFVertexSize(pMesh.GetMesh()->GetFVF()),
        &vMin,
        &vMax);

    pVB->Unlock();
    SAFE_RELEASE(pVB);

    // 中心
    m_OBB.m_Pos = (vMin + vMax) * 0.5f;

    // 軸：最初はワールド座標軸と一致
    m_OBB.m_Axis[0] = D3DXVECTOR3(1, 0, 0);  // X軸
    m_OBB.m_Axis[1] = D3DXVECTOR3(0, 1, 0);  // Y軸
    m_OBB.m_Axis[2] = D3DXVECTOR3(0, 0, 1);  // Z軸

    // 各軸の長さ（半分）
    D3DXVECTOR3 halfSize = (vMax - vMin) * 0.5f;
    m_OBB.m_Length[0] = halfSize.x;
    m_OBB.m_Length[1] = halfSize.y;
    m_OBB.m_Length[2] = halfSize.z;

    return S_OK;
}

// 2つのOBBの衝突判定（Separating Axis Theoremに基づく）
bool CBoundingBox::IsHitOBB(OBB& obb1, OBB& obb2)
{
    // obb1の各軸方向（正規化ベクトル）とその長さを使って軸ベクトルを計算
    D3DXVECTOR3 NAe1 = obb1.m_Axis[0], Ae1 = NAe1 * obb1.m_Length[0];
    D3DXVECTOR3 NAe2 = obb1.m_Axis[1], Ae2 = NAe2 * obb1.m_Length[1];
    D3DXVECTOR3 NAe3 = obb1.m_Axis[2], Ae3 = NAe3 * obb1.m_Length[2];

    // obb2の各軸方向（正規化ベクトル）とその長さを使って軸ベクトルを計算
    D3DXVECTOR3 NBe1 = obb2.m_Axis[0], Be1 = NBe1 * obb2.m_Length[0];
    D3DXVECTOR3 NBe2 = obb2.m_Axis[1], Be2 = NBe2 * obb2.m_Length[1];
    D3DXVECTOR3 NBe3 = obb2.m_Axis[2], Be3 = NBe3 * obb2.m_Length[2];

    // OBB同士の中心点間のベクトル（中心間距離）
    D3DXVECTOR3 Interval = obb1.m_Pos - obb2.m_Pos;

    // 分離軸に投影されたときの「幅」を計算するラムダ式
    auto LenSegOnSeparateAxis = [](const D3DXVECTOR3& Sep, const D3DXVECTOR3& e1, const D3DXVECTOR3& e2, const D3DXVECTOR3& e3)
    {
        // それぞれの軸ベクトルを分離軸に投影し、絶対値の合計を取る（スカラー幅）
        float r1 = fabs(D3DXVec3Dot(&Sep, &e1));
        float r2 = fabs(D3DXVec3Dot(&Sep, &e2));
        float r3 = fabs(D3DXVec3Dot(&Sep, &e3));
        return r1 + r2 + r3;  // 合計がそのオブジェクトの分離軸上の半幅になる
    };

    float rA, rB, L;

    // 任意の分離軸に対して衝突していないか確認するラムダ式
    auto checkAxis = [&](const D3DXVECTOR3& axis) -> bool
    {
        // obb1を分離軸に投影したときの半幅
        rA = LenSegOnSeparateAxis(axis, Ae1, Ae2, Ae3);
        // obb2を分離軸に投影したときの半幅
        rB = LenSegOnSeparateAxis(axis, Be1, Be2, Be3);
        // 中心間距離を分離軸に投影したスカラー距離
        L = fabs(D3DXVec3Dot(&Interval, &axis));
        // L > rA + rB であれば分離軸が存在 → 衝突していない
        return L > rA + rB;
    };

    // 判定に使う15の軸（3+3+9）を格納する配列
    D3DXVECTOR3 axes[] = {
        NAe1, NAe2, NAe3,    // obb1のローカル軸（3）
        NBe1, NBe2, NBe3,    // obb2のローカル軸（3）
        D3DXVECTOR3(), D3DXVECTOR3(), D3DXVECTOR3(),  // クロス軸（以下、9）
        D3DXVECTOR3(), D3DXVECTOR3(), D3DXVECTOR3(),
        D3DXVECTOR3(), D3DXVECTOR3(), D3DXVECTOR3()
    };

    // obb1とobb2の軸ベクトルの外積（クロス積）を計算
    // 各軸の組み合わせに対して直交ベクトルを生成
    D3DXVec3Cross(&axes[6], &NAe1, &NBe1);
    D3DXVec3Cross(&axes[7], &NAe1, &NBe2);
    D3DXVec3Cross(&axes[8], &NAe1, &NBe3);
    D3DXVec3Cross(&axes[9], &NAe2, &NBe1);
    D3DXVec3Cross(&axes[10], &NAe2, &NBe2);
    D3DXVec3Cross(&axes[11], &NAe2, &NBe3);
    D3DXVec3Cross(&axes[12], &NAe3, &NBe1);
    D3DXVec3Cross(&axes[13], &NAe3, &NBe2);
    D3DXVec3Cross(&axes[14], &NAe3, &NBe3);

    // 15軸すべてについて分離軸が存在するかをチェック
    for (int i = 0; i < 15; ++i)
    {
        // 無効な軸（長さがほぼゼロ）はスキップ（外積が0ベクトルの場合）
        if (D3DXVec3LengthSq(&axes[i]) < 1e-6f) continue;

        // 軸を正規化（単位ベクトル化）
        D3DXVec3Normalize(&axes[i], &axes[i]);

        // この軸で分離できれば衝突していないと判定
        if (checkAxis(axes[i]))
            return false;
    }

    // すべての軸で分離できなかった → 衝突している
    return true;
}

void CBoundingBox::SetRotation(const D3DXMATRIX& matRot)
{
    for (int i = 0; i < 3; i++)
    {
		D3DXVec3TransformNormal(&m_OBB.m_Axis[i], &m_OBB.m_Axis[i], &matRot);
    }
}
