
#ifndef _GPS_RingBuff_H
#define _GPS_RingBuff_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "usart.h"

#include "DS3231/ds3231.h"
#include "PrayTimes/PrayTimes.h"

#define rxBuf_Len   400
#define HEAD_TYPE_MAX 1

extern uint32_t gps_lastTime;

typedef enum {
	GPS_State_WaitForStart = 0,
	GPS_State_Collecting
}GPS_State_Type;

extern GPS_State_Type gpsState;

typedef struct {
  const char *Str;
	uint8_t    Len;

}GPS_String_t;

extern GPS_String_t Head_Strings[HEAD_TYPE_MAX];

typedef enum {
	GPS_Head_GPRMC = 0,
	GPS_Head_NULL
}GPS_Head_Type;

typedef struct {
	uint8_t Hour;
	uint8_t Minute;
	float Second;
	float Offset; //second
}GPS_Time_t;

typedef struct {
	int Degree;
	float Minute;
}GPS_Latitude_t;

typedef struct {
	int Degree;
	float Minute;
}GPS_Longitude_t;

typedef struct {
	GPS_Latitude_t gpsLatitude;
	char gpsNS;
	GPS_Longitude_t gpsLongitude;
	char gpsEW;
}GPS_Location_t;

typedef struct {
	int Day;
	int Month;
	int Year;
}GPS_Date_t;

typedef struct {
	GPS_Time_t gpsTime;
	uint8_t gpsStatus;
	GPS_Location_t gpsLocation;
	GPS_Date_t gpsDate;
}GPS_GPRMC_Struct_t;

extern GPS_GPRMC_Struct_t gpsGPRMC;

extern uint8_t rxBuf[rxBuf_Len];
extern uint16_t rdIdx;

extern uint8_t AnalBuffer[100];
extern uint16_t anal_idx;

extern uint8_t FLAG_LOAD_GPS;

uint16_t GPS_GetAvailable(void);
__inline void GPS_Mov_RdIdx(uint16_t offset);
void GPS_ReadData(uint8_t *dataOut,uint16_t dLen);
uint8_t GPS_GetFieldLen(uint8_t *fPtr);
uint8_t* GPS_GetField(uint8_t fieldNum);
GPS_Head_Type GPS_GetHeadType(char *fptr);
void GPS_DoAnalyze(void);
uint8_t GPS_Checksum(void);
void GPS_CheckBytes(void);

#endif
