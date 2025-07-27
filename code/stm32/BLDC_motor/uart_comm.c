/*
 * uart_comm.c
 *
 *  Created on: Jul 13, 2025
 *      Author: user_1404
 */


#include "uart_comm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

UART_Handler uart;

void UART_Init(UART_HandleTypeDef *huart) {
    memset(&uart, 0, sizeof(UART_Handler));
    uart.huart = huart;
    HAL_UART_Receive_IT(uart.huart, &uart.rx_buffer[uart.rx_index], 1);
}

void UART_Process_Commands(Motor_Controller *motor) {
    if(!uart.cmd_ready) return;

    // Null-terminate the received string
    uart.rx_buffer[uart.rx_index] = '\0';

    // Parse command
    char *token = strtok((char*)uart.rx_buffer, " \r\n");
    UART_Command cmd = CMD_UNKNOWN;

    if(strcmp(token, "SPEED") == 0) cmd = CMD_SET_SPEED;
    else if(strcmp(token, "STOP") == 0) cmd = CMD_STOP;
    else if(strcmp(token, "STATUS") == 0) cmd = CMD_GET_STATUS;
    else if(strcmp(token, "SET") == 0) cmd = CMD_SET_PARAM;

    // Process command
    switch(cmd) {
        case CMD_SET_SPEED:
            token = strtok(NULL, " \r\n");
            if(token != NULL) {
                float speed = atof(token);
                Motor_Set_Speed(motor, speed);
            }
            break;

        case CMD_STOP:
            Motor_Stop(motor);
            break;

        case CMD_GET_STATUS:
            UART_Send_Status(motor);
            break;

        case CMD_SET_PARAM: {
            char *param = strtok(NULL, " \r\n");
            char *value_str = strtok(NULL, " \r\n");
            if(param && value_str) {
                float value = atof(value_str);
                if(strcmp(param, "KP_SPEED") == 0) motor->speed_pid.Kp = value;
                else if(strcmp(param, "KI_SPEED") == 0) motor->speed_pid.Ki = value;
            }
            break;
        }

        default:
            break;
    }

    // Reset for next command
    uart.rx_index = 0;
    uart.cmd_ready = 0;
    memset(uart.rx_buffer, 0, UART_BUFFER_SIZE);
    HAL_UART_Receive_IT(uart.huart, &uart.rx_buffer[uart.rx_index], 1);
}

void UART_Send_Status(Motor_Controller *motor) {
    char status_msg[UART_BUFFER_SIZE];
    snprintf(status_msg, UART_BUFFER_SIZE,
             "State:%d Speed:%.2f Angle:%.2f Iq:%.2f\r\n",
             motor->state,
             motor->observer.speed,
             motor->observer.angle,
             motor->current_q_pid.prev_error);

    HAL_UART_Transmit(uart.huart, (uint8_t*)status_msg, strlen(status_msg), HAL_MAX_DELAY);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if(huart->Instance == uart.huart->Instance) {
        if(uart.rx_buffer[uart.rx_index] == '\r' ||
           uart.rx_buffer[uart.rx_index] == '\n') {
            uart.cmd_ready = 1;
        } else {
            uart.rx_index++;
            if(uart.rx_index >= UART_BUFFER_SIZE-1) {
                uart.rx_index = 0;
            }
        }
        HAL_UART_Receive_IT(uart.huart, &uart.rx_buffer[uart.rx_index], 1);
    }
}
