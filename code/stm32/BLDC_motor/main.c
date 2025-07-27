/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc_handler.h"
#include "motor.h"
#include "uart_comm.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Motor_Controller motor;  // Global motor controller instance
uint8_t system_ready = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // Initialize modules in proper sequence
  ADC_Handler_Init();
  UART_Init(&huart1);
  Motor_Init(&motor);
  Motor_Config_Init(&motor);

  // Calibrate current sensors (motor must be stopped)
  Current_Sensor_Calibrate(&motor, &hadc1);

  // System ready - start PWM
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  // Initial debug output
  const char *init_msg = "System Initialized\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)init_msg, strlen(init_msg), 100);

  system_ready = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t last_control_tick = HAL_GetTick();
  uint32_t last_status_tick = last_control_tick;
  uint32_t last_debug_tick = last_control_tick;

  while (1)
  {
    uint32_t current_tick = HAL_GetTick();
    float Vdc = Get_DC_Voltage(); // Read DC voltage once per loop

    // 1kHz motor control loop
    if(current_tick - last_control_tick >= 1) {
      last_control_tick = current_tick;

      if(system_ready && motor.state != MOTOR_FAULT) {
        float dt = 0.001f; // Fixed 1ms time step

        // Read sensors with error checking
        float Ia = Get_Phase_Current(0);
        float Ib = Get_Phase_Current(1);
        float Ic = Get_Phase_Current(2);

        // Validate sensor readings
        if(isnan(Ia) || isnan(Ib) || isnan(Ic) || isnan(Vdc) ||
           Vdc < MIN_VOLTAGE_THRESHOLD || Vdc > motor.max_voltage) {
          motor.state = MOTOR_FAULT;
          Safety_Handler();
          continue;
        }

        // Run motor control algorithm
        float pwm1, pwm2, pwm3;
        Motor_Update(&motor, Ia, Ib, Ic, Vdc, dt, &pwm1, &pwm2, &pwm3);

        // Apply PWM with limits
        uint32_t pwm_max = htim1.Instance->ARR;
        pwm1 = fmaxf(0.0f, fminf(pwm1, 0.95f)); // Limit to 95% duty cycle
        pwm2 = fmaxf(0.0f, fminf(pwm2, 0.95f));
        pwm3 = fmaxf(0.0f, fminf(pwm3, 0.95f));

        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint32_t)(pwm1 * pwm_max));
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)(pwm2 * pwm_max));
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (uint32_t)(pwm3 * pwm_max));
      }
    }

    // 10Hz status update
    if(current_tick - last_status_tick >= 100) {
      last_status_tick = current_tick;
      System_Status_Update();
    }

    // 1Hz debug output
    if(current_tick - last_debug_tick >= 1000) {
      last_debug_tick = current_tick;
      Debug_Output(Vdc);
    }

    // Handle incoming commands
    UART_Process_Commands(&motor);

    // Minimal delay to prevent CPU overload
    HAL_Delay(1);
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void System_Status_Update(void)
{
  // Toggle status LED to indicate alive
  HAL_GPIO_TogglePin(status_LED_GPIO_Port, status_LED_Pin);

  // Fast blink if fault condition
  if(motor.state == MOTOR_FAULT) {
    HAL_GPIO_TogglePin(status_LED_GPIO_Port, status_LED_Pin);
  }
}

static void Safety_Handler(void)
{
  // Immediately disable all PWM outputs
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);

  // Log fault via UART
  char fault_msg[150];
  snprintf(fault_msg, sizeof(fault_msg),
          "FAULT! System shutdown\r\n"
          "State: %d\r\n"
          "Voltage: %.1fV (Min: %.1fV)\r\n"
          "Current Offsets: %.1f, %.1f, %.1f\r\n",
          motor.state,
          Get_DC_Voltage(),
          MIN_VOLTAGE_THRESHOLD,
          motor.current_offsets[0],
          motor.current_offsets[1],
          motor.current_offsets[2]);
  HAL_UART_Transmit(&huart1, (uint8_t*)fault_msg, strlen(fault_msg), 100);
}

static void Debug_Output(float Vdc)
{
  char dbg_msg[200];
  snprintf(dbg_msg, sizeof(dbg_msg),
          "System Status:\r\n"
          "State: %d | Target: %.1f rad/s | Actual: %.1f rad/s\r\n"
          "Voltage: %.1fV | Currents: %.2f/%.2f/%.2fA\r\n"
          "PWM Duty: %.1f%%, %.1f%%, %.1f%%\r\n"
          "I_d: %.2fA | I_q: %.2fA\r\n"
          "--------------------------------\r\n",
          motor.state,
          motor.target_speed,
          motor.actual_speed,
          Vdc,
          Get_Phase_Current(0),
          Get_Phase_Current(1),
          Get_Phase_Current(2),
          __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_1) * 100.0f / htim1.Instance->ARR,
          __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_2) * 100.0f / htim1.Instance->ARR,
          __HAL_TIM_GET_COMPARE(&htim1, TIM_CHANNEL_3) * 100.0f / htim1.Instance->ARR,
          motor.I_d,
          motor.I_q);
  HAL_UART_Transmit(&huart1, (uint8_t*)dbg_msg, strlen(dbg_msg), 100);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
    // Rapid LED blink for critical errors
    HAL_GPIO_TogglePin(status_LED_GPIO_Port, status_LED_Pin);
    HAL_Delay(100);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  char assert_msg[100];
  snprintf(assert_msg, sizeof(assert_msg), "Assert failed: %s line %lu\r\n", file, line);
  HAL_UART_Transmit(&huart1, (uint8_t*)assert_msg, strlen(assert_msg), 100);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
