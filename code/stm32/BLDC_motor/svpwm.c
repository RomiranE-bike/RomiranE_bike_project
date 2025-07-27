/*
 * svpwm.c
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#include "svpwm.h"
#include <stdint.h>  // For uint8_t and other standard integer types
#include <math.h>
/**
 * @brief Implement SVPWM algorithm
 */
void SVPWM_Generate(float alpha, float beta, float *pwm1, float *pwm2, float *pwm3) {
    // 1. Sector determination
    float vref1 = beta;
    float vref2 = SQRT3_2 * alpha - 0.5f * beta;
    float vref3 = -SQRT3_2 * alpha - 0.5f * beta;

    uint8_t sector = 0;
    if(vref1 > 0) sector |= 0x01;
    if(vref2 > 0) sector |= 0x02;
    if(vref3 > 0) sector |= 0x04;

    // 2. Calculate duty cycles per sector
    float t1, t2;
    switch(sector) {
        case 1: t1 = vref1; t2 = vref2; break;  // Sector 1
        case 2: t1 = -vref3; t2 = -vref1; break; // Sector 2
        case 3: t1 = vref2; t2 = vref3; break;   // Sector 3
        case 4: t1 = -vref1; t2 = -vref2; break; // Sector 4
        case 5: t1 = vref3; t2 = vref1; break;   // Sector 5
        case 6: t1 = -vref2; t2 = -vref3; break; // Sector 6
        default: t1 = 0; t2 = 0; break;          // Invalid sector
    }

    // 3. Normalize times
    float t0 = 1.0f - t1 - t2;  // Zero vector time

    // 4. Generate PWM based on sector
    switch(sector) {
        case 1: *pwm1 = t1+t2+t0/2; *pwm2 = t2+t0/2; *pwm3 = t0/2; break;
        case 2: *pwm1 = t1+t0/2; *pwm2 = t1+t2+t0/2; *pwm3 = t0/2; break;
        case 3: *pwm1 = t0/2; *pwm2 = t1+t2+t0/2; *pwm3 = t2+t0/2; break;
        case 4: *pwm1 = t0/2; *pwm2 = t1+t0/2; *pwm3 = t1+t2+t0/2; break;
        case 5: *pwm1 = t2+t0/2; *pwm2 = t0/2; *pwm3 = t1+t2+t0/2; break;
        case 6: *pwm1 = t1+t2+t0/2; *pwm2 = t0/2; *pwm3 = t1+t0/2; break;
        default: *pwm1 = 0.5f; *pwm2 = 0.5f; *pwm3 = 0.5f; break; // Default to 50%
    }
}
