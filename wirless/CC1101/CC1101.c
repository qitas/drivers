
/*******************************************************************************
                         Qitas CC1101 for STM32
*******************************************************************************/
#include <stdio.h>

#include  "regs.h"
#include  "CC1101.h"

#define  PKT_LEN   60
#define  RX_FLAG  (halSpiReadStatus(RXBYTES)& NUM_RXBYTES)

u8  Status[8];
u8  CC1101_Mode;
u32 RF_CNT[4];
u8  RF_RX_MODE = 0; 
u8  RF_RX_STEP = 0; 
u8  RF_TX_STAT = 0; 

/*******************************************************************************
* Function Name  : halSpiByteWrite
* Description    : send 8 bit data to SDO line
*******************************************************************************/
void halSpiByteWrite(u8 cData)
{
    u8 i;
    for(i=0; i<8; i++)
    { 
         SCLK_LOW();  
         if((cData&0x80)==0x80)
         {
             SDO_HIGH();    
         }
         else
         {
             SDO_LOW();  
         }
         cData<<=1;
         Delay_us(3);
         SCLK_HIGH();    
         Delay_us(2);
    }
    SCLK_LOW();        
}

void halRfByteWrite(u8 cData)
{
    u8 i;
    for(i=0; i<8; i++)
    { 
         SCLK_LOW();  
         if((cData&0x80)==0x80)
         {
             SDO_HIGH();    
         }
         else
         {
             SDO_LOW();  
         }
         cData<<=1;
         Delay_us(3); 
         SCLK_HIGH();    
         Delay_us(2);
    }
    SCLK_LOW();        
}

/*******************************************************************************
* Function Name  : halSpiByteRead
* Description    : get 8 bit data from SDI line
*******************************************************************************/
u8 halSpiByteRead(void)
{
    u8 i, result;
    result = 0;
    SCLK_LOW();  
    Delay_us(3);
    for(i=0; i<8; i++)
    {
        SCLK_HIGH();
        result<<=1;
        if(SDI_IN())
        {
            result |= 0x01; 
        }
        Delay_us(2);
        SCLK_LOW(); 
        Delay_us(2);
    }
    return result;
}

/*******************************************************************************
* Function Name  : RF_HalInit
*******************************************************************************/
void Hal_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
    // CSn + SCLK + MOSI
    GPIO_InitStructure.GPIO_Pin   = SPI_CS | SPI_SCK | SPI_MOSI;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);
    // MISO
    GPIO_InitStructure.GPIO_Pin   = SPI_MISO;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GDO0_Pin | GDO2_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GDO_PORT, &GPIO_InitStructure);			
}

void CC1101_Init(void)
{
	u8 addr[3]={1,0X34,0X56};
    //halSpi_Init(); 
	halRf_Init(); 		
    POWER_RESET();
    WriteRfSettings(); 
	RF_SetPWR(0x18);	
	RF_SetADR(addr);
	RF_RX();
	RF_CNT[0]=0;
	RF_CNT[1]=0;	
	Delay_ms(1);
	GDO0_IRQ();
	GDO1_IRQ();
	GDO2_IRQ();
}
/*******************************************************************************
* Function Name  :
*******************************************************************************/
u8 GDO0_IRQ(void) 
{	
		EXTI_InitTypeDef EXTI_InitStructure;   
    NVIC_InitTypeDef NVIC_InitStructure; 	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    EXTI_ClearITPendingBit(EXTI_Line10);
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;    
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);  	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
}
/*******************************************************************************
* Function Name  :
*******************************************************************************/
u8 GDO2_IRQ(void) 
{	
		EXTI_InitTypeDef EXTI_InitStructure;   
    NVIC_InitTypeDef NVIC_InitStructure; 	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
    EXTI_ClearITPendingBit(EXTI_Line11);
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;    
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);  	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
}

