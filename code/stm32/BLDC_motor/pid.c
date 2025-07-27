/*
 * pid.c
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#include "pid.h"
#include <math.h>

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd,
             float output_limit, float integral_limit) {
    if (pid == NULL) return;

    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output = 0.0f;
    pid->output_limit = fabsf(output_limit);
    pid->integral_limit = fabsf(integral_limit);
}

float PID_Update(PID_Controller *pid, float setpoint, float feedback, float dt) {
    if (pid == NULL || dt <= 0.0f) return 0.0f;

    pid->setpoint = setpoint;
    float error = setpoint - feedback;

    float P = pid->Kp * error;

    pid->integral += error * dt;
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }
    float I = pid->Ki * pid->integral;

    float D = pid->Kd * (error - pid->prev_error) / dt;
    pid->prev_error = error;

    pid->output = P + I + D;
    if (pid->output > pid->output_limit) {
        pid->output = pid->output_limit;
    } else if (pid->output < -pid->output_limit) {
        pid->output = -pid->output_limit;
    }

    return pid->output;
}

void PID_Reset(PID_Controller *pid) {
    if (pid == NULL) return;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output = 0.0f;
}
