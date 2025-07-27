/*
 * adc_handler.h
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#ifndef INC_ADC_HANDLER_H_
#define INC_ADC_HANDLER_H_

#include "stm32f1xx_hal.h"
#include "adc.h"

/* Hardware configuration */
#define SHUNT_RESISTOR     0.05f
#define AMP_GAIN          20.0f
#define VBUS_DIV_RATIO    11.0f

/* ADC Channel Mapping */
typedef enum {
    ADC_CH_IA = 0,
    ADC_CH_IB = 1,
    ADC_CH_IC = 2,
    ADC_CH_VBUS = 3,
    ADC_CH_POT = 4
} ADC_Channel_t;

/* Configuration */
#define ADC_FILTER_SAMPLES   8
#define ADC_NUM_CHANNELS     5

/* Math macros */
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/* Public variables */
extern uint16_t adc_buffer[ADC_NUM_CHANNELS * ADC_FILTER_SAMPLES];

/* Function prototypes */
HAL_StatusTypeDef ADC_Handler_Init(void);
void Calibrate_Current_Offsets(uint32_t samples);
float Get_Phase_Current(ADC_Channel_t channel);
float Get_DC_Voltage(void);
float Get_Potentiometer(void);

#endif /* INC_ADC_HANDLER_H_ */
