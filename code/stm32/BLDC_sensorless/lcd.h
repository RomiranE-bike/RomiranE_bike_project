// lcd.h - Optimized Hybrid LCD Library (4-bit Mode, No Flicker)
#ifndef LCD_H
#define LCD_H

#include "stm32f1xx_hal.h"

// LCD Pin Definitions


// === LCD Pin Mapping ===
#define LCD_RS_PORT GPIOB
#define LCD_RS_PIN  GPIO_PIN_2

#define LCD_EN_PORT GPIOB
#define LCD_EN_PIN  GPIO_PIN_10

#define LCD_D4_PORT GPIOB
#define LCD_D4_PIN  GPIO_PIN_11

#define LCD_D5_PORT GPIOB
#define LCD_D5_PIN  GPIO_PIN_3

#define LCD_D6_PORT GPIOB
#define LCD_D6_PIN  GPIO_PIN_4

#define LCD_D7_PORT GPIOB
#define LCD_D7_PIN  GPIO_PIN_5

// === Public API ===
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char *str);
void LCD_Printf(uint8_t row, uint8_t col, const char *fmt, ...);
//void DWT_Init(void);
//void DWT_Delay_us(volatile uint32_t microseconds);

#endif
