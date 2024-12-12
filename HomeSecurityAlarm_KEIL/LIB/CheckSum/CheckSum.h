
#ifndef _CheckSum_H
#define _CheckSum_H

#include "stdint.h"

void CheckSum_Write(uint8_t *data, uint8_t len);
uint8_t CheckSum_Read(uint8_t *data, uint8_t len);

#endif
