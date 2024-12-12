#include "MyHalFlash.h"



uint16_t VirtAddVarTab[NB_OF_VAR];


void MyHalFlash_Init()
{
	uint8_t count=0;
	
	do{
		if(HAL_FLASH_Unlock() ==HAL_OK)
			break;
		count++;
	}while(count < 3);
	
	for(count=0;count<NB_OF_VAR;count++)
	{
		VirtAddVarTab[count]=count;
	}
	
	
}


HAL_StatusTypeDef FLASH_EraseSector(uint8_t page_ID,uint8_t volt_RANGE)
{
	HAL_StatusTypeDef outRes;
  FLASH_EraseInitTypeDef infoToErase;
	uint32_t tempError;
	infoToErase.Banks=FLASH_BANK_1;
	infoToErase.NbSectors=1;
	infoToErase.Sector=page_ID;
	infoToErase.TypeErase=FLASH_TYPEERASE_SECTORS;
	infoToErase.VoltageRange=volt_RANGE;
	
	
	
	outRes=HAL_FLASHEx_Erase( &infoToErase, &tempError);

	return outRes;
}

HAL_StatusTypeDef FLASH_ProgramHalfWord(uint32_t pAddress,uint16_t data)
{
	HAL_StatusTypeDef outRes;
	
	
	outRes = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, pAddress, data);

	
	return outRes;
}

