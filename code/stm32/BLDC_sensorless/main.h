/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define status_LED_Pin GPIO_PIN_13
#define status_LED_GPIO_Port GPIOC
#define BUTTON_1_Pin GPIO_PIN_14
#define BUTTON_1_GPIO_Port GPIOC
#define BUTTON_2_Pin GPIO_PIN_15
#define BUTTON_2_GPIO_Port GPIOC
#define XTAL_in_Pin GPIO_PIN_0
#define XTAL_in_GPIO_Port GPIOD
#define XTAL_out_Pin GPIO_PIN_1
#define XTAL_out_GPIO_Port GPIOD
#define Shunt_A_Pin GPIO_PIN_0
#define Shunt_A_GPIO_Port GPIOA
#define Shunt_B_Pin GPIO_PIN_1
#define Shunt_B_GPIO_Port GPIOA
#define Shunt_C_Pin GPIO_PIN_2
#define Shunt_C_GPIO_Port GPIOA
#define V_BAT_Pin GPIO_PIN_3
#define V_BAT_GPIO_Port GPIOA
#define V_Speed_Pin GPIO_PIN_4
#define V_Speed_GPIO_Port GPIOA
#define BEMF_A_Pin GPIO_PIN_5
#define BEMF_A_GPIO_Port GPIOA
#define BEMF_B_Pin GPIO_PIN_6
#define BEMF_B_GPIO_Port GPIOA
#define BEMF_C_Pin GPIO_PIN_7
#define BEMF_C_GPIO_Port GPIOA
#define BEMF_N_Pin GPIO_PIN_0
#define BEMF_N_GPIO_Port GPIOB
#define MOTOR_TEMP_Pin GPIO_PIN_1
#define MOTOR_TEMP_GPIO_Port GPIOB
#define LCD_RS_Pin GPIO_PIN_2
#define LCD_RS_GPIO_Port GPIOB
#define LCD_EN_Pin GPIO_PIN_10
#define LCD_EN_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_11
#define LCD_D4_GPIO_Port GPIOB
#define FAULT_IN_Pin GPIO_PIN_12
#define FAULT_IN_GPIO_Port GPIOB
#define PWM_AN_Pin GPIO_PIN_13
#define PWM_AN_GPIO_Port GPIOB
#define PWM_BN_Pin GPIO_PIN_14
#define PWM_BN_GPIO_Port GPIOB
#define PWM_CN_Pin GPIO_PIN_15
#define PWM_CN_GPIO_Port GPIOB
#define PWM_A_Pin GPIO_PIN_8
#define PWM_A_GPIO_Port GPIOA
#define PWM_B_Pin GPIO_PIN_9
#define PWM_B_GPIO_Port GPIOA
#define PWM_C_Pin GPIO_PIN_10
#define PWM_C_GPIO_Port GPIOA
#define SWDIO_STLINK_Pin GPIO_PIN_13
#define SWDIO_STLINK_GPIO_Port GPIOA
#define SWCLK_STLINK_Pin GPIO_PIN_14
#define SWCLK_STLINK_GPIO_Port GPIOA
#define LCD_D5_Pin GPIO_PIN_3
#define LCD_D5_GPIO_Port GPIOB
#define LCD_D6_Pin GPIO_PIN_4
#define LCD_D6_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_5
#define LCD_D7_GPIO_Port GPIOB
#define Data_TX_Pin GPIO_PIN_6
#define Data_TX_GPIO_Port GPIOB
#define Data_RX_Pin GPIO_PIN_7
#define Data_RX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