/*******************************************************************************
* Function Name  : ADC_GDO
* Description    : Configure the ADC.
*******************************************************************************/
void ADC_GDO(void)
{
			GPIO_InitTypeDef GPIO_InitStructure;
			ADC_InitTypeDef ADC_InitStructure;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO, ENABLE);		
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
			GPIO_Init(GPIOA, &GPIO_InitStructure);   		
	
			ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	   
			ADC_InitStructure.ADC_ScanConvMode = ENABLE;			     
			ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	        
			ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  
			ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;		     
			ADC_InitStructure.ADC_NbrOfChannel = 1;					         
			ADC_Init(ADC1, &ADC_InitStructure);

			ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_239Cycles5);
			ADC_Cmd(ADC1, ENABLE);                                                       
			ADC_SoftwareStartConvCmd(ADC1,ENABLE);                 
}

/*******************************************************************************
* Function Name  : WriteRfSettings
* Description    :
*******************************************************************************/
void WriteRfSettings(void) {
    halRfWriteReg(FSCTRL1,  0x06);//FSCTRL1   Frequency synthesizer control.
    halRfWriteReg(FREQ2,    0x10);//FREQ2     Frequency control word, high byte.
    halRfWriteReg(FREQ1,    0xa7);//FREQ1     Frequency control word, middle byte
    halRfWriteReg(FREQ0,    0x62);//FREQ0     Frequency control word, low byte
    halRfWriteReg(MDMCFG4,  0xf6);//MDMCFG4   Modem configuration
    halRfWriteReg(MDMCFG3,  0x83);//MDMCFG3   Modem configuration.
    halRfWriteReg(MDMCFG2,  0x03);//0b, MDMCFG2   Modem configuration.
    halRfWriteReg(MDMCFG1,  0xa2);//MDMCFG1   Modem configuration.
    halRfWriteReg(MDMCFG0,  0xf8);//MDMCFG0   Modem configuration.               ------default value
    halRfWriteReg(CHANNR,   0x00);//CHANNR    Channel number.                      ------default value
    halRfWriteReg(DEVIATN,  0x15);//250k   Modem deviation setting (when FSK modulation is enabled).

    halRfWriteReg(FREND1,   0x56);//FREND1    Front end RX configuration
    halRfWriteReg(FREND0,   0x10);//FREND0    Front end RX configuration         ------default value
	
	  halRfWriteReg(MCSM1 ,   0x3F);//MCSM0     Main Radio Control State Machine configuration
    halRfWriteReg(MCSM0 ,   0x18);//MCSM0     Main Radio Control State Machine configuration
	  halRfWriteReg(FIFOTHR,  0x47); 
    halRfWriteReg(FOCCFG,   0x16);//FOCCFG    Frequency Offset Compensation Configuration
    halRfWriteReg(BSCFG,    0x6c);//BSCFG     Bit synchronization Configuration.   ------default value
    halRfWriteReg(AGCCTRL2, 0x03);//AGCCTRL2  AGC control                          ------default value
    halRfWriteReg(AGCCTRL1, 0x40);//AGCCTRL1  AGC control                          ------default value
    halRfWriteReg(AGCCTRL0, 0x91);//AGCCTRL0  AGC control.                       ------default value
    halRfWriteReg(FSCAL3,   0xe9);//FSCAL3    Frequency synthesizer calibration.
    halRfWriteReg(FSCAL2,   0x2a);//FSCAL2    Frequency synthesizer calibration. ------default value
    halRfWriteReg(FSCAL1,   0x00);//FSCAL1    Frequency synthesizer calibration.
    halRfWriteReg(FSCAL0,   0x1f);//FSCAL0    Frequency synthesizer calibration.
    halRfWriteReg(FSTEST,   0x59);
		
    halRfWriteReg(TEST2,    0x81);
    halRfWriteReg(TEST1,    0x35);
    halRfWriteReg(TEST0,    0x09);		
		
    halRfWriteReg(IOCFG0,   0x0E);
		halRfWriteReg(IOCFG2,   0x0E);	
	  halRfWriteReg(PKTCTRL1, 0x05);
    halRfWriteReg(PKTCTRL0, 0x44);
    //halRfWriteReg(PKTLEN,   PKT_LEN);//PKTLEN    Packet length.
}

