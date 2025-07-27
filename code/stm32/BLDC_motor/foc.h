/*
 * foc.h
 * @brief Field-Oriented Control (FOC) implementation
 * @version 1.1
 * @date 2025-07-16
 */

#ifndef FOC_H_
#define FOC_H_

#include "stm32f1xx_hal.h"
#include <math.h>

typedef struct {
    float alpha;
    float beta;
} Clarke_Components;

typedef struct {
    float d;
    float q;
} Park_Components;

typedef struct {
    float alpha;
    float beta;
} Inv_Park_Components;

// Transform functions
Clarke_Components Clarke_Transform(float a, float b, float c);
Park_Components Park_Transform(float alpha, float beta, float theta);
Inv_Park_Components Inv_Park_Transform(float d, float q, float theta);

// Main FOC update function
void FOC_Update(float I_alpha, float I_beta, float theta,
               float Vd_ref, float Vq_ref,
               float *pwm1, float *pwm2, float *pwm3);

#endif /* FOC_H_ */
