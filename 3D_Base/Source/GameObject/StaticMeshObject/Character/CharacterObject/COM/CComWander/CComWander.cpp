#include "CComWander.h"

CComWander::CComWander()
	: m_Angle (0.0f)
{
}

void CComWander::Update()
{
    // ‚½‚Ü‚É‚¾‚¯•ûŒü‚ð—h‚ç‚·
    if ((std::rand() & 31) == 0) // 1/32ƒtƒŒ[ƒ€‚®‚ç‚¢
    {
#if 1
        if (std::rand() & 1)
        {
            const float sign = 1.0f;
            m_Angle += sign * DELTA;
        }
        else
        {
            const float sign = -1.0f;
        }
#endif
#if 0
        const float sign = (std::rand() & 1) ? +1.f : -1.f;
        m_Angle += sign * DELTA;
#endif
        if (m_Angle > CLAMP) m_Angle = CLAMP;
        if (m_Angle < -CLAMP) m_Angle = -CLAMP;
    }

}