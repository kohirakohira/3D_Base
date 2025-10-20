#include "CFxBurstLite.h"
#undef max;

void CFxBurstLite::Configure(CEffect::enList effectId,
	int poolsize, float interval, float speed)
{
	id = effectId;
	m_interval = std::max(0.0f, interval);
	m_speed = speed;
	m_pool.assign(std::max(1, poolsize), kInvalidFx);
	m_head = 0;
	//状態は触らないので、再設定しても現状の稼働は維持される
}

void CFxBurstLite::StartByCount(int count)
{
	m_active = true;
	m_remainCount = std::max(0, count);
	m_remainSec = 0.0f;	//未使用
	m_timer = 0.0f; //即1発目
}

void CFxBurstLite::StartByDuration(float seconds)
{
	m_active = true;
	m_remainCount = -1; //時間モード
	m_remainSec = std::max(0.0f, seconds);
	m_timer = 0.0f; //即1発目
}

void CFxBurstLite::Update(float dt,
	const D3DXVECTOR3& pos,
	const D3DXVECTOR3& rot,
	const D3DXVECTOR3& scl)
{
	//稼働中でなくてもハンドルの追従だけは行う
	for (auto& handle : m_pool)
	{
		if (handle != kInvalidFx)
		{
			CEffect::SetLocation(handle, pos);
		}
	}

	if (!m_active) return;

	//時間モード
	if (m_remainCount < 0)
	{
		m_remainSec -= dt;
		if (m_remainSec <= 0.0f)
		{
			m_active = false;
			return;
		}
	}

	//発生タイマー
	m_timer -= dt;
	if (m_timer > 0.0f) return;

	//一発生成
	auto& slot = m_pool[m_head];
	if (slot != kInvalidFx) {
		CEffect::Stop(slot); //古いものは置き換え
	}

	slot = CEffect::Play(id, pos);
	if (slot != kInvalidFx)
	{
		CEffect::SetSpeed(slot, m_speed);
		CEffect::SetRotation(slot, rot);
		CEffect::SetScale(slot, scl);
		CEffect::SetLocation(slot, pos);
	}
	m_head = (m_head + 1) % static_cast<int>(m_pool.size());

	//回数モードなら消費する
	if (m_remainCount > 0)
	{
		m_remainCount--;
		if (m_remainCount == 0)
		{
			m_active = false;
		}
	}
	//次までまつ
	m_timer = m_interval;
}

void CFxBurstLite::Stop()
{
	m_active = false;
	for (auto& handle : m_pool)
	{
		if (handle != kInvalidFx)
		{
			CEffect::Stop(handle);
			handle = kInvalidFx;
		}
	}
}