#ifndef MyHalFlash_H_
#define MyHalFlash_H_

#include "stm32f4xx_hal.h"

/* Variables' number */
#define NB_OF_VAR             ((uint8_t)20)  //number of variable uint26_t  = 20 

void MyHalFlash_Init(void);

HAL_StatusTypeDef FLASH_EraseSector(uint8_t page_ID,uint8_t volt_RANGE);
HAL_StatusTypeDef FLASH_ProgramHalfWord(uint32_t pAddress,uint16_t data);

#endif

