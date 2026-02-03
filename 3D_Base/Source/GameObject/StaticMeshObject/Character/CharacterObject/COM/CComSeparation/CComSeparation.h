#pragma once

/*
	COM同士が重なり合わないようにするための分離計算クラス
*/

#include <vector>
#include <memory>

class CBody;

//分離計算の結果
struct SeparationResult
{
	D3DXVECTOR3 direction;	//分離方向ベクトル
	float nearestDist;		//最も近いCOMまでの距離
	int neighborCount;		//範囲内COM数

	SeparationResult()
		: direction(0.0f, 0.0f, 0.0f)
		, nearestDist(1e9f)
		, neighborCount(0)
	{
	}
};

//COM分離クラス
class CComSeparation
{
public:

	//設定構造体
	struct Config
	{
		float avoidRadius;	//この距離以内のCOMから離れる
		float avoidWeight;	//分離ベクトルの重み

		Config()
			: avoidRadius(10.f)
			, avoidWeight(2.0f)
		{
		}
	};

	CComSeparation();
	~CComSeparation() = default;

	//Configの設定・取得
	void SetConfig(const Config& config) { m_Config = config; }
	const Config& GetConfig() const { return m_Config; }

	//
	void SetAvoidRadius(float radius) { m_Config.avoidRadius = radius; }
	void SetAvoidWeight(float weight) { m_Config.avoidWeight = weight; }

	float GetAvoidRadius() const { return m_Config.avoidRadius; }
	float GetAvoidWeight() const { return m_Config.avoidWeight; }

private:
	Config m_Config;
};