/*******************************************************************************
* Function Name  : RF_SetPA
*******************************************************************************/
void RF_SetPWR(u8 cofig)
{
		u8 LS,HS;
		u8 paTable[10]= {0x03,0x17,0x1D,0x26,0x37,0x50,0x86,0xCD,0xC5,0xC0}; 
		HS=cofig>>4;
		LS=(cofig & 0x0F);
	  if(HS<1) HS=1;
		halSpiWriteBurstReg(PATABLE, paTable+LS, HS);   
		if(HS>1)  halRfWriteReg(FREND0, 0x17);
}
/*******************************************************************************
* Function Name  : RF_SetAddr
* Description    : 
*******************************************************************************/
void RF_SetADR(u8 *addr)
{ 
   if(*addr!=0)
	 {
 		  halRfWriteReg(ADDR, *addr); 
		  halRfWriteReg(PKTCTRL1, 0x0D);
	 }
   else  halRfWriteReg(PKTCTRL1, 0x0C);  	
   if(*(addr+1)!=0) halRfWriteReg(SYNC0, *(addr+1));  	
   if(*(addr+2)!=0) halRfWriteReg(SYNC1, *(addr+2));  		
}

/*******************************************************************************
* Function Name  : RF_RX
* Description    : 
*******************************************************************************/

void RF_RX(void)
{
	   halSpiStrobe(SFRX);
		 halSpiStrobe(SIDLE);	 
		 Delay_ms(1);  
		 halSpiStrobe(SRX);
	   CC1101_Mode=RF_RX_Flag;
}
/*******************************************************************************
* Function Name  : RF_RX
* Description    : 
*******************************************************************************/

void RF_IDLE(void)
{
//		 halSpiStrobe(SFRX);
//	   halSpiStrobe(SFTX);
		 halSpiStrobe(SIDLE);
     CC1101_Mode=RF_IDLE_Flag;
}
/*******************************************************************************
* Function Name  : RF_RX
* Description    : 
*******************************************************************************/

u8 RF_GET_STAT(void)
{
   return  halSpiReadStatus(MARCSTATE);
}
/*******************************************************************************
* Function Name  : halRfWriteReg
* Description    : 
*******************************************************************************/
void POWER_RESET(void)
{
    CSn_HIGH;
    Delay_us(3);
    CSn_LOW;
    Delay_us(3);
    CSn_HIGH;
    Delay_us(45);
    CSn_LOW;
    halSpiStrobe(SRES);
    CSn_HIGH;
}
/*******************************************************************************
* Function Name  : halSpiWriteBurstReg
* Description    : 
*******************************************************************************/
void halSpiWriteBurstReg(u8 addr, u8 *buffer, u8 count)
{
    u8 i, SPI_DATA;
    CSn_LOW;
    SPI_DATA = addr | WRITE_BURST;
    halSpiByteWrite(SPI_DATA);
    for (i = 0; i < count; i++)
    {
        SPI_DATA = buffer[i];
        halSpiByteWrite(SPI_DATA);
    }
    CSn_HIGH;
}
/*******************************************************************************
* Function Name  : halRfWriteReg
* Description    : 
*******************************************************************************/
void halRfWriteReg(u8 addr, u8 value)
{
    u8 SPI_DATA;	
    CSn_LOW;
    SPI_DATA = addr;
    halSpiByteWrite(SPI_DATA);
    SPI_DATA = value;
    halSpiByteWrite(SPI_DATA);
    CSn_HIGH;
}

/*******************************************************************************
* Function Name  : halRfWriteReg
* Description    : 
*******************************************************************************/

void halSpiStrobe(u8 strobe)
{
    u8 SPI_DATA;
    CSn_LOW;
    SPI_DATA = strobe;
    halSpiByteWrite(SPI_DATA);
    CSn_HIGH;
}

