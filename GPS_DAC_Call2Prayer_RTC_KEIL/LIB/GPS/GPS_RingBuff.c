
#include "GPS_RingBuff.h"

uint32_t gps_lastTime;

GPS_State_Type gpsState = GPS_State_WaitForStart;

GPS_String_t Head_Strings[HEAD_TYPE_MAX] = 
{
	{"GPRMC",5}
};

GPS_GPRMC_Struct_t gpsGPRMC;

uint8_t rxBuf[rxBuf_Len];
uint16_t rdIdx = 0;

uint8_t AnalBuffer[100];
uint16_t anal_idx = 0;

uint8_t FLAG_LOAD_GPS = 0;
// ----------------
uint16_t GPS_GetAvailable(void)
{
	uint16_t wrIdx = rxBuf_Len - huart1.hdmarx->Instance->NDTR;
	if(wrIdx >= rdIdx)
	{
		return wrIdx - rdIdx;
	}
	else
	{
		return ((rxBuf_Len - rdIdx) + wrIdx);
	}
}

__inline void GPS_Mov_RdIdx(uint16_t offset)
{
	rdIdx += offset;
	if(rdIdx >= rxBuf_Len)
	{
		rdIdx -= rxBuf_Len;
	}
}

void GPS_ReadData(uint8_t *dataOut, uint16_t dLen)
{
	uint16_t count;
	
	for(count = 0; count < dLen; count++)
	{
		 dataOut[count] = rxBuf[rdIdx];
		 GPS_Mov_RdIdx(1);		
	}	
}

uint8_t GPS_GetFieldLen(uint8_t *fPtr)
{
	uint8_t count = 0;
	while(1)
	{
		if(*fPtr==',' || *fPtr=='\r' ||*fPtr=='*' || *fPtr=='\n')
		{
			break;
		}
		else
		{
			fPtr++;
			count++;
		}
	}
	return count;
}

uint8_t* GPS_GetField(uint8_t fieldNum)
{
	uint8_t count;
	uint8_t *fPtr = 0;
	uint8_t *srcPtr = AnalBuffer;
	if(fieldNum == 0)
	{
		return &AnalBuffer[0];
	}
	else
	{
		for(count = 0; count < fieldNum; count++)
		{		
			fPtr = memchr(srcPtr, ',', anal_idx);
			fPtr++;
			srcPtr = fPtr;			
		}
  }
	return fPtr;
}

GPS_Head_Type GPS_GetHeadType(char *fptr)
{
	uint8_t count;
	for(count = 0; count < HEAD_TYPE_MAX; count++)
	{
		if(strncmp(fptr, Head_Strings[count].Str, Head_Strings[count].Len) == 0)
		{
			return (GPS_Head_Type)count;
		}
	}
	return GPS_Head_NULL;
}

void UTC2Local(int UTC_Hour, int UTC_Minute, float UTC_Second)
{
	int degree_longitude = 0;
	float minute_longitude = 0.0, second_longitude = 0.0;	
	float second_difference = 0.0;	
	int Local_Hour = 0, Local_Minute = 0;
	float Local_Second = 0.0;	
	
	degree_longitude = gpsGPRMC.gpsLocation.gpsLongitude.Degree;
	minute_longitude = gpsGPRMC.gpsLocation.gpsLongitude.Minute;	
	
	second_longitude = (degree_longitude * 3600.0 + minute_longitude * 60.0);
	second_difference = second_longitude / 15.0;
	gpsGPRMC.gpsTime.Offset = second_difference;
	
	UTC_Second = UTC_Hour * 3600.0 + UTC_Minute * 60.0 + UTC_Second;
	if(gpsGPRMC.gpsLocation.gpsEW == 'E') //Local = UTC + Diff
	{
		Local_Second = UTC_Second + second_difference;
	}
	else if(gpsGPRMC.gpsLocation.gpsEW == 'W') //Local = UTC - Diff
	{
		Local_Second = UTC_Second - second_difference; 
	}
	Local_Hour = (int)(Local_Second / 3600.0);
	Local_Second -= Local_Hour * 3600.0;
	Local_Minute = (int)(Local_Second / 60.0);
	Local_Second -= Local_Minute * 60.0; 
	
	gpsGPRMC.gpsTime.Hour = Local_Hour;
	gpsGPRMC.gpsTime.Minute = Local_Minute;
	gpsGPRMC.gpsTime.Second = Local_Second;
}

void GPS_GetTime(char *fptr)
{
	int UTC_Hour = 0, UTC_Minute = 0;
	float UTC_Second = 0.0;
	
	UTC_Hour 	 = 10*(fptr[0]-'0') + (fptr[1]-'0');
	UTC_Minute = 10*(fptr[2]-'0') + (fptr[3]-'0');
	UTC_Second = 10*(fptr[4]-'0') + (fptr[5]-'0') + 0.1*(fptr[7]-'0') + 0.01*(fptr[8]-'0') + 0.001*(fptr[9]-'0');
	
	//15 degrees of longitude = 1 hour difference; 
	UTC2Local(UTC_Hour, UTC_Minute, UTC_Second);
}

void GPS_GetLatitude(char *fptr)
{
	gpsGPRMC.gpsLocation.gpsLatitude.Degree = 10*(fptr[0]-'0') + (fptr[1]-'0');
	gpsGPRMC.gpsLocation.gpsLatitude.Minute = 10*(fptr[2]-'0') + (fptr[3]-'0') + 0.1*(fptr[5]-'0') + 0.01*(fptr[6]-'0') + 0.001*(fptr[7]-'0') + 0.0001*(fptr[8]-'0');
}

