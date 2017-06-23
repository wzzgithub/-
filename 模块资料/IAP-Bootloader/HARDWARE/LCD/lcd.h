#ifndef _LCD_H_
#define _LCD_H_

#include "stdint.h"
#include "stm32f10x_usart.h"

#define LCDSON 1
#define LCDOFF 0

extern uint8_t g_u08_LCDStatus;
extern void LCD_DISPLAY(uint16_t x, uint16_t y, unsigned char * Str);
extern void LCD_DISPLAY1(uint16_t x, uint16_t y, unsigned char * Str);
extern void LCD_DISPLAY2(uint16_t x, uint16_t y, unsigned char * Str);

extern void LCD_DISPLAY_LENGTH(uint16_t x, uint16_t y, uint8_t length, unsigned char * Str);
extern void LCD_DISPLAY1_LENGTH(uint16_t x, uint16_t y, uint8_t length, unsigned char * Str);
extern void LCD_DISPLAY2_LENGTH(uint16_t x, uint16_t y, uint8_t length, unsigned char * Str);

extern void LCD_CLEAR_LENGTH(uint16_t x, uint16_t y, uint8_t length);
extern void LCD_CLEAR_LENGTH1(uint16_t x, uint16_t y, uint8_t length);
extern void LCD_CLEAR_ALL(void);

extern u8 g_u08_DispCnt;
extern u8 g_u08_DisplaySta;

#endif
