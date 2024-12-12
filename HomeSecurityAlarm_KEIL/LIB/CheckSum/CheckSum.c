
#include "CheckSum.h"

void CheckSum_Write(uint8_t *data, uint8_t len) //Transmit
{
	uint8_t checksum = 0, carry = 0;
	uint16_t sum = 0;
	
	// calculate sum of data[0...len-2]
	for(int i = 0; i < (len -1); i++)
	{
		sum += data[i];
	}
	
	// calculate checksum of data[0...len-2]
	checksum = (uint8_t) sum;
	carry = (uint8_t) (sum >> 8);
	
	checksum += carry;
	checksum = 255 - checksum;
	
	// write checksum in data[len-1]
	data[len-1] = checksum;
}

uint8_t CheckSum_Read(uint8_t *data, uint8_t len) //Recieve
{
	uint8_t checksum = 0, carry = 0;
	uint16_t sum = 0;
	
	// calculate sum of data[0...len-1]
	for(int i = 0; i < len; i++)
	{
		sum += data[i];
	}
	
	// calculate checksum of data[0...len-1]
	checksum = (uint8_t) sum;
	carry = (uint8_t) (sum >> 8);
	
	checksum += carry;
	
	if(checksum == 255)
		return 1;
	else
		return 0;
}