void GPS_GetNS(char *fptr)
{
	gpsGPRMC.gpsLocation.gpsNS = fptr[0];
}

void GPS_GetLongitude(char *fptr)
{	
	gpsGPRMC.gpsLocation.gpsLongitude.Degree = 100*(fptr[0]-'0') + 10*(fptr[1]-'0') + (fptr[2]-'0');
	gpsGPRMC.gpsLocation.gpsLongitude.Minute = 10*(fptr[3]-'0') + (fptr[4]-'0') + 0.1*(fptr[6]-'0') + 0.01*(fptr[7]-'0') + 0.001*(fptr[8]-'0') + 0.0001*(fptr[9]-'0'); 
}

void GPS_GetEW(char *fptr)
{
	gpsGPRMC.gpsLocation.gpsEW = fptr[0];
}

void GPS_GetDate(char *fptr)
{
	gpsGPRMC.gpsDate.Day   = 10*(fptr[0]-'0') + (fptr[1]-'0');
	gpsGPRMC.gpsDate.Month = 10*(fptr[2]-'0') + (fptr[3]-'0');
	gpsGPRMC.gpsDate.Year  = 10*(fptr[4]-'0') + (fptr[5]-'0');
}

void GPS_DoAnalyze(void)
{
	uint8_t *filedName = GPS_GetField(0);
	uint8_t  fieldLen = GPS_GetFieldLen(filedName);
	GPS_Head_Type headInfo = GPS_GetHeadType((char*)filedName);
	switch(headInfo)
	{
		case GPS_Head_GPRMC:
		{
			//HAL_UART_Transmit(&huart1, (uint8_t *)"GPRMC RX...", 10, 100);
			//Blink a LED
			uint8_t *filedName = GPS_GetField(3);		// Latitude
			uint8_t  fieldLen = GPS_GetFieldLen(filedName);
			GPS_GetLatitude((char*)filedName);
			
			filedName = GPS_GetField(4); 						// N/S Indicator
			fieldLen = GPS_GetFieldLen(filedName);
			GPS_GetNS((char*)filedName);
			
			filedName = GPS_GetField(5); 						// Longitude
			fieldLen = GPS_GetFieldLen(filedName);
			GPS_GetLongitude((char*)filedName);
			
			filedName = GPS_GetField(6); 						// E/W Indicator
			fieldLen = GPS_GetFieldLen(filedName);
			GPS_GetEW((char*)filedName);
			
			filedName = GPS_GetField(1); 						// Local GPS Clock
			fieldLen = GPS_GetFieldLen(filedName);
			GPS_GetTime((char*)filedName);
			
			filedName = GPS_GetField(9); 						// GPS Date
			fieldLen = GPS_GetFieldLen(filedName);
			GPS_GetDate((char*)filedName);
			
//			DS3231_SetTime(gpsGPRMC.gpsTime.Second, gpsGPRMC.gpsTime.Minute, gpsGPRMC.gpsTime.Hour, 
//											1, gpsGPRMC.gpsDate.Day, gpsGPRMC.gpsDate.Month, gpsGPRMC.gpsDate.Year); //dow = 1
												
			double lati = gpsGPRMC.gpsLocation.gpsLatitude.Degree + gpsGPRMC.gpsLocation.gpsLatitude.Minute / 60.0;
			double longi = gpsGPRMC.gpsLocation.gpsLongitude.Degree + gpsGPRMC.gpsLocation.gpsLongitude.Minute / 60.0;
			double hour_praytimes[TimesCount] = {0, 0, 0, 0, 0, 0, 0}; //Hour
	
			get_prayer_times(gpsGPRMC.gpsDate.Year, gpsGPRMC.gpsDate.Month, gpsGPRMC.gpsDate.Day, 
										lati, longi, gpsGPRMC.gpsTime.Offset/3600.0, hour_praytimes);
			
			for(uint8_t i = 0; i < TimesCount; i++)
			{
				praytimes[i].Hour = (int)hour_praytimes[i];
				float temp = (hour_praytimes[i]-(int)hour_praytimes[i])*60.0;
				praytimes[i].Minute = (int)temp;
				temp = (temp-(int)temp)*60.0;
				praytimes[i].Second = (int)temp;
			}
			
			FLAG_LOAD_GPS = 1;
			
		}break;
		case GPS_Head_NULL:
		{
			
		}break;
	}
}

uint8_t GPS_Checksum(void)
{
	uint8_t count = 0;
	uint8_t total = 0;
	uint32_t packTotal = 0;
	for(count = 0; count < anal_idx - 4; count++)
	{
		total ^= AnalBuffer[count];
	}
	sscanf((char *)&AnalBuffer[anal_idx-3], "%2x", &packTotal);
	if(total == packTotal)
		return 1;
	else
		return 0;
}

void GPS_CheckBytes(void)
{
	while(GPS_GetAvailable())
	{
		switch(gpsState)
		{
			case GPS_State_WaitForStart:
			{
				GPS_ReadData(AnalBuffer, 1);
				if(AnalBuffer[0] == '$')
				{
					/* Head Detected */
					gpsState = GPS_State_Collecting;
					anal_idx = 0;
				}
			}break;
			case GPS_State_Collecting:
			{
				GPS_ReadData(&AnalBuffer[anal_idx], 1);
				if(AnalBuffer[anal_idx] == '\r')
				{
					anal_idx++;
					if(GPS_Checksum())
						GPS_DoAnalyze();
					gpsState = GPS_State_WaitForStart;
				}
				else
				{
					anal_idx++;
					if(anal_idx == 100)
					{
						/* Input Fault */
						gpsState = GPS_State_WaitForStart;
					}
				}
			}break;
		}
	}
}
