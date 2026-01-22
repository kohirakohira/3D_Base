#pragma once
#include <vector>
#include <memory>

class CBody;
class CCannon;
class CComObstacleAvoidance;
class CComPlayer;

/*
	COM移動制御クラス
*/

class CComMovement
{
public:
	//移動パラメータ
	struct MovementParams
	{
		float avoidRadius = 10.0f;	//他COMから離れる半径
		float avoidWeight = 2.0f;	//分離ベクトルの重み
		float keepDistance = 9.0f;	//ターゲットとの維持距離
	};

	CComMovement();
	~CComMovement() = default;

	void SetParams(const MovementParams& params) { m_Params = params; }
	MovementParams& GetParams() { return m_Params; }	//外部から変更可能
	const MovementParams& GetParams() const { return m_Params; }	//変更ふのう

	void SetObstacleAvoidance(const std::weak_ptr<CComObstacleAvoidance>& avoidance) { m_pObstacleAvoidance = avoidance; }

	//循環参照回避
	void SetOwner(const std::shared_ptr<CComPlayer>& owner) { m_pOwner = owner; }

	//前進
	void SafeAdvance(const std::shared_ptr<CBody>& body, const D3DXVECTOR3& targetPos);

	//ターゲットへの追尾移動
	void ChaseTo(const std::shared_ptr<CBody>& body, const D3DXVECTOR3& targetPos);

	//徘徊角度の更新
	void UpdateWander();
	float GetWanderAngle() const { return m_WanderAngle; }			//徘徊角度の取得
	void SetWanderAngle(float angle) { m_WanderAngle = angle; }		//徘徊角度の設定

	//COM同士の分離ベクトル
	void ComputeSeparation(
		const D3DXVECTOR3& selfPos,
		D3DXVECTOR3& outSep,
		float& outNearest) const;

	//砲塔を車体に同期
	void SyncCannonToBody(
		const std::shared_ptr<CBody>& body,
		const std::shared_ptr<CCannon>& cannon,
		float cannonHeight);


private:
	MovementParams m_Params;
	std::weak_ptr<CComObstacleAvoidance> m_pObstacleAvoidance;
	std::weak_ptr<CComPlayer> m_pOwner;	

	//徘徊
	float m_WanderAngle;
	static constexpr float WANDER_DELTA = 0.10f;
	static constexpr float WANDER_CLAMP = 0.6f;


};