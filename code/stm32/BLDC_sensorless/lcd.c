/*
 * lcd.c
 *
 *  Created on: Jul 19, 2025
 *      Author: user_1404
 */

#include "lcd.h"
#include <stdarg.h>
#include <stdio.h>

/*
void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void DWT_Delay_us(volatile uint32_t microseconds)
{
    uint32_t clk_cycle_start = DWT->CYCCNT;
    uint32_t cycles_to_wait = microseconds * (HAL_RCC_GetHCLKFreq() / 1000000);
    while ((DWT->CYCCNT - clk_cycle_start) < cycles_to_wait);
}
*/
static void LCD_Delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    us *= (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < us);
}

static void LCD_WriteNibble(uint8_t nibble) {
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (nibble >> 0) & 1);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (nibble >> 1) & 1);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (nibble >> 2) & 1);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (nibble >> 3) & 1);

    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_Delay_us(1);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_Delay_us(40);
}

static void LCD_Write(uint8_t data, uint8_t rs) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, rs ? GPIO_PIN_SET : GPIO_PIN_RESET);
    LCD_WriteNibble(data >> 4);
    LCD_WriteNibble(data & 0x0F);
}

void LCD_Init(void) {
    // Enable DWT Cycle Counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    HAL_Delay(50);
    LCD_Write(0x33, 0); HAL_Delay(5);
    LCD_Write(0x32, 0); HAL_Delay(5);
    LCD_Write(0x28, 0); // 4-bit, 2-line, 5x8 font
    LCD_Write(0x0C, 0); // Display ON, Cursor OFF
    LCD_Write(0x06, 0); // Entry mode
    LCD_Write(0x01, 0); // Clear display
    HAL_Delay(2);
}

void LCD_Clear(void) {
    LCD_Write(0x01, 0);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    static const uint8_t addr[] = {0x00, 0x40};
    LCD_Write(0x80 | (addr[row] + col), 0);
}

void LCD_Print(char *str) {
    while (*str) {
        LCD_Write(*str++, 1);
    }
}

void LCD_Printf(uint8_t row, uint8_t col, const char *fmt, ...) {
    char buffer[17];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    LCD_SetCursor(row, col);
    LCD_Print(buffer);
}
