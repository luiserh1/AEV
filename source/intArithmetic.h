#ifndef INTARITHMETIC_H
#define INTARITHMETIC_H

#include <sys/types.h>

/*Q15 integers addition*/
int16_t q15Add(int16_t a, int16_t b);

/*Q15 integers addition with saturation*/
int16_t q15AddSat(int16_t a, int16_t b);

/*Q15 integers substraction*/
int16_t q15Sub(int16_t a, int16_t b);

/*Q15 integers multiplication*/
int16_t q15Mul(int16_t a, int16_t b);

/*Q15 integers division*/
int16_t q15Div(int16_t a, int16_t b);

float q15ToFloat(int16_t q);

int16_t floatToQ15(float q);

#endif
