#include "CComUtility.h"

#if 0
// [-ƒÎ,ƒÎ] ‚É³‹K‰»
float CComUtility::Wrap(float a)
{
    const float TWO_PI = D3DX_PI * 2.0f;
    while (a > D3DX_PI) a -= TWO_PI;
    while (a < -D3DX_PI) a += TWO_PI;
    return a;

}

//ˆê‚Â•ûŒü‚Ìstep‚É‹ß‚Ã‚¯‚é
float CComUtility::Approach(float cur, float goal, float step)
{
    const float d = goal - cur;
    if (d > step) return cur + step;
    if (d < -step) return cur - step;
    return goal;

}
#endif


