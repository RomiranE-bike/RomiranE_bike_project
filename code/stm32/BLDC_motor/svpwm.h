/*
 * svpwm.h
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#ifndef INC_SVPWM_H_
#define INC_SVPWM_H_

#include <math.h>

// Mathematical constants for SVPWM
#define SQRT3_2         0.86602540378f   // sqrt(3)/2
#define ONE_BY_SQRT3    0.57735026919f   // 1/sqrt(3)

/**
 * @brief Generate 3-phase PWM duties using Space Vector Modulation
 * @param alpha,beta Stationary frame voltage components
 * @param pwm1-3 Output PWM duties [0-1]
 */
void SVPWM_Generate(float alpha, float beta, float *pwm1, float *pwm2, float *pwm3);

#endif /* INC_SVPWM_H_ */
