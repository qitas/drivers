#ifndef __SENSOR_485_H__
#define __SENSOR_485_H__

#include "stm32f10x.h"
#include <stdio.h>

     

void RS485_INIT(void);
u8  RS485_SET_HT(u8 addr,u8 Register,u16 value);
u8  RS485_SET_PR(u8 addr,u8 Register,u16 value);
u8 AUTO_CTRL_HT(u16 temp,u16 humi,u8 mode);
void RS485_GET(u8 addr,u8 head, u8 len, u16* DataOut);

#endif

