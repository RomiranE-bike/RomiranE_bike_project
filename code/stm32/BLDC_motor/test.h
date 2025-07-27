/*
 * test.h
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#ifndef __TEST_H
#define __TEST_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

// Test result enumeration
typedef enum {
    TEST_PASSED,
    TEST_FAILED,
    TEST_WARNING
} TestResult;

// Test categories
typedef enum {
    TEST_GPIO,
    TEST_ADC,
    TEST_PWM,
    TEST_TIMER,
    TEST_COMMS,
    TEST_SYSTEM
} TestCategory;

// Test structure
typedef struct {
    const char* name;
    TestCategory category;
    TestResult (*function)(void);
    const char* description;
} TestCase;

// LED patterns for test feedback
typedef enum {
    LED_OFF,
    LED_SOLID,
    LED_BLINK_SLOW,
    LED_BLINK_FAST,
    LED_BLINK_VERY_FAST,
    LED_PATTERN_3_SHORT,
    LED_PATTERN_2_SHORT_1_LONG
} LEDPattern;

// Exported functions
void TEST_RunAll(void);
void TEST_IndicateResult(TestResult result);
void TEST_SetLED(LEDPattern pattern);
void TEST_SendUART(const char* message);
void TEST_Beep(uint8_t count, uint16_t duration_ms);

// Individual test functions
TestResult TEST_GPIO_Inputs(void);
TestResult TEST_GPIO_Outputs(void);
TestResult TEST_ADC_Channels(void);
TestResult TEST_PWM_Outputs(void);
TestResult TEST_TIMER_Configuration(void);
TestResult TEST_UART_Communication(void);
TestResult TEST_SystemClock(void);

// Helper functions
bool TEST_CheckVoltageInRange(uint32_t adc_value, float expected_voltage, float tolerance_percent);
uint32_t TEST_MeasurePulseWidth(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif /* __TEST_H */
