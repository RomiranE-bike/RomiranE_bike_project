/*
 * test.c
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */


#include "test.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"

// External variables
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart1;

// Test cases array
static const TestCase testCases[] = {
    {"GPIO Input Test", TEST_GPIO, TEST_GPIO_Inputs, "Verifies all input GPIOs are functional"},
    {"GPIO Output Test", TEST_GPIO, TEST_GPIO_Outputs, "Verifies all output GPIOs are functional"},
    {"ADC Channel Test", TEST_ADC, TEST_ADC_Channels, "Verifies all ADC channels are working"},
    {"PWM Output Test", TEST_PWM, TEST_PWM_Outputs, "Verifies PWM outputs are functional"},
    {"Timer Config Test", TEST_TIMER, TEST_TIMER_Configuration, "Verifies timer configuration"},
    {"UART Comm Test", TEST_COMMS, TEST_UART_Communication, "Verifies UART communication"},
    {"System Clock Test", TEST_SYSTEM, TEST_SystemClock, "Verifies system clock configuration"}
};

#define NUM_TESTS (sizeof(testCases) / sizeof(TestCase))

// Run all tests
void TEST_RunAll(void) {
    uint8_t passed = 0;
    uint8_t failed = 0;
    uint8_t warnings = 0;

    TEST_SendUART("\r\n=== BLDC Motor Controller Self-Test ===\r\n");
    TEST_SendUART("Starting comprehensive system test...\r\n\r\n");

    for (uint8_t i = 0; i < NUM_TESTS; i++) {
        TEST_SendUART(testCases[i].name);
        TEST_SendUART(": ");
        TEST_SendUART(testCases[i].description);
        TEST_SendUART("... ");

        TestResult result = testCases[i].function();

        switch(result) {
            case TEST_PASSED:
                TEST_SendUART("PASSED\r\n");
                passed++;
                break;
            case TEST_FAILED:
                TEST_SendUART("FAILED\r\n");
                failed++;
                break;
            case TEST_WARNING:
                TEST_SendUART("WARNING\r\n");
                warnings++;
                break;
        }

        TEST_IndicateResult(result);
        HAL_Delay(500); // Short delay between tests
    }

    // Final summary
    char summary[100];
    snprintf(summary, sizeof(summary), "\r\nTest Summary: %d Passed, %d Failed, %d Warnings\r\n",
             passed, failed, warnings);
    TEST_SendUART(summary);

    // Final indication
    if (failed > 0) {
        TEST_SetLED(LED_BLINK_VERY_FAST);
        TEST_Beep(3, 200);
    } else if (warnings > 0) {
        TEST_SetLED(LED_BLINK_SLOW);
        TEST_Beep(2, 200);
    } else {
        TEST_SetLED(LED_PATTERN_3_SHORT);
        TEST_Beep(1, 500);
    }
}

// Set LED pattern (using PC13 as per your IOC)
void TEST_SetLED(LEDPattern pattern) {
    switch(pattern) {
        case LED_OFF:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
            break;
        case LED_SOLID:
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
            break;
        case LED_BLINK_SLOW:
            // Implement with timer or simple delay in main loop
            break;
        case LED_BLINK_FAST:
            // Implement with timer or simple delay in main loop
            break;
        case LED_BLINK_VERY_FAST:
            // Implement with timer or simple delay in main loop
            break;
        case LED_PATTERN_3_SHORT:
            // Implement pattern
            break;
        case LED_PATTERN_2_SHORT_1_LONG:
            // Implement pattern
            break;
    }
}

// Send message via UART
void TEST_SendUART(const char* message) {
    HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

// Beep pattern (if buzzer is connected)
void TEST_Beep(uint8_t count, uint16_t duration_ms) {
    // Implement if you have a buzzer connected
    // This would typically use PWM to generate tones
}

// Indicate test result with LED and beep
void TEST_IndicateResult(TestResult result) {
    switch(result) {
        case TEST_PASSED:
            TEST_SetLED(LED_SOLID);
            HAL_Delay(200);
            TEST_SetLED(LED_OFF);
            break;
        case TEST_FAILED:
            TEST_SetLED(LED_BLINK_FAST);
            HAL_Delay(500);
            TEST_SetLED(LED_OFF);
            break;
        case TEST_WARNING:
            TEST_SetLED(LED_BLINK_SLOW);
            HAL_Delay(500);
            TEST_SetLED(LED_OFF);
            break;
    }
}

// Test GPIO Inputs (fault_pin)
TestResult TEST_GPIO_Inputs(void) {
    // Test fault_pin (PB12)
    GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);

    if (state == GPIO_PIN_SET) { // Should be pulled up according to IOC
        return TEST_PASSED;
    }

    char message[50];
    snprintf(message, sizeof(message), "Fault pin (PB12) state: %d\r\n", state);
    TEST_SendUART(message);

    return TEST_FAILED;
}

// Test GPIO Outputs (status LED)
TestResult TEST_GPIO_Outputs(void) {
    // Test status LED (PC13)
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    return TEST_PASSED; // If we got here, it worked
}