/*******************************************************************************
* Function Name  : halSpiReadStatus
* Description    : 
*******************************************************************************/
u8 halSpiReadStatus(u8 addr)
{
    u8 SPI_DATA;
    CSn_LOW;
    SPI_DATA = (addr | READ_BURST);
    halSpiByteWrite(SPI_DATA);
    SPI_DATA = halSpiByteRead();
    CSn_HIGH;
    return SPI_DATA;
}
u8 halRfReadStatus(u8 addr)
{
    u8 SPI_DATA;
    CSn_LOW;
    SPI_DATA = (addr | READ_BURST);
    halRfByteWrite(SPI_DATA);
    SPI_DATA = halSpiByteRead();
    CSn_HIGH;
    return SPI_DATA;
}

/*******************************************************************************
* Function Name  : halSpiReadBurstReg
* Description    : This function reads multiple CCxxx0 register, using SPI burst access.
*******************************************************************************/
void halRfReadBurstReg(u8 addr, u8 *buffer, u8 count)
{
    u8 i, SPI_DATA;
    CSn_LOW;
    SPI_DATA = (addr | READ_BURST);
    halSpiByteWrite(SPI_DATA);
    for (i = 0; i < count; i++)
    {
        SPI_DATA = halSpiByteRead();

        buffer[i] = SPI_DATA;
    }
    CSn_HIGH;
}
void halSpiReadBurstReg(u8 addr, u8 *buffer, u8 count)
{
    u8 i, SPI_DATA;
    CSn_LOW;
    SPI_DATA = (addr | READ_BURST);
    halSpiByteWrite(SPI_DATA);
    for (i = 0; i < count; i++)
    {
        SPI_DATA = halSpiByteRead();

        buffer[i] = SPI_DATA;
    }
    CSn_HIGH;
}
/*******************************************************************************
* Function Name  : halRfReceivePacket
* Description    :
*******************************************************************************/
u8  RfReceivePacket(u8 *rxBuffer, u8 length)
{
    u8 status[2];
    u8 packetLength;
    GDO0_HIGH_WAIT();
    GDO0_LOW_WAIT();
    if ((halSpiReadStatus(RXBYTES) & NUM_RXBYTES))
    {
        if (length <=PKT_LEN)
        {
          halSpiReadBurstReg(RXFIFO, rxBuffer, PKT_LEN);
          halSpiReadBurstReg(RXFIFO, status, 4);
          halSpiStrobe(SFRX);
            return 1;
        }
        else
        {
            halSpiStrobe(SFRX);
            RF_RX();
            return 0;
        }
    }
    else
        return FALSE;
}


/*******************************************************************************
* Function Name  : halRfReceivePacket
*******************************************************************************/
u8 halRfReceivePacket(u8 *rxBuffer,u8 *status,u8 length) 
{
    GDO0_HIGH_WAIT();
    GDO0_LOW_WAIT();
    if((halSpiReadStatus(RXBYTES) & NUM_RXBYTES))
    {
			  if (length <=62)
        {
						halSpiReadBurstReg(RXFIFO, rxBuffer, PKT_LEN);
						halSpiReadBurstReg(RXFIFO, status, 2);
						halSpiStrobe(SFRX);
						return 1;
        }
        else
        {
            halSpiStrobe(SFRX);
            RF_RX();
            return 0;
        }
    }
    else  return FALSE;
}

/*******************************************************************************
* Function Name  : halRfSendPacket
* Description    :
*******************************************************************************/
void halRfSendPacket(u8 *txBuffer, u8 size)
{
    halSpiStrobe(SIDLE); 
    Delay_ms(1);
    halSpiWriteBurstReg(TXFIFO, txBuffer, size+2);
    halSpiStrobe(STX);
	  halRfWriteReg(IOCFG1,  0x05);  
	  CC1101_Mode=RF_TX_Flag;	  
}
/*******************************************************************************
* Function Name  : halRfSendPacket
*******************************************************************************/
u8 GetRSSI(u8 rssi_dec)
{
		 u8  rssi_dBm; 
		 u8  rssi_offset = 74;  
		 if (rssi_dec >= 128) rssi_dBm =rssi_offset-(256-rssi_dec)/2; 
		 else rssi_dBm = rssi_offset-(rssi_dec/2);	
		 return rssi_dBm;		  
}


