#pragma once

/*
ユーティリティクラス
メンバ関数ではない
*/

//角度計算用マクロ.
# define L_PI			(3.1415f)
#define  D3DX_PI		((FLOAT)  3.141592654f)


#include <cmath>	//計算用

namespace Util	//省略
{
	inline static float PI()
	{
		return D3DX_PI;
	}

	inline static float TWO_PI()
	{
		return D3DX_PI * 2.0f;
	}

	//[-π,π]に正規化
	inline float Wrap(float a)
	{
		while (a > PI()) a -= TWO_PI();
		while (a < -PI()) a += TWO_PI();
		return a;
	}

	//一方向にstep分だけ近づける
	inline static float Approach(float cur, float goal, float step)
	{
		const float d = goal - cur;
		if (d > step)  return cur + step;
		if (d < -step) return cur - step;
		return goal;
	}

	inline static D3DXVECTOR3 ForwardFromYaw(float yaw)
	{
		return D3DXVECTOR3(sinf(yaw), 0.0f, cosf(yaw));
	}

	inline static float Deg2Red(float d)
	{
		return d * (D3DX_PI / 180.f);
	}

	inline float DistXZ(const D3DXVECTOR3& targetPos, const D3DXVECTOR3& selfPos)
	{
		const float dx = targetPos.x - selfPos.x;
		const float dz = targetPos.z - selfPos.z;
		return std::sqrtf(dx * dx + dz * dz);
	}

	inline float AngleError(float fromYaw, const D3DXVECTOR3& fromPos, const D3DXVECTOR3& toPos)
	{
		D3DXVECTOR3 vec = toPos - fromPos;
		vec.y = 0.f;

		if (vec.x == 0 && vec.z == 0)
		{
			return 0.f;
		}

		const float desired = std::atan2f(vec.x, vec.z);
		const float error = Util::Wrap(desired - fromYaw);
		return std::fabs(error);
	}

	// ヘルパ
	inline static float AngleDeadband(float a, float epsRad) {
		return (std::fabs(a) < epsRad) ? 0.0f : a;
	}

	inline static float ToRad(float d)
	{
		return d * (D3DX_PI / 180.f);
	}

	inline static float ClampF(float v, float lo, float hi)
	{
		return (v < lo) ? lo : (v > hi) ? hi : v;

	}

	//動作切替
	inline static float Sqr(float v)
	{
		return v * v;
	}

};