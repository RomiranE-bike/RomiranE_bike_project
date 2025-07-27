/*
 * uart_comm.h
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */

#ifndef INC_UART_COMM_H_
#define INC_UART_COMM_H_

#include "motor.h"
#include "stm32f1xx_hal.h"

// Forward declaration
extern UART_HandleTypeDef huart1;

#define UART_BUFFER_SIZE 64

typedef enum {
    CMD_SET_SPEED,
    CMD_STOP,
    CMD_GET_STATUS,
    CMD_SET_PARAM,
    CMD_UNKNOWN
} UART_Command;

typedef struct {
    UART_HandleTypeDef *huart;  // Pointer to UART instance
    uint8_t rx_buffer[UART_BUFFER_SIZE];
    uint16_t rx_index;
    uint8_t cmd_ready;
} UART_Handler;

void UART_Init(UART_HandleTypeDef *huart);
void UART_Process_Commands(Motor_Controller *motor);
void UART_Send_Status(Motor_Controller *motor);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_UART_COMM_H_ */
