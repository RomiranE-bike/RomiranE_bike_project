/*
 * observer.c
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#include "observer.h"
#include <math.h>

#define SIGN(x) ((x) > 0 ? 1.0f : ((x) < 0 ? -1.0f : 0.0f))

float Observer_GetAngle(const SlidingModeObserver *obs) {
    return obs ? obs->angle : 0.0f;
}

void Observer_Init(SlidingModeObserver *obs, float dt) {
    if (obs == NULL) return;

    obs->angle = 0.0f;
    obs->speed = 0.0f;
    obs->K1 = 100.0f;
    obs->K2 = 10.0f;
    obs->Ls = 0.0f;
    obs->Rs = 0.0f;
    obs->dt = dt;
}

float Observer_Update(SlidingModeObserver *obs, float alpha, float beta,
                    float pwm1, float pwm2, float pwm3, float Vdc, float dt) {
    if (!obs) return 0.0f;

    float Valpha = (pwm1 - 0.5f) * Vdc;
    float Vbeta = (pwm2 - 0.5f) * Vdc;

    float e_alpha = obs->alpha_est - alpha;
    float e_beta = obs->beta_est - beta;

    float z_alpha = obs->K1 * SIGN(e_alpha);
    float z_beta = obs->K1 * SIGN(e_beta);

    float Ealpha = z_alpha;
    float Ebeta = z_beta;

    obs->alpha_est += dt * ((Valpha - obs->Rs * alpha - Ealpha) / obs->Ls);
    obs->beta_est += dt * ((Vbeta - obs->Rs * beta - Ebeta) / obs->Ls);

    float angle_est = atan2f(-Ealpha, Ebeta);

    float angle_diff = angle_est - obs->angle;
    while(angle_diff > M_PI) angle_diff -= 2*M_PI;
    while(angle_diff < -M_PI) angle_diff += 2*M_PI;

    obs->speed = angle_diff / dt;
    obs->angle = angle_est;

    while(obs->angle > 2*M_PI) obs->angle -= 2*M_PI;
    while(obs->angle < 0) obs->angle += 2*M_PI;

    return obs->speed;
}
