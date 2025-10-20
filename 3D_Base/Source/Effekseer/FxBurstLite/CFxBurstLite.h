#pragma once
#pragma once
#include <vector>
#include <algorithm>
#include <d3dx9math.h>
#include "Effekseer/include/Effekseer.h"
#include <Assets/Effect/CEffect.h>

//エフェクト制御クラス

class CFxBurstLite
{
public:
	//設定
	void Configure(CEffect::enList effectId,
		int poolSize = 4,
		float interval = 0.25f,
		float speed = 1.0f);

	//回数で開始
	void StartByCount(int count);

	//秒数で開始
	void StartByDuration(float seconds);

	void Update(float dt,
		const D3DXVECTOR3& pos,
		const D3DXVECTOR3& rot,
		const D3DXVECTOR3& scl);

	void Stop();

	//稼働中かどうか
	bool IsActive() const { return m_active; }

	//コピー禁止
	CFxBurstLite() = default;
	CFxBurstLite(const CFxBurstLite&) = delete;
	CFxBurstLite& operator= (const CFxBurstLite&) = delete;

	//ムーブ可能
	CFxBurstLite(CFxBurstLite&&) = default;
	CFxBurstLite& operator = (CFxBurstLite&&) = default;

private:
	using EsHandle = ::EsHandle;
	static constexpr EsHandle kInvalidFx = static_cast<EsHandle>(-1);

	CEffect::enList id = CEffect::Test0;
	std::vector<EsHandle> m_pool;
	int m_head = 0; //次に使うFXハンドルのインデックス
	float m_interval = 0.25f; //FX発生間隔
	float m_speed = 1.0f; //FXの再生速度

	//モード管理
	int m_remainCount = 0; //残り発生回数0なら回数モードで-1なら時間モード
	float m_remainSec = 0.0f; //残り時間

	//ランタイム
	float m_timer = 0.0f;
	bool m_active = false;
};
