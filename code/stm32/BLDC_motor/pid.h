/*
 * pid.h
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#ifndef INC_PID_H_
#define INC_PID_H_

/**
 * @brief PID controller structure
 */
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
    float output_limit;
    float integral_limit;
    float output;
    float setpoint;
} PID_Controller;

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd,
              float output_limit, float integral_limit);
float PID_Update(PID_Controller *pid, float setpoint, float feedback, float dt);
void PID_Reset(PID_Controller *pid);

#endif /* INC_PID_H_ */
