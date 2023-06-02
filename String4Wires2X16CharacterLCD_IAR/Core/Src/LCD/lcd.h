
#ifndef _LCD_H
#define _LCD_H

#include "stm32f1xx_hal.h"

#define T  1  

#define DATA_PORT_D4   GPIOA
#define DATA_PORT_D5   GPIOA
#define DATA_PORT_D6   GPIOA
#define DATA_PORT_D7   GPIOA
#define D4_PIN_NUMBER  GPIO_PIN_4
#define D5_PIN_NUMBER  GPIO_PIN_5
#define D6_PIN_NUMBER  GPIO_PIN_6
#define D7_PIN_NUMBER  GPIO_PIN_7

#define CTRL_PORT_RS   GPIOB
#define CTRL_PORT_RW   GPIOB
#define CTRL_PORT_E    GPIOB
#define RS_PIN_NUMBER  GPIO_PIN_5
#define RW_PIN_NUMBER  GPIO_PIN_6
#define E_PIN_NUMBER   GPIO_PIN_7


void lcd_init(void);
void lcd_clear(void);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_putchar(unsigned char data);
void lcd_puts(char *str);
void DelayUS(uint32_t us);

#endif
