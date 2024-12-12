
#include "gsm.h"


// Ring Buffer for Collecting
ringbuff_t rxBuf;

// Linear Buffer for Analyzing
linearbuff_t analBuf;

GSM_State_Type gsmState;

rxBuf_State_Type rxBufState = rxBuf_State_0;

char GSM_User_PhoneList[CountUsers][13] = {"+98XXXXXXXXXX"}; //Write Phone Numbers of Users
uint8_t idx_user = 0;

uint8_t Number_RX[13] = {'\0'}, Date_RX[8] = {'\0'}, Clock_RX[8] = {'\0'}, Message_RX[10] = {'\0'};

uint8_t User_TX[6] = {'\0'};;
uint8_t Message_TX[50] = {'\0'};;

uint8_t CtrlZData[]={26};

uint8_t FLAG_SEND_SMS = 0;
uint16_t cnt_send_sms = 0;

uint8_t FLAG_CALL_ALARM = 0;
uint16_t cnt_call_alarm = 0;

uint8_t FLAG_STATUS_ALARM = 0;
uint8_t FLAG_PIR_SENSOR = 0;
uint8_t FLAG_ALARM_MENU = 0;

uint8_t HourAlarm = 0;
uint8_t MinAlarm = 0;
uint8_t SecAlarm = 0;

/*------------------------ Functions -------------------------*/

/*------------------------Send Command To SIM800-------------------------*/
HAL_StatusTypeDef send_AT_command(char *command)
{
	return HAL_UART_Transmit(&huart1, (uint8_t *)command, strlen(command), 1000);
}

