/**
 * @file motor.h
 * @brief BLDC Motor Controller using Field-Oriented Control (FOC)
 * @version 1.2
 * @date 2025-07-16
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "pid.h"
#include "observer.h"
#include "foc.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <math.h>

/* Default Motor Parameters */
#define DEFAULT_POLE_PAIRS          7
#define DEFAULT_PHASE_RESISTANCE    0.4f
#define DEFAULT_PHASE_INDUCTANCE    0.001f
#define DEFAULT_MAX_CURRENT         10.0f
#define DEFAULT_MAX_VOLTAGE         48.0f
#define DEFAULT_SHUNT_RESISTOR      0.05f
#define DEFAULT_AMP_GAIN            20.0f

/* Safety Limits */
#define MIN_VOLTAGE_THRESHOLD      12.0f
#define MAX_STARTUP_ATTEMPTS       3

typedef enum {
    MOTOR_STOPPED,
    MOTOR_STARTUP,
    MOTOR_RUNNING,
    MOTOR_FAULT
} Motor_State;

typedef struct {
    Motor_State state;
    float target_speed;     // [rad/s]
    float actual_speed;     // [rad/s]
    float max_current;      // [A]
    float max_voltage;      // [V]
    float pole_pairs;
    float phase_resistance; // [Ω]
    float phase_inductance; // [H]

    /* Control Loops */
    PID_Controller speed_pid;
    PID_Controller current_q_pid;
    PID_Controller current_d_pid;

    /* Sensorless Observer */
    SlidingModeObserver observer;

    /* Current Measurements */
    float I_alpha;          // α-axis current
    float I_beta;           // β-axis current
    float I_d;              // d-axis current
    float I_q;              // q-axis current

    /* Startup parameters */
    float startup_duty;
    float startup_time;
    float startup_elapsed;
    uint8_t startup_attempts;

    /* Current Sensing */
    float current_offsets[3];
    float shunt_resistor;
    float amp_gain;
} Motor_Controller;

/* Function Prototypes */
void Motor_Config_Init(Motor_Controller *motor);
void Motor_Init(Motor_Controller *motor);
void Motor_Update(Motor_Controller *motor, float Ia, float Ib, float Ic,
                 float Vdc, float dt, float *pwm1, float *pwm2, float *pwm3);
void Motor_Set_Speed(Motor_Controller *motor, float speed);
void Motor_Stop(Motor_Controller *motor);
void Current_Sensor_Calibrate(Motor_Controller *motor, ADC_HandleTypeDef *hadc);

#endif /* INC_MOTOR_H_ */
