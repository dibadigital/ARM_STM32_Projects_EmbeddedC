
#include "ds3231.h"

DS3231_Time_t dsTime;

// Convert normal decimal numbers to binary coded decimal
uint8_t DEC2BCD(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
int BCD2DEC(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

void DS3231_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year)
{
  uint8_t set_time[7];
  
  set_time[0] = DEC2BCD(sec);
  set_time[1] = DEC2BCD(min);
  set_time[2] = DEC2BCD(hour);
  set_time[3] = DEC2BCD(dow);
  set_time[4] = DEC2BCD(dom);
  set_time[5] = DEC2BCD(month);
  set_time[6] = DEC2BCD(year);

  HAL_I2C_Mem_Write(&hi2c1, DS3231_ADDRESS, 0x00, 1, set_time, 7, 1000);
}

void DS3231_GetTime(void)
{
  uint8_t get_time[7];
  
  HAL_I2C_Mem_Read(&hi2c1, DS3231_ADDRESS, 0x00, 1, get_time, 7, 1000);
	
  dsTime.Second = BCD2DEC(get_time[0]);
  dsTime.Minute = BCD2DEC(get_time[1]);
  dsTime.Hour   = BCD2DEC(get_time[2]);
  dsTime.DOW    = BCD2DEC(get_time[3]);
  dsTime.Day    = BCD2DEC(get_time[4]);
  dsTime.Month  = BCD2DEC(get_time[5]);
  dsTime.Year   = BCD2DEC(get_time[6]);
}
