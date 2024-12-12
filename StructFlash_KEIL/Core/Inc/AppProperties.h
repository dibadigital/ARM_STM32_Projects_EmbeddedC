#ifndef AppProperties_H_
#define AppProperties_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include <string.h>

#define APP_FLASH_BASE_ADDRESS 		0x080E0000
#define APP_SIGN_VALUE						0x3344FF00

#define APP_DEFAULT_TIME					20
#define APP_DEFAULT_RELAYNUMBER		3
#define APP_DEFAULT_THR						23.7F
#define APP_DEFAULT_PERCENTVALUE	10.0F

typedef struct
{
	uint32_t Sign;
	uint8_t  Time;
	uint8_t  RelayNumber;
	float    THr;
	float    PercentValue;
}SystemConfig_t;

extern SystemConfig_t StartupConfig;

void AppProperties_Init(void);
void SaveConfig(void);
void LoadConfig(void); 

#endif