/*******************************************************************************
* Function Name  : RF_SetMode
*******************************************************************************/
u16 RF_GetTemp(void)
{ 
	  u16 AD_value[5]={0};	
    u8 cnt;		
		//halRfWriteReg(PTEST, 0xBF);
		halRfWriteReg(IOCFG0,0x80);
		Delay_ms(40);
	  ADC_GDO();
		for(cnt=0;cnt<3;cnt++)
		{
				if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==SET)
				{
						AD_value[cnt]=ADC_GetConversionValue(ADC1);	
				}
		}		
		//halRfWriteReg(IOCFG0,0x0e);
		return AD_value[0];
}

/*******************************************************************************
* Function Name  : RF_RX
*******************************************************************************/
void RF_END(void)
{
   //CSn_LOW;
   halSpiStrobe(SIDLE);
   Delay_us(100);
   halSpiStrobe(SXOFF);
   Delay_us(900);
   //CSn_HIGH;
}//
/*******************************************************************************
* Function Name  : 
*******************************************************************************/
u8 RX_GDO(u8 Chan) 
{	  
		Status[RF_RX_STEP]=halSpiReadStatus(RSSI);	  
	  if(Status[RF_RX_STEP]>70)  halRfWriteReg(FIFOTHR, 0x37); 
		switch(RF_RX_STEP)
		{
					case 0:
					{
							if(Chan==0)
								halRfWriteReg(IOCFG0,  0x06);
							else if(Chan==1)
								halRfWriteReg(IOCFG1,  0x06);
							else if(Chan==2)
								halRfWriteReg(IOCFG2,  0x06);													
							RF_RX_STEP=1;
					}
					break;	
					case 1:	
					{									
							if (RF_RX_MODE==RF_MODE_HEAR)
							{
									halRfWriteReg(IOCFG0,  0x01);
									RF_RX_STEP=2;	
							}
							else if(RF_RX_MODE==RF_MODE_FIFO)
							{
								  halRfWriteReg(IOCFG0,  0);
									RF_RX_STEP=2;	
							}
							else if(RF_RX_MODE==RF_MODE_CRC)
							{
								  halRfWriteReg(IOCFG0,  0x07);
									RF_RX_STEP=2;	
							}
							else if (RF_RX_MODE==RF_MODE_SERA)
							{
									RF_RX_STEP=4;	
							}
					}							
					break;
					case 2:  
				  {			
								halSpiReadBurstReg(RXFIFO,RxBuf,PKT_LEN);						  
								halSpiReadBurstReg(RXFIFO, Status+3, 2);	          						
								RF_RX_STEP=0;					
					}
					break;	
	 }
	 RF_RX();
}

/*******************************************************************************

 ******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
		RF_RX_MODE =RF_MODE_FIFO;// RF_MODE_CRC; 
		if (EXTI_GetITStatus(EXTI_Line10) != RESET ) 
		{
				 EXTI_ClearITPendingBit(EXTI_Line10);
				 if(CC1101_Mode==0)	CC1101_Mode=RF_TX_Finish;
				 else if(CC1101_Mode==RF_RX_Flag)  RX_GDO(0) ;	
				 RF_CNT[1]++;				
		}	
		else if(EXTI_GetITStatus(EXTI_Line11) != RESET ) 
		{			
					EXTI_ClearITPendingBit(EXTI_Line11);
					if(CC1101_Mode==RF_RX_Flag) RX_GDO(2);
					RF_CNT[0]++;
		}	
		else if(EXTI_GetITStatus(EXTI_Line14) != RESET ) 
		{		
					EXTI_ClearITPendingBit(EXTI_Line14);				
					if(CC1101_Mode==RF_TX_Flag)
					CC1101_Mode=RF_TX_Finish;
//				else if(CC1101_Mode==RF_RX_Flag)
//				RX_GDO(1);
		}	
}
