/*
 * adc_handler.c
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#include "adc_handler.h"
#include "main.h"
#include <math.h>

/* Private variables */
static float current_offsets[3] = {0};
uint16_t adc_buffer[ADC_NUM_CHANNELS * ADC_FILTER_SAMPLES] = {0};

HAL_StatusTypeDef ADC_Handler_Init(void)
{
    HAL_StatusTypeDef status;

    MX_ADC1_Init();
    status = HAL_ADCEx_Calibration_Start(&hadc1);
    if (status != HAL_OK) return status;

    return HAL_ADC_Start_DMA(&hadc1,
                           (uint32_t*)adc_buffer,
                           ADC_NUM_CHANNELS * ADC_FILTER_SAMPLES);
}

void Calibrate_Current_Offsets(uint32_t samples)
{
    uint32_t sum[3] = {0};
    uint32_t valid_samples = MIN(samples, ADC_FILTER_SAMPLES);

    if (HAL_ADC_GetState(&hadc1) == HAL_ADC_STATE_REG_EOC) {
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer,
                         ADC_NUM_CHANNELS * ADC_FILTER_SAMPLES);
    }

    for (uint32_t i = 0; i < valid_samples; i++) {
        sum[0] += adc_buffer[ADC_CH_IA * ADC_FILTER_SAMPLES + i];
        sum[1] += adc_buffer[ADC_CH_IB * ADC_FILTER_SAMPLES + i];
        sum[2] += adc_buffer[ADC_CH_IC * ADC_FILTER_SAMPLES + i];
    }

    current_offsets[0] = (float)sum[0] / valid_samples;
    current_offsets[1] = (float)sum[1] / valid_samples;
    current_offsets[2] = (float)sum[2] / valid_samples;
}

float Get_Phase_Current(ADC_Channel_t channel)
{
    if (channel > ADC_CH_IC) return 0.0f;

    uint32_t sum = 0;
    for (uint32_t i = 0; i < ADC_FILTER_SAMPLES; i++) {
        sum += adc_buffer[channel * ADC_FILTER_SAMPLES + i];
    }
    float voltage = ((float)sum / ADC_FILTER_SAMPLES - current_offsets[channel]) * (3.3f / 4095.0f);
    return voltage / (SHUNT_RESISTOR * AMP_GAIN);
}

float Get_DC_Voltage(void)
{
    uint32_t sum = 0;
    for (uint32_t i = 0; i < ADC_FILTER_SAMPLES; i++) {
        sum += adc_buffer[ADC_CH_VBUS * ADC_FILTER_SAMPLES + i];
    }
    return ((float)sum / ADC_FILTER_SAMPLES) * (3.3f / 4095.0f) * VBUS_DIV_RATIO;
}

float Get_Potentiometer(void)
{
    uint32_t sum = 0;
    for (uint32_t i = 0; i < ADC_FILTER_SAMPLES; i++) {
        sum += adc_buffer[ADC_CH_POT * ADC_FILTER_SAMPLES + i];
    }
    return ((float)sum / ADC_FILTER_SAMPLES) / 4095.0f;
}
