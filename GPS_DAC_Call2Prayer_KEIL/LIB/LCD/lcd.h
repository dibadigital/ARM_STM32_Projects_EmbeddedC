
#ifndef _LCD_H
#define _LCD_H

#include "stm32f4xx_hal.h"

#define T  1  

#define DATA_PORT_D4   GPIOB
#define DATA_PORT_D5   GPIOB
#define DATA_PORT_D6   GPIOB
#define DATA_PORT_D7   GPIOB
#define D4_PIN_NUMBER  GPIO_PIN_4
#define D5_PIN_NUMBER  GPIO_PIN_5
#define D6_PIN_NUMBER  GPIO_PIN_6
#define D7_PIN_NUMBER  GPIO_PIN_7

#define CTRL_PORT_RS   GPIOC
#define CTRL_PORT_RW   GPIOC
#define CTRL_PORT_E    GPIOC
#define RS_PIN_NUMBER  GPIO_PIN_0
#define RW_PIN_NUMBER  GPIO_PIN_1
#define E_PIN_NUMBER   GPIO_PIN_2


void lcd_init(void);
void lcd_clear(void);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_putchar(unsigned char data);
void lcd_puts(char *str);
void DelayUS(uint32_t us);

#endif
