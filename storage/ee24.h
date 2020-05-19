#ifndef	_EE24_H
#define	_EE24_H

#include <stdbool.h>
#include <stdint.h>
#include "ee24Config.h"

bool    ee24_isConnected(void);
bool    ee24_write(uint16_t address, uint8_t *data, size_t lenInBytes, uint32_t timeout);	
bool    ee24_read(uint16_t address, uint8_t *data, size_t lenInBytes, uint32_t timeout);

#endif