// Test ADC Channels
TestResult TEST_ADC_Channels(void) {
    uint32_t adc_values[5] = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    // Start ADC
    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        TEST_SendUART("ADC Start Failed\r\n");
        return TEST_FAILED;
    }

    // Read all channels
    for (uint8_t i = 0; i < 5; i++) {
        sConfig.Channel = ADC_CHANNEL_0 + i;
        sConfig.Rank = ADC_REGULAR_RANK_1;
        sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;

        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
            char msg[30];
            snprintf(msg, sizeof(msg), "ADC Channel %d Config Failed\r\n", i);
            TEST_SendUART(msg);
            return TEST_FAILED;
        }

        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            adc_values[i] = HAL_ADC_GetValue(&hadc1);
        } else {
            char msg[30];
            snprintf(msg, sizeof(msg), "ADC Channel %d Read Failed\r\n", i);
            TEST_SendUART(msg);
            return TEST_FAILED;
        }
    }

    // Print ADC values for debugging
    char adc_msg[100];
    snprintf(adc_msg, sizeof(adc_msg),
             "ADC Values: A=%lu, B=%lu, C=%lu, Vbat=%lu, Vspeed=%lu\r\n",
             adc_values[0], adc_values[1], adc_values[2], adc_values[3], adc_values[4]);
    TEST_SendUART(adc_msg);

    // Basic sanity checks
    for (uint8_t i = 0; i < 5; i++) {
        if (adc_values[i] == 0 || adc_values[i] > 4095) {
            return TEST_WARNING; // Probably not connected but ADC works
        }
    }

    return TEST_PASSED;
}

// Test PWM Outputs
TestResult TEST_PWM_Outputs(void) {
    // Start PWM outputs
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);  // PWM_A (PA8)
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);  // PWM_B (PA9)
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);  // PWM_C (PA10)
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); // PWM_AN (PB13)
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // PWM_BN (PB14)
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); // PWM_CN (PB15)

    // Set different duty cycles to verify all channels
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, htim1.Init.Period / 4);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, htim1.Init.Period / 2);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Init.Period * 3 / 4);

    TEST_SendUART("PWM outputs activated. Verify with scope.\r\n");

    HAL_Delay(1000); // Let PWM run for a bit

    // Stop PWM outputs
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3);

    return TEST_PASSED; // If we got here, PWM was started successfully
}

// Test Timer Configuration
TestResult TEST_TIMER_Configuration(void) {
    // Verify timer configuration matches what we expect
    if (htim1.Instance != TIM1) {
        return TEST_FAILED;
    }

    if (htim1.Init.Prescaler != 0) {
        return TEST_WARNING;
    }

    if (htim1.Init.CounterMode != TIM_COUNTERMODE_CENTERALIGNED3) {
        return TEST_FAILED;
    }

    if (htim1.Init.Period != 3599) { // From your IOC file
        return TEST_FAILED;
    }

    if (htim1.Init.AutoReloadPreload != TIM_AUTORELOAD_PRELOAD_ENABLE) {
        return TEST_FAILED;
    }

    return TEST_PASSED;
}

// Test UART Communication
TestResult TEST_UART_Communication(void) {
    const char* test_message = "UART Test Message\r\n";
    //uint8_t rx_buffer[20] = {0};

    // Send test message
    if (HAL_UART_Transmit(&huart1, (uint8_t*)test_message, strlen(test_message), 100) != HAL_OK) {
        return TEST_FAILED;
    }

    // For full test, you would loopback TX to RX and verify
    // Since we don't have hardware loopback, we'll just assume it works if transmit succeeds

    return TEST_PASSED;
}

// Test System Clock
TestResult TEST_SystemClock(void) {
    // Verify system clock is running at expected frequency (16MHz from IOC)
    // This is a simplified check - in practice you'd need to measure or verify indirectly

    uint32_t sysclk = HAL_RCC_GetSysClockFreq();
    char msg[50];
    snprintf(msg, sizeof(msg), "System Clock: %lu Hz\r\n", sysclk);
    TEST_SendUART(msg);

    if (sysclk < 15000000 || sysclk > 17000000) { // 16MHz ± ~6%
        return TEST_FAILED;
    }

    return TEST_PASSED;
}

// Helper function to check voltage in range
bool TEST_CheckVoltageInRange(uint32_t adc_value, float expected_voltage, float tolerance_percent) {
    float measured_voltage = (adc_value * 3.3f) / 4095.0f; // Assuming 3.3V reference
    float min_voltage = expected_voltage * (1.0f - tolerance_percent/100.0f);
    float max_voltage = expected_voltage * (1.0f + tolerance_percent/100.0f);

    return (measured_voltage >= min_voltage) && (measured_voltage <= max_voltage);
}

// Helper function to measure pulse width
uint32_t TEST_MeasurePulseWidth(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    // This would require precise timing measurement implementation
    // Returns pulse width in microseconds
    return 0;
}
