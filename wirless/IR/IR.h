#ifndef _IR_H_
#define _IR_H_

#include "MAP.h"

#define  IR_TX     0x11
#define  IR_RX     0x37



void IR_TX_Init(u8 chan);
void RX_Init(void);
void RX_IO_Init(void);
void IR_PWM_OCInit(u8 chan,u8 Rate);

void IR_TX_Close(void);

void IR_Pulse_ON(u16 Pulse);
void IR_Pulse_OFF(void);
void IRSend(u8 ADDR,u8 *Data,u8 LEN);
void IR_Recv(void);
void IR_TIM(u16 arr,u16 psc);


#endif  /* _TIME_H_ */
