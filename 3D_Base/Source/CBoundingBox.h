#pragma once

#include <d3dx9.h>
#include "CStaticMesh.h" // スタティックメッシュクラス

/**************************************************
*	バウンディングボックスクラス (OBB)
**************************************************/
class CBoundingBox
{
public:
    struct OBB
    {
        D3DXVECTOR3 m_Pos;     // 中心座標
        D3DXVECTOR3 m_Axis[3]; // ローカルX, Y, Z軸（単位ベクトル）
        float       m_Length[3]; // 各軸方向の半長さ（x, y, z）
    };

public:
    CBoundingBox();
    ~CBoundingBox();

    // モデルに合わせたバウンディングボックスを作成
    HRESULT CreateBoxForMesh(const CStaticMesh& pMesh);

    // 他のOBBとの当たり判定
    bool IsHitOBB(OBB& obb1, OBB& obb2);

    // OBBの回転を設定
    void SetRotation(const D3DXMATRIX& matRot);

    // 現在のOBBを取得
    const OBB& GetOBB() const { return m_OBB; }

private:
    OBB m_OBB; // このオブジェクトのOBB
};
