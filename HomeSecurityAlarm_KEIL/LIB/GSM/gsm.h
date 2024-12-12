
#ifndef _GSM_H
#define _GSM_H

#include "LCD/lcd.h"
#include "DS3231/ds3231.h"
#include "RingBuffer/RingBuffer.h"
#include "usart.h"
#include "dac.h"
#include "Audio/alarm.h"
#include "DAC/DAC_LIB.h"
#include "tim.h"
#include <stdio.h>
#include "EEPROM/eeprom.h"
#include "MyHalFlash/MyHalFlash.h"

#define CountUsers 1

extern ringbuff_t rxBuf;

typedef struct {
	uint8_t Buffer[100];
	uint16_t anal_idx;
}linearbuff_t;
extern linearbuff_t analBuf;

typedef enum {
	GSM_State_0 = 0,
	GSM_State_1,
	GSM_State_2,
	GSM_State_3,
	GSM_State_4,
	GSM_State_5,
	GSM_State_6,
	GSM_State_7,
	GSM_State_8,
	GSM_State_9,
	GSM_State_10,
	GSM_State_11,
	GSM_State_12,
	GSM_State_13
}GSM_State_Type;
extern GSM_State_Type gsmState;

typedef enum {
	rxBuf_State_0 = 0,
	rxBuf_State_1,
	rxBuf_State_2,
	rxBuf_State_3
}rxBuf_State_Type;
extern rxBuf_State_Type rxBufState;

extern char GSM_User_PhoneList[][13];

extern uint8_t Number_RX[13], Date_RX[8], Clock_RX[8], Message_RX[10];

extern uint8_t CtrlZData[];

extern uint8_t FLAG_SEND_SMS;
extern uint16_t cnt_send_sms;

extern uint8_t FLAG_CALL_ALARM;
extern uint16_t cnt_call_alarm;

extern uint8_t FLAG_STATUS_ALARM;
extern uint8_t FLAG_PIR_SENSOR;
extern uint8_t FLAG_ALARM_MENU;

extern uint8_t HourAlarm;
extern uint8_t MinAlarm;
extern uint8_t SecAlarm;

/*------------------------ Functions -------------------------*/

HAL_StatusTypeDef send_AT_command(char *command);
//void GSM_DoAnalyze(void);
void GSM_CheckState(void);
void GSM_CheckBytes(void);

void Call_ALL_Users(void);

#endif
