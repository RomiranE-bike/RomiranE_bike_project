/**
 * @file motor.c
 * @brief BLDC Motor Controller Implementation
 * @version 1.2
 * @date 2025-07-16
 */

#include "stm32f1xx_hal.h"
#include "motor.h"
#include "foc.h"
#include <math.h>
#include <string.h>

void Motor_Config_Init(Motor_Controller *motor) {
    if (motor == NULL) return;

    motor->pole_pairs = DEFAULT_POLE_PAIRS;
    motor->phase_resistance = DEFAULT_PHASE_RESISTANCE;
    motor->phase_inductance = DEFAULT_PHASE_INDUCTANCE;
    motor->max_current = DEFAULT_MAX_CURRENT;
    motor->max_voltage = DEFAULT_MAX_VOLTAGE;
    motor->shunt_resistor = DEFAULT_SHUNT_RESISTOR;
    motor->amp_gain = DEFAULT_AMP_GAIN;
}

void Motor_Init(Motor_Controller *motor) {
    if (motor == NULL) return;

    memset(motor, 0, sizeof(Motor_Controller));

    motor->state = MOTOR_STOPPED;
    motor->target_speed = 0.0f;
    motor->actual_speed = 0.0f;

    Motor_Config_Init(motor);

    // Initialize PID controllers
    PID_Init(&motor->speed_pid, 0.1f, 0.05f, 0.001f,
             motor->max_current, motor->max_current);
    PID_Init(&motor->current_q_pid, 1.0f, 0.1f, 0.0f,
             motor->max_voltage, motor->max_voltage);
    PID_Init(&motor->current_d_pid, 1.0f, 0.1f, 0.0f,
             motor->max_voltage, motor->max_voltage);

    // Initialize observer
    Observer_Init(&motor->observer, 0.00005f);
    motor->observer.Ls = motor->phase_inductance;
    motor->observer.Rs = motor->phase_resistance;

    // Startup parameters
    motor->startup_duty = 0.1f;
    motor->startup_time = 1.0f;
    motor->startup_elapsed = 0.0f;
    motor->startup_attempts = 0;
}

void Motor_Update(Motor_Controller *motor, float Ia, float Ib, float Ic,
                 float Vdc, float dt, float *pwm1, float *pwm2, float *pwm3) {
    if (motor == NULL || pwm1 == NULL || pwm2 == NULL || pwm3 == NULL) return;

    switch(motor->state) {
        case MOTOR_STOPPED:
            *pwm1 = *pwm2 = *pwm3 = 0.5f;
            break;

        case MOTOR_STARTUP:
            motor->startup_elapsed += dt;
            if (motor->startup_elapsed >= motor->startup_time) {
                motor->state = MOTOR_RUNNING;
            }
            *pwm1 = *pwm2 = *pwm3 = motor->startup_duty;
            break;

        case MOTOR_RUNNING: {
            if (Vdc < MIN_VOLTAGE_THRESHOLD) {
                motor->state = MOTOR_FAULT;
                *pwm1 = *pwm2 = *pwm3 = 0.5f;
                break;
            }

            float theta = Observer_GetAngle(&motor->observer);

            Clarke_Components clarke = Clarke_Transform(Ia, Ib, Ic);
            motor->I_alpha = clarke.alpha;
            motor->I_beta = clarke.beta;

            Park_Components park = Park_Transform(clarke.alpha, clarke.beta, theta);
            motor->I_d = park.d;
            motor->I_q = park.q;

            // Calculate speed error directly in PID call
            float I_q_ref = PID_Update(&motor->speed_pid, motor->target_speed, motor->actual_speed, dt);
            I_q_ref = fmaxf(fminf(I_q_ref, motor->max_current), -motor->max_current);

            float V_d_ref = PID_Update(&motor->current_d_pid, 0.0f, motor->I_d, dt);
            float V_q_ref = PID_Update(&motor->current_q_pid, I_q_ref, motor->I_q, dt);

            FOC_Update(motor->I_alpha, motor->I_beta, theta, V_d_ref, V_q_ref, pwm1, pwm2, pwm3);

            motor->actual_speed = Observer_Update(&motor->observer,
                                                motor->I_alpha, motor->I_beta,
                                                *pwm1, *pwm2, *pwm3, Vdc, dt);
            break;
        }

        case MOTOR_FAULT:
            *pwm1 = *pwm2 = *pwm3 = 0.0f;
            break;

        default:
            *pwm1 = *pwm2 = *pwm3 = 0.5f;
            motor->state = MOTOR_STOPPED;
            break;
    }
}

void Motor_Set_Speed(Motor_Controller *motor, float speed) {
    if (motor == NULL) return;

    if (speed == 0.0f) {
        Motor_Stop(motor);
    } else {
        motor->target_speed = speed;
        if (motor->state == MOTOR_STOPPED) {
            motor->state = MOTOR_STARTUP;
            motor->startup_elapsed = 0.0f;
        }
    }
}

void Motor_Stop(Motor_Controller *motor) {
    if (motor == NULL) return;

    motor->state = MOTOR_STOPPED;
    motor->target_speed = 0.0f;

    PID_Reset(&motor->speed_pid);
    PID_Reset(&motor->current_q_pid);
    PID_Reset(&motor->current_d_pid);
}

void Current_Sensor_Calibrate(Motor_Controller *motor, ADC_HandleTypeDef *hadc) {
    if (motor == NULL || hadc == NULL) return;

    float sum[3] = {0};
    uint16_t adc_buffer[3*8];

    HAL_ADC_Start_DMA(hadc, (uint32_t*)adc_buffer, 24);
    HAL_Delay(10);

    for (int i = 0; i < 8; i++) {
        sum[0] += adc_buffer[i];
        sum[1] += adc_buffer[8+i];
        sum[2] += adc_buffer[16+i];
    }

    motor->current_offsets[0] = sum[0]/8;
    motor->current_offsets[1] = sum[1]/8;
    motor->current_offsets[2] = sum[2]/8;
}