uint8_t GSM_GetFieldLen(uint8_t *fPtr)
{
	uint8_t count = 0;
	while(1)
	{
		if(*fPtr==',' || *fPtr=='\r' || *fPtr=='\n')
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

uint8_t* GSM_GetField(uint8_t fieldNum)
{
	uint8_t count;
	uint8_t *fPtr = 0;
	uint8_t *srcPtr = analBuf.Buffer;
	if(fieldNum == 0)
	{
		return &analBuf.Buffer[0];
	}
	else
	{
		for(count = 0; count < fieldNum; count++)
		{		
			fPtr = memchr(srcPtr, ',', analBuf.anal_idx);
			fPtr++;
			srcPtr = fPtr;			
		}
  }
	return fPtr;
}

void Clear_analBuf(void)
{
	for(uint8_t i = 0; i < 100; i++)
		analBuf.Buffer[i] = '\n';
	analBuf.anal_idx = 0;
}

int8_t CheckNumber(void)
{
	uint8_t s;
	for(uint8_t i = 0; i < CountUsers; i++)
	{
		s = 0;
		for(uint8_t j = 0; j < 13; j++)
		{
			if(Number_RX[j] == GSM_User_PhoneList[i][j])
			{
				s++;
				if(s == 13)
					return i;
			}
			else
			{
				break;
			}
		}
	}
	return -1;
}

void CheckUser(void)
{
	switch(CheckNumber())
	{
		case 0:
			strncpy((char *) User_TX, (char *) "User 1", 6);
			break;
		case 1:
			strncpy((char *) User_TX, (char *) "User 2", 6);
			break;
		case 2:
			strncpy((char *) User_TX, (char *) "User 3", 6);
			break;
		case 3:
			strncpy((char *) User_TX, (char *) "User 4", 6);
			break;
		case 4:
			strncpy((char *) User_TX, (char *) "User 5", 6);
			break;
	}
}

void Clear_Message_TX(void)
{
	for(uint8_t i = 0; i < 50; i++)
		Message_TX[i] = '\0';
}
	
void SendSMS_ALL_Users(void)
{
	if(FLAG_SEND_SMS == 1)
	{
		FLAG_SEND_SMS = 0;
		char CMD[100] = {'\0'};
		strncat(CMD, "AT+CMGS=\"", strlen("AT+CMGS=\""));
		strncat(CMD, GSM_User_PhoneList[idx_user], 13);
		strncat(CMD, "\"\r\n", strlen("\"\r\n"));
		send_AT_command(CMD);
		HAL_Delay(300);
		send_AT_command((char *)Message_TX);
		HAL_UART_Transmit(&huart1, CtrlZData, 1, 1000);
		//HAL_Delay(3000);
		idx_user++;
		if(idx_user == CountUsers)
		{
			idx_user = 0;
			gsmState = GSM_State_8; //GSM_State_12;
		}
	}
}

void Call_ALL_Users(void)
{
	if(FLAG_CALL_ALARM == 1)
	{
		FLAG_CALL_ALARM = 0;
		char CMD[100] = {'\0'};
		
		send_AT_command("ATH\r\n");
		HAL_Delay(300);
		
		strncat(CMD, "ATD", strlen("ATD"));
		strncat(CMD, GSM_User_PhoneList[idx_user], 13);
		strncat(CMD, ";\r\n", strlen(";\r\n"));
		send_AT_command(CMD);
//		HAL_Delay(300);
		idx_user++;
		if(idx_user == CountUsers)
		{
			idx_user = 0;
			gsmState = GSM_State_8;
		}
	}
}

void GSM_CheckState(void)
{	
	switch(gsmState)
	{
		case GSM_State_0: //send "at" command
		{
			send_AT_command("at\r\n");
			gsmState = GSM_State_1;
		}break;
		
		case GSM_State_1: //wait for ack "at" command
		{
			if(FLAG_RECEIVE == 1)
			{
				FLAG_RECEIVE = 0;
				if(strncmp((char *)analBuf.Buffer, "OK", 2) == 0)
					gsmState = GSM_State_2;
				else
					gsmState = GSM_State_0;
				Clear_analBuf();
			}
		}break;
		
		case GSM_State_2: //send "ATE0" command: turn off Echo
		{
			send_AT_command("ATE0\r\n");
			gsmState = GSM_State_3;
		}break;
		
		case GSM_State_3: //wait for ack "ATE0" command
		{
			if(FLAG_RECEIVE == 1)
			{
				FLAG_RECEIVE = 0;
				if(strncmp((char *)analBuf.Buffer, "OK", 2) == 0)
						gsmState = GSM_State_4;
				else
						gsmState = GSM_State_2;
				Clear_analBuf();
			}
		}break;
		
		case GSM_State_4: //send "CMGF" command: send in text mode
		{
			send_AT_command("AT+CMGF=1\r\n");
			gsmState = GSM_State_5;
		}break;
		
		case GSM_State_5: //wait for ack "CMGF" command
		{
			if(FLAG_RECEIVE == 1)
			{
				FLAG_RECEIVE = 0;
				if(strncmp((char *)analBuf.Buffer, "OK", 2) == 0)
						gsmState = GSM_State_6;
				else
						gsmState = GSM_State_4;
				Clear_analBuf();
			}
		}break;
		
		case GSM_State_6: //send "CNMI" command
		{
			send_AT_command("AT+CNMI=2,2,0,1,0\r\n");
			gsmState = GSM_State_7;
		}break;
		
		case GSM_State_7: //wait for ack "CNMI" command
		{
			if(FLAG_RECEIVE == 1)
			{
				FLAG_RECEIVE = 0;
				if(strncmp((char *)analBuf.Buffer, "OK", 2) == 0)
						gsmState = GSM_State_8;						
				else
						gsmState = GSM_State_6;
				Clear_analBuf();
			}
		}break;
		
		case GSM_State_8: //ready for recieve "+CMT" event
		{
			if(FLAG_RECEIVE == 1)
			{
				FLAG_RECEIVE = 0;
				if(strncmp((char *)analBuf.Buffer, "+CMT", 4) == 0)
				{
					uint8_t *filedName;
					
					filedName = GSM_GetField(0); // +CMT: "98XXXXXXXXXX"
					strncpy((char *) Number_RX, (char *) &filedName[7], 13);
					CheckUser();
					
					filedName = GSM_GetField(2); // "24/04/28
					strncpy((char *) Date_RX, (char *) &filedName[1], 8);
					
					filedName = GSM_GetField(3); // 20:16:39+18"
					strncpy((char *) Clock_RX, (char *) &filedName[0], 8);
					
					gsmState = GSM_State_9;
				}
				Clear_analBuf();
			}
			
			if(FLAG_PIR_SENSOR == 1)
			{
				FLAG_PIR_SENSOR = 0;
				FLAG_STATUS_ALARM = 0;
				
				FLAG_ALARM_MENU = 1;
				EE_WriteVariable (1, FLAG_ALARM_MENU );
				
				DS3231_GetTime();
				HourAlarm = dsTime.Hour;
				MinAlarm = dsTime.Minute;
				SecAlarm = dsTime.Second;
				
				EE_WriteVariable (2, HourAlarm );
				EE_WriteVariable (3, MinAlarm );
				EE_WriteVariable (4, SecAlarm );
				
				idx_user = 0;
				gsmState = GSM_State_11;
			}
		}break;
		
		case GSM_State_9: // Read and Check the Message
		{
			if(FLAG_RECEIVE == 1)
			{
				FLAG_RECEIVE = 0;
				
				uint8_t *filedName;
				uint8_t  fieldLen;
				
				filedName = GSM_GetField(0); // Alarm=ON
				fieldLen = GSM_GetFieldLen(filedName);
				strncpy((char *) Message_RX, (char *) &filedName[0], fieldLen);			
				
				if(strncmp((char *)Message_RX, "Alarm=ON", 8) == 0)
				{
					FLAG_STATUS_ALARM = 1;
					EE_WriteVariable (0, FLAG_STATUS_ALARM );

					Clear_Message_TX();
					strncat((char *) Message_TX, (char *) User_TX, 6);
					strncat((char *) Message_TX, " Enabled the Alarm.\n", strlen(" Enabled the Alarm.\n"));
					strncat((char *) Message_TX, (char *) Date_RX, 8);
					strncat((char *) Message_TX, ", ", strlen(", "));
					strncat((char *) Message_TX, (char *) Clock_RX, 8);
					idx_user = 0;
					gsmState = GSM_State_10;
				}
				else if(strncmp((char *)Message_RX, "Alarm=OFF", 9) == 0)
				{
					FLAG_STATUS_ALARM = 0; 
					EE_WriteVariable (0, FLAG_STATUS_ALARM );
					
					FLAG_ALARM_MENU = 0;
					EE_WriteVariable (1, FLAG_ALARM_MENU );
					lcd_clear();
					
					HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
					Clear_Message_TX();
					strncat((char *) Message_TX, (char *) User_TX, 6);
					strncat((char *) Message_TX, " Disabled the Alarm.\n", strlen(" Disabled the Alarm.\n"));
					strncat((char *) Message_TX, (char *) Date_RX, 8);
					strncat((char *) Message_TX, ", ", strlen(", "));
					strncat((char *) Message_TX, (char *) Clock_RX, 8);
					idx_user = 0;
					gsmState = GSM_State_10;
				}
			}
		}break;
		
		case GSM_State_10:  // Alarm=ON or Alarm=OFF //send "CMGS" command: send a message to all users
		{
			SendSMS_ALL_Users();
		}break;
		
		case GSM_State_11:  // call to all users
		{
			Call_ALL_Users();
		}break;
	}
}
	
void GSM_CheckBytes(void)
{
	uint8_t data;
	
	while(RingBuff_GetAvaiableRead(&rxBuf, 1) == 1 && FLAG_RECEIVE == 0) //Continue RD index to WR index
	{
		RingBuff_Read(&rxBuf, &data, 1);
		switch(rxBufState)
		{
			case rxBuf_State_0: //montazere '\r' aval
				{
					if(data == '\r')
					{
						rxBufState = rxBuf_State_1;
					}
				}break;
			case rxBuf_State_1: //montazere '\n' aval
				{
					if(data == '\n')
						rxBufState = rxBuf_State_2;
					else if(data == '\r')
						rxBufState = rxBuf_State_1;
					else
						rxBufState = rxBuf_State_0;
				}break;
			case rxBuf_State_2: //montazere '\r' dovom
				{
					if(data == '\r')
						rxBufState = rxBuf_State_3;
					else if(data == '\n')
						rxBufState = rxBuf_State_0;
					else
					{
						analBuf.Buffer[analBuf.anal_idx] = data;
						analBuf.anal_idx++;
						if(analBuf.anal_idx == 100)
						{
							/* Input Fault */
							Clear_analBuf();
							rxBufState = rxBuf_State_0;
						}
					}
				}break;
			case rxBuf_State_3: //montazere '\n' dovom
				{
					if(data == '\n')
					{
						if (analBuf.Buffer[0] != '\n')
						{
							// find out
							FLAG_RECEIVE = 1;
						}
						rxBufState = rxBuf_State_2;   //rxBuf_State_0
					}	
					else if(data == '\r')
						rxBufState = rxBuf_State_1;
					else
						rxBufState = rxBuf_State_0;
				}break;
		}
	}
}
