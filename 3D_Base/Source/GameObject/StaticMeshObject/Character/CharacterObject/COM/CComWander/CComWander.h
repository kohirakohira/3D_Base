#pragma once

class CComWander
{
public:
	CComWander();

	void Update();
	float GetAngle() const { return m_Angle; }
	void Reset() { m_Angle = 0.0f; }

private:
	float m_Angle;
	static constexpr float DELTA = 0.10f;
	static constexpr float CLAMP = 0.6f;
};