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

/*Get a float number from th q15 number*/
float q15ToFloat(int16_t q);

/*Get a q15 number from a float*/
int16_t floatToQ15(float q);

/*17.15 plus q15 addition*/
int32_t addQ15To17_15(int16_t q15, int32_t ia17_15);

#endif
