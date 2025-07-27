/*
 * foc.c
 * @brief Field-Oriented Control implementation
 * @version 1.1
 * @date 2025-07-16
 */

#include "foc.h"
#include "svpwm.h"

/**
 * @brief Clarke transform (3-phase to αβ)
 */
Clarke_Components Clarke_Transform(float a, float b, float c) {
    Clarke_Components clarke;
    clarke.alpha = a;  // Phase A maps directly to alpha
    clarke.beta = (1.0f/sqrtf(3.0f)) * (b - c);  // (b - c) / sqrt(3)
    return clarke;
}

/**
 * @brief Park transform (αβ to dq)
 */
Park_Components Park_Transform(float alpha, float beta, float theta) {
    Park_Components park;
    float sin_t = sinf(theta);
    float cos_t = cosf(theta);
    park.d = alpha * cos_t + beta * sin_t;
    park.q = -alpha * sin_t + beta * cos_t;
    return park;
}

/**
 * @brief Inverse Park transform (dq to αβ)
 */
Inv_Park_Components Inv_Park_Transform(float d, float q, float theta) {
    Inv_Park_Components inv_park;
    float sin_t = sinf(theta);
    float cos_t = cosf(theta);
    inv_park.alpha = d * cos_t - q * sin_t;
    inv_park.beta = d * sin_t + q * cos_t;
    return inv_park;
}

/**
 * @brief Main FOC update function
 */
void FOC_Update(float I_alpha, float I_beta, float theta,
               float Vd_ref, float Vq_ref,
               float *pwm1, float *pwm2, float *pwm3) {
    // 1. Perform inverse Park transform to get αβ voltages
    Inv_Park_Components inv_park = Inv_Park_Transform(Vd_ref, Vq_ref, theta);

    // 2. Generate PWM signals using Space Vector Modulation
    SVPWM_Generate(inv_park.alpha, inv_park.beta, pwm1, pwm2, pwm3);
}
