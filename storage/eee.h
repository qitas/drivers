#ifndef __EEEPROM_H
#define __EEEPROM_H
//EEPROM emulation STM32 HAL Library  
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
// #include "gpio.h"

//################################################################################################################
bool      ee_init(void);
bool      ee_format(bool keepRamData);
bool      ee_read(uint32_t startVirtualAddress, uint32_t len, uint8_t* data);
bool      ee_write(uint32_t startVirtualAddress, uint32_t len, uint8_t* data);
bool      ee_writeToRam(uint32_t startVirtualAddress, uint32_t len, uint8_t* data); //  only use when _EE_USE_RAM_BYTE is enabled
bool      ee_commit(void);  //  only use when _EE_USE_RAM_BYTE is enabled
uint32_t  ee_maxVirtualAddress(void);

//################################################################################################################

#ifdef __cplusplus
}
#endif

#endif
