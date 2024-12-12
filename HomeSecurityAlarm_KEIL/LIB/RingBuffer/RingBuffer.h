
#ifndef _RingBuffer_H
#define _RingBuffer_H

#include "string.h"
#include "stdint.h"
#include "usart.h"

#define PacketSize 		100
#define BufferSize 		2*PacketSize 			// 200

typedef struct{
	uint8_t Buffer[BufferSize];
	uint8_t Size;
	uint8_t WR;
	uint8_t RD;
	uint8_t Diff;
}ringbuff_t;

extern uint8_t FLAG_RECEIVE;

void RingBuff_Init(ringbuff_t *_rx_buffer);
void RingBuff_GetLength(ringbuff_t *_rx_buffer);
uint8_t RingBuff_GetAvaiableWrite(ringbuff_t *_rx_buffer, uint16_t dLen);
__inline void RingBuff_Mov_WrIdx(ringbuff_t *_rx_buffer, uint16_t offset);
void RingBuff_Write(ringbuff_t *_rx_buffer, uint8_t *dataIn, uint16_t dLen);
uint8_t RingBuff_GetAvaiableRead(ringbuff_t *_rx_buffer, uint16_t dLen);
__inline void RingBuff_Mov_RdIdx(ringbuff_t *_rx_buffer, uint16_t offset);
void RingBuff_Read(ringbuff_t *_rx_buffer, uint8_t *dataOut, uint16_t dLen);
uint8_t RingBuff_Peek(ringbuff_t *_rx_buffer, uint8_t *dataOut, uint16_t dLen);
void RingBuff_Flush(ringbuff_t *_rx_buffer);

#endif
