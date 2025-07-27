#ifndef INC_OBSERVER_H_
#define INC_OBSERVER_H_

#include <stdint.h>

typedef struct {
    float angle;
    float speed;
    float alpha_est;
    float beta_est;
    float K1;
    float K2;
    float Ls;
    float Rs;
    float dt;
} SlidingModeObserver;

void Observer_Init(SlidingModeObserver *obs, float dt);
float Observer_GetAngle(const SlidingModeObserver *obs);
float Observer_Update(SlidingModeObserver *obs, float alpha, float beta,
                    float pwm1, float pwm2, float pwm3, float Vdc, float dt);

#endif /* INC_OBSERVER_H_ */
