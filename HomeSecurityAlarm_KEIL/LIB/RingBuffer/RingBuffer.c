
#include "RingBuffer.h"

uint8_t FLAG_RECEIVE = 0;

/* Initialize the ring buffer */
void RingBuff_Init(ringbuff_t *_rx_buffer)
{
	memset(_rx_buffer->Buffer, '\0', BufferSize);
	_rx_buffer->Size = BufferSize;
	_rx_buffer->WR   = 0;
	_rx_buffer->RD   = 0;
	_rx_buffer->Diff = 0;
}

/* Calculate the length of the new data */
void RingBuff_GetLength(ringbuff_t *_rx_buffer)
{
	_rx_buffer->WR = _rx_buffer->Size - huart1.hdmarx->Instance->NDTR;
	if(_rx_buffer->WR >= _rx_buffer->RD)
	{
		_rx_buffer->Diff = _rx_buffer->WR - _rx_buffer->RD;
	}
	else
	{
		_rx_buffer->Diff = (_rx_buffer->Size - _rx_buffer->RD) + _rx_buffer->WR;
	}
}
	
/* Check DataAvaiable for Write: if the Diff + dLen < Size, we can write new data */
uint8_t RingBuff_GetAvaiableWrite(ringbuff_t *_rx_buffer, uint16_t dLen)
{
	RingBuff_GetLength(_rx_buffer);
	if(_rx_buffer->Diff + dLen < _rx_buffer->Size)
		return 1;
	else
		return 0;
}

/* Increase Write count as long as the offset */
__inline void RingBuff_Mov_WrIdx(ringbuff_t *_rx_buffer, uint16_t offset)
{
	_rx_buffer->WR += offset;
	if(_rx_buffer->WR >= _rx_buffer->Size)
	{
		_rx_buffer->WR %= _rx_buffer->Size;
	}
}

/* writes the data to the buffer and increment the write count in buffer */
void RingBuff_Write(ringbuff_t *_rx_buffer, uint8_t *dataIn, uint16_t dLen)
{
	uint16_t count;
	
	for(count = 0; count < dLen; count++)
	{
		_rx_buffer->Buffer[_rx_buffer->WR] = dataIn[count]; //Load data
		RingBuff_Mov_WrIdx(_rx_buffer, 1);	// offset = 1 because it is in loop
	}	
}

/* Check DataAvaiable for Read: if the Diff >= dLen, we have new data */
uint8_t RingBuff_GetAvaiableRead(ringbuff_t *_rx_buffer, uint16_t dLen)
{
	RingBuff_GetLength(_rx_buffer);
	if(_rx_buffer->Diff >= dLen)
		return 1;
	else
		return 0;
}

/* Increase Read count as long as the offset */
__inline void RingBuff_Mov_RdIdx(ringbuff_t *_rx_buffer, uint16_t offset)
{
	_rx_buffer->RD += offset;
	if(_rx_buffer->RD >= _rx_buffer->Size)
	{
		_rx_buffer->RD %= _rx_buffer->Size;
	}
}

/* reads the data in the buffer and increment the read count in buffer */
void RingBuff_Read(ringbuff_t *_rx_buffer, uint8_t *dataOut, uint16_t dLen)
{
	uint16_t count;
	
	for(count = 0; count < dLen; count++)
	{
		dataOut[count] = _rx_buffer->Buffer[_rx_buffer->RD]; //Read Data
		RingBuff_Mov_RdIdx(_rx_buffer, 1); // offset = 1 because it is in loop
	}
}

/* Peek for the data in the buffer without incrementing the read count and returns the character */
// Example: if (peek() == 'M') do something
uint8_t RingBuff_Peek(ringbuff_t *_rx_buffer, uint8_t *dataOut, uint16_t dLen)
{
	uint8_t RD;
	uint16_t count;
	
	if(RingBuff_GetAvaiableRead(_rx_buffer, dLen))
	{
		RD = _rx_buffer->RD;
		for(count = 0; count < dLen; count++)
		{
			dataOut[count] = _rx_buffer->Buffer[RD];
			RD++;
			if(RD == _rx_buffer->Size)
				RD = 0;
		}
		return 1; // return success to indicate successful peek.
	}
	else
	{
		return 0;
	}
}

/* Resets the entire ring buffer, the new data will start from position 0 */
void RingBuff_Flush(ringbuff_t *_rx_buffer)
{
	memset(_rx_buffer->Buffer, '\0', _rx_buffer->Size);
	_rx_buffer->WR = 0;
	_rx_buffer->RD = 0;
	_rx_buffer->Diff = 0;
}
