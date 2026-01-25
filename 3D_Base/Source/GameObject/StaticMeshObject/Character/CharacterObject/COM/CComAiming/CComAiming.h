#pragma once
#include <memory>

class CCannon;
class CBody;
struct TurretParams;
struct PredictedShot;
class CComShot;

/*
	COMのAIM処理をする
*/

class CComAiming
{
public:
	struct Config
	{
		float turretTurnSpeed = 0.03f;	//砲塔をどれくらい回すか
		float cannonHeight = 0.3f;		//砲塔の高さ
		float smoothFactor = 0.25f;		//ターゲット位置の平滑化の強さ
	};

	CComAiming();

	void SetConfig(const Config& config) { m_Config = config; }
	Config& GetConfig() { return m_Config; }

	//砲塔をターゲットに向ける
	void AimAt(
		std::shared_ptr<CCannon> cannon,
		std::shared_ptr<CBody> body,
		const D3DXVECTOR3& targetPos,
		const D3DXVECTOR3& targetVelocity,
		const TurretParams& turretParams,
		CComShot& comShot
	);

	//砲塔を車体に同期
	void SyncToBody(
		std::shared_ptr<CCannon> cannon,
		std::shared_ptr<CBody> body
	);

	//平滑化されたターゲット位置を取得
	D3DXVECTOR3 GetSmoothedTargetPos() const { return m_SmoothedTargetPos; }

private:
	Config m_Config;
	D3DXVECTOR3 m_SmoothedTargetPos;

};