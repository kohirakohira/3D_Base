#pragma once
//#include "GameObject/StaticMeshObject/Character/COM/CComPlayer.h"	//COMクラス
#include "GameObject/StaticMeshObject/Character/Player/PlayerTank/CPlayer.h"    //プレイヤークラス

//前方宣言
class CComPlayer;

#include <vector>   //ライブラリ
#include <memory>


//COMの動作に必要な情報

class ComData : public CPlayer
{
public:
	//常にターゲットの方向をむく
	void TickChaseTo(const D3DXVECTOR3& target, float AvoidWeight, float KeepDistance, float AvoidRadius);

    //砲塔があれば常にターゲットの方向を見る
    void TickAimTo(const D3DXVECTOR3& targetPos);

    //一方向にstepだけ近づける
    float Approach(float cur, float goal, float step)
    {
        const float d = goal - cur;
        if (d > step)  return cur + step;
        if (d < -step) return cur - step;
        return goal;
    }

    static float PI() { return D3DX_PI; }
    static float TWO_PI() { return D3DX_PI * 2.0f; }

    //[-π,π]に正規化
    float Wrap(float a)
    {
        while (a > PI())     a -= TWO_PI();
        while (a < -PI())     a += TWO_PI();
        return a;
    }

    D3DXVECTOR3 ForwardFromYaw(float yaw)
    {
        return D3DXVECTOR3(std::sinf(yaw), 0.0f, std::cosf(yaw));
    }


};