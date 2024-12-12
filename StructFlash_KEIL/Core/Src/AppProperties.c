#include "AppProperties.h"

SystemConfig_t StartupConfig;

void AppProperties_Set_Default(void)
{
	StartupConfig.Time 					= APP_DEFAULT_TIME;
	StartupConfig.RelayNumber 	= APP_DEFAULT_RELAYNUMBER;
	StartupConfig.THr 					= APP_DEFAULT_THR;
	StartupConfig.PercentValue 	= APP_DEFAULT_PERCENTVALUE;
	
	StartupConfig.Sign = APP_SIGN_VALUE;
	SaveConfig();
}

void AppProperties_Init(void)
{
	uint8_t count = 0;
	do
	{
		if(HAL_FLASH_Unlock() == HAL_OK)
			break;
		count++;
	}while(count < 3);
	
	LoadConfig();
	if(StartupConfig.Sign != APP_SIGN_VALUE)
	{
		AppProperties_Set_Default();
	}
	else
	{
	 //nothing
	}
}

static void FlashSaveByte(uint8_t *data, uint16_t len)
{
	uint16_t count = 0;
	for(count = 0; count < len; count++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, APP_FLASH_BASE_ADDRESS+count, data[count]);
		FLASH_WaitForLastOperation(100);
	}
}
	
void SaveConfig(void)
{
	uint8_t count = 0;
	uint8_t *ptrConfig = (uint8_t*) &StartupConfig;
	
	FLASH_EraseInitTypeDef erInfo;
	uint32_t SectorError = 0;
	
	erInfo.Banks = FLASH_BANK_1;
	erInfo.NbSectors = 1;
	erInfo.Sector = FLASH_SECTOR_11;
	erInfo.VoltageRange = VOLTAGE_RANGE_3;
	
	do
	{
		if(HAL_FLASHEx_Erase(&erInfo, &SectorError) == HAL_OK)
			break;
		count++;
	}while(count < 3);
	
	FlashSaveByte(ptrConfig, sizeof(SystemConfig_t));
}

void LoadConfig(void)
{
	SystemConfig_t *ptrConfig = (SystemConfig_t*) APP_FLASH_BASE_ADDRESS;
	memcpy(&StartupConfig, ptrConfig, sizeof(SystemConfig_t));
}
