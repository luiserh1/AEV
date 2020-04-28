/*From https://en.wikipedia.org/wiki/Q_(number_format)*/
#include "intArithmetic.h"

// precomputed value:
#define K   (1 << (Q - 1))
#define Q   15

int16_t q15Add(int16_t a, int16_t b)
{
    return a + b;
}

int16_t q15AddSat(int16_t a, int16_t b)
{
    int16_t result;
    int32_t tmp;

    return 
    tmp = (int32_t)a + (int32_t)b;
    if (tmp > 0x7FFF)
        tmp = 0x7FFF;
    if (tmp < -1 * 0x8000)
        tmp = -1 * 0x8000;
    result = (int16_t)tmp;

    return result;
}

int16_t q15Sub(int16_t a, int16_t b)
{
    return a - b;
}

// saturate to range of int16_t
int16_t sat16(int32_t x)
{
	if (x > 0x7FFF) return 0x7FFF;
	else if (x < -0x8000) return -0x8000;
	else return (int16_t)x;
}

int16_t q15Mul(int16_t a, int16_t b)
{
    int16_t result;
    int32_t temp;

    temp = (int32_t)a * (int32_t)b; // result type is operand's type
    // Rounding; mid values are rounded up
    temp += K;
    // Correct by dividing by base and saturate result
    result = sat16(temp >> Q);

    return result;
}

int16_t q15Div(int16_t a, int16_t b)
{
    /* pre-multiply by the base (Upscale to Q16 so that the result will be in Q8 format) */
    int32_t temp = (int32_t)a << Q;
    /* Rounding: mid values are rounded up (down for negative values). */
    /* OR compare most significant bits i.e. if (((temp >> 31) & 1) == ((b >> 15) & 1)) */
    if ((temp >= 0 && b >= 0) || (temp < 0 && b < 0)) {   
        temp += b / 2;    /* OR shift 1 bit i.e. temp += (b >> 1); */
    } else {
        temp -= b / 2;    /* OR shift 1 bit i.e. temp -= (b >> 1); */
    }
    return (int16_t)(temp / b);
}


float q15ToFloat(int16_t q)
{
    return  q / (float)(2 << (Q-1));
}

int16_t floatToQ15(float q)
{
    return (int) (q * (2 << Q));
}

int32_t addQ15To17_15(int16_t q15, int32_t ia17_15)
{
    return ia17_15 + (int32_t)q15;
}