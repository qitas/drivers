
#ifndef CC1101_H
#define CC1101_H

#include "map.h"
#include  "SPI.h"

#define  RF_MODE_FIFO        0
#define  RF_MODE_HEAR     0X01
#define  RF_MODE_SYNC     0X06
#define  RF_MODE_CRC    	0X07
#define  RF_MODE_SERA     0X0B

#define  RF_RX_Flag       0X11
#define  RF_TX_Flag       0X15
#define  RF_IDLE_Flag     0X17
#define  RF_TX_Finish     0x19

#define	  SPI_PORT 	  	GPIOB
#define	  SPI_CS  	  	GPIO_Pin_12
#define	  SPI_SCK  	  	GPIO_Pin_13
#define	  SPI_MISO  	GPIO_Pin_14
#define	  SPI_MOSI  	GPIO_Pin_15
#define	  GDO_PORT 	  	GPIOB
#define	  GDO0_Pin    	GPIO_Pin_10
#define	  GDO2_Pin    	GPIO_Pin_11

//SCLK
#define SCLK_LOW()   GPIO_ResetBits(SPI_PORT, SPI_SCK)
#define SCLK_HIGH()  GPIO_SetBits(SPI_PORT, SPI_SCK)
//MISO
#define SDI_IN()       		GPIO_ReadInputDataBit(SPI_PORT,SPI_MISO)
#define SDI_LOW_WAIT()   	while(1==SDI_IN()) 
#define SDI_HIGH_WAIT()  	while(0==SDI_IN())  

#define SDO_LOW()   GPIO_ResetBits(SPI_PORT, SPI_MOSI)
#define SDO_HIGH()  GPIO_SetBits(SPI_PORT, SPI_MOSI)

#define CSn_LOW   GPIO_ResetBits(SPI_PORT, SPI_CS)
#define CSn_HIGH  GPIO_SetBits(SPI_PORT, SPI_CS)

#define GDO0_LOW_WAIT()   while(1==GPIO_ReadInputDataBit(GDO_PORT,GDO0_Pin))  //wait untill CC1100's SO to be cleared
#define GDO0_HIGH_WAIT()  while(0==GPIO_ReadInputDataBit(GDO_PORT,GDO0_Pin))  //wait untill CC1100's SO to be set

#define GDO2_LOW_WAIT()   while(1==GPIO_ReadInputDataBit(GDO_PORT,GDO2_Pin))  //wait untill CC1100's SO to be cleared
#define GDO2_HIGH_WAIT()  while(0==GPIO_ReadInputDataBit(GDO_PORT,GDO2_Pin))  //wait untill CC1100's SO to be set



extern u8 TxBuf[61];
extern u8 RxBuf[61]; 
extern u8 Status[8];
extern u8  CC1101_Mode;

void RF_RX(void);
void RF_TX(void);
void RF_END(void);
void RF_IDLE(void);
void POWER_RESET(void);
void CC1101_Init(void);
void WriteRfSettings(void) ;

void RF_SetPWR(u8 cofig);
void RF_SetADR(u8 *addr);
void RF_SetPKT(u8 mode,u16 leng);
void RF_SetGDO(u8 fifo);
void ADC_GDO(void);
u16 RF_GetTemp(void);
u8 GetRSSI(u8 rssi_dec);

void halRfStrobe(u8 strobe);
void halRfWriteReg(u8 addr, u8 value);
u8 halRfReadStatus(u8 addr);
void halRfReadBurstReg(u8 addr, u8 *buffer, u8 count);
void halRfSendPacket(u8 *txBuffer, u8 size);
u8 RfReceivePacket(u8 *rxBuffer, u8 length) ;
u8 halRfReceivePacket(u8 *rxBuffer,u8 *status,u8 length);

void halSpiStrobe(u8 strobe);
void halSpiWriteReg(u8 addr, u8 value);
u8 halSpiReadStatus(u8 addr);
u8 halSpiReadReg(u8 addr);
void halSpiWriteBurstReg(u8 addr, u8 *buffer, u8 count);
void halSpiReadBurstReg(u8 addr, u8 *buffer, u8 count);
void halSpiSendPacket(u8 *txBuffer, u8 size);




#endif
