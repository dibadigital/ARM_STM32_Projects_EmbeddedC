
#include "DAC_LIB.h"

uint32_t Index_Max_Alarm = 0;
uint8_t idx_music = 0;

uint8_t FLAG_ALARM = 0;

void Music_DAC_DMA(DAC_HandleTypeDef* hdac, uint16_t Index_Max, const uint16_t *Music, uint32_t Music_LEN)
{
	uint32_t len = 0;
	idx_music++;
	if(idx_music < Index_Max)
	{
		len = 65535;
		HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, (uint32_t *)&Music[idx_music*65536], len, DAC_ALIGN_12B_R);
	}
	else
	{
		if(idx_music*65536 < Music_LEN - 1)
		{
			len = Music_LEN - idx_music*65536;
			HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, (uint32_t *)&Music[idx_music*65536], len, DAC_ALIGN_12B_R);
		}
		else
		{
			FLAG_ALARM = 0;
		}
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	Music_DAC_DMA(hdac, Index_Max_Alarm, alarm, alarm_LEN);
}
