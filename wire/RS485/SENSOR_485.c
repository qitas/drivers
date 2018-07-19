
#include "SENSOR_485.h"
#include "CRC_MODBUS.h"

u8 HT_ADDR=1;
u8 PR_ADDR=1;
u8 cnt,cout=0,RS485[20]={0};

#define DIR485_RX      GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define DIR485_TX      GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define RS485_ON      GPIO_ResetBits(GPIOC , GPIO_Pin_13)
#define RS485_OFF     GPIO_SetBits(GPIOC , GPIO_Pin_13)
void Delay_ms_485(uint16_t i) 
{
	uint16_t x=0;  
	 while(i--)
	 {
		x=8000;  
		while(x--);    
	 }
}

void NVIC_ConfigurationUSART1(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;     
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RS485_INIT(void)
{
    USART_InitTypeDef USART_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure; 
	  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;
    //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	  USART_Cmd(USART1, DISABLE);
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		NVIC_ConfigurationUSART1();
    USART_Cmd(USART1, ENABLE); 
		RS485_ON;
}

int writeUSART1(int ch)
{
	   USART_SendData(USART1, (unsigned char) ch);  
    while( USART_GetFlagStatus(USART1,USART_FLAG_TC)== RESET); 
    return (ch);
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {     
						  RS485[cout]=USART_ReceiveData(USART1);
					    cout++;		
    }
}

u8 RS485_SET_HT(u8 addr,u8 Register,u16 value)
{
	    u8 cnt,buff;
	    RS485_ON;
	    	//   RS485_OFF;
			RS485[0]=addr;
	    HT_ADDR=addr;
			RS485[1]=0X06;
	    RS485[2]=0;
	    RS485[3]=Register;
			buff=value>>8;
	    RS485[4]=buff;
	    buff=(u8)(value&0x00ff);
	    RS485[5]=buff;  
		  CheckCRCModBus(RS485,6, RS485+6); 
	    cout=0;  
	    DIR485_TX;
      for(cnt=0;cnt<8;cnt++) writeUSART1(RS485[cnt]);
	    DIR485_RX;
	    for(cnt=0;cnt<18;cnt++) RS485[cnt]=0;
	    
		  cnt=250;
	    Delay_ms_485(500);
	    while(RS485[0]==0 && cnt){ cnt--; Delay_ms_485(4);}	 
			RS485_OFF;
	   // Delay_ms(800);
	    if(RS485[0]==addr && RS485[3]==Register) return 1;
	    return 0;
}

u8 RS485_SET_PR(u8 addr,u8 Register,u16 value)
{
	    u8 cnt,buff;
	    RS485_ON;
	    
			RS485[0]=addr;
			RS485[1]=0x10;
	    RS485[2]=0;
	    RS485[3]=Register;
		  RS485[4]=0;
	    RS485[5]=1;
	    RS485[6]=2;
			buff=value>>8;
	    RS485[7]=buff;
	    buff=(u8)(value&0x00ff);
	    RS485[8]=buff;  
		  CheckCRCModBus(RS485,9, RS485+9); 
	    cout=0;  
	 DIR485_TX;
      for(cnt=0;cnt<11;cnt++) writeUSART1(RS485[cnt]);
	 DIR485_RX;
	    for(cnt=0;cnt<18;cnt++) RS485[cnt]=0;
	   
		  cnt=250;
			Delay_ms_485(500);
	    while(RS485[0]==0 && cnt){ cnt--; Delay_ms_485(6);}	    
			RS485_OFF;			
	    if(RS485[0]==addr) return 1;
	    return 0;
}

u8 AUTO_CTRL_HT(u16 temp,u16 humi,u8 mode)
{
	    //u8 cnt,buff;
	    RS485_SET_HT(HT_ADDR,0X04,mode);
		RS485_SET_HT(HT_ADDR,0X03,humi);
		RS485_SET_HT(HT_ADDR,0X02,temp);
	    return 0;
}

void RS485_GET(u8 addr,u8 head, u8 len, u16* DataOut)
{
	    int cnt,buff;
	    RS485_ON;
	    
		RS485[0]=addr;
		RS485[1]=0X03;
	    RS485[2]=0;
	    RS485[3]=head;
	    RS485[4]=0;
	    RS485[5]=len;
	    CheckCRCModBus(RS485,6, RS485+6);  
	    cout=0; 
	DIR485_TX;
      for(cnt=0;cnt<8;cnt++) writeUSART1(RS485[cnt]);
	 DIR485_RX;
	    for(cnt=0;cnt<18;cnt++) RS485[cnt]=0;
	   
		  cnt=250;
	    Delay_ms_485(500);
	    while(RS485[0]==0 && cnt){ cnt--; Delay_ms_485(4);}	 
	    for(cnt=0;cnt<len;cnt++)
			{
				   buff=3+cnt*2;
					*(DataOut+cnt)= RS485[buff];
					*(DataOut+cnt)<<=8;
					*(DataOut+cnt)+=RS485[buff+1];
			}
			RS485_OFF;
}
u16  RS485_GET_HT(u8 addr, u16* DataOut)
{
	    int cnt,buff;
	    RS485_ON;

			RS485[0]=addr;
			RS485[1]=0X03;
	    RS485[2]=0;
	    RS485[3]=0;
	    RS485[4]=0;
	    RS485[5]=0X02;
	    CheckCRCModBus(RS485,6, RS485+6);
	    cout=0; 
	 DIR485_TX;
      for(cnt=0;cnt<8;cnt++) writeUSART1(RS485[cnt]);
	  DIR485_RX;
	    for(cnt=0;cnt<8;cnt++) RS485[cnt]=0;
	  
		  cnt=250;
	    while(RS485[0]==0 && cnt){ cnt--; Delay_ms_485(6);}	    
	   // Delay_ms(400);
		*(DataOut+cnt)= RS485[buff];
		*(DataOut+cnt)<<=8;
		*(DataOut+cnt)+=RS485[buff+1];
			
	    buff= RS485[3];
	    buff<<=8;
	    buff+=RS485[4];
		*DataOut=buff;
		buff= RS485[5];
	    buff<<=8;
	    buff+=RS485[6];
		*(DataOut+1)=buff;
		RS485_OFF;
	    return buff;
}
