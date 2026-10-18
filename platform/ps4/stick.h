#ifndef AVP_PS4_STICK_H
#define AVP_PS4_STICK_H
#include <math.h>
/* Exact neutral/endpoints, radial deadzone and normalized diagonals. */
static void avp_stick(unsigned char bx, unsigned char by, float deadzone,
                      int curved, float *ox, float *oy)
{
    float x = ((int)bx - 128) / (bx < 128 ? 128.0f : 127.0f);
    float y = ((int)by - 128) / (by < 128 ? 128.0f : 127.0f);
    float length = sqrtf(x*x + y*y);
    *ox = *oy = 0;
    if (length <= deadzone) return;
    float radius = ((length < 1.0f ? length : 1.0f) - deadzone) / (1.0f-deadzone);
    if (curved) radius *= 0.35f + 0.65f * radius;
    *ox = x * radius / length;
    *oy = y * radius / length;
}
#endif
