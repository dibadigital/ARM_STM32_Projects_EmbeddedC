
#ifndef _ds3231_H
#define _ds3231_H

#include "stm32f4xx_hal.h"

#define DS3231_ADDRESS 0xD0

extern I2C_HandleTypeDef hi2c1;

typedef struct {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t DOW;  //day of week : 1 to 7 (1 = Sunday)
  uint8_t Day;  //day of month
  uint8_t Month;
  uint8_t Year;
} DS3231_Time_t;

extern DS3231_Time_t dsTime;

uint8_t DEC2BCD(int val);
int BCD2DEC(uint8_t val);
void DS3231_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year);
void DS3231_GetTime(void);

#endif
