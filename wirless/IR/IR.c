
#include "IR.h"

#define IR_LED_H   GPIOB->BSRR|=0X0020
#define IR_LED_L   GPIOB->BRR |=0X0020 
#define IR_LED_C   GPIOB->ODR^=0X0020

#define  IR_ON     IR_Pulse_ON(Pulse_Rate)	 
#define  IR_OFF    IR_Pulse_OFF()


u16 Timeout=10000;// 10us
u16 TimeA=2000;
u16 TimeB=800;
u16 TimeC=180;
u16 TimeD=60;

u8  IR_MODE = 0; 
u8  Send_Status = 0; 
u8  Recv_Status = 0;   	
u8 Receive_time_cnt = 0 ;	
u16 Pulse_Rate = 0;
u16 TIM_cnt;   
u8 IR_NUM = 0; 
u8 Send_Byte[16]={0};
u8 Recv_Byte[16]={0};
u8 IR_Byte = 0; 
u8 IR_Bit = 0; 
/*******************************************************************************

 ******************************************************************************/
void IR_TX_Init(u8 chan)
{
	  IR_PWM_OCInit(chan,2);
	  IR_TIM(56,719); 
	  IR_MODE=IR_TX;
	  TIM_cnt=0;	
	  Send_Status=0;
}
void IR_TX_Close(void)
{
		TIM_Cmd(TIM2, DISABLE);
	  TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
	  IR_Pulse_OFF();
    IR_MODE=IR_RX;	
	  IR_LED_L;	
}

/*******************************************************************************

 ******************************************************************************/
void IRSend(u8 ADDR,u8 *Data,u8 LEN)
{
	
	   IR_TX_Init(ADDR&0x0F);
		 for(IR_NUM = 0;IR_NUM<LEN;IR_NUM++)
		 {
				Send_Byte[IR_NUM*2]=*(Data+IR_NUM);
				Send_Byte[IR_NUM*2+1]=0XFF-Send_Byte[IR_NUM*2];
		 }		
		 TIM_Cmd(TIM2, ENABLE);
		 TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}


/*******************************************************************************

 ******************************************************************************/
void IR_Send(void)
{ 
	 u16 cnt,buff;
	 switch(Send_Status)
	 {
		case 0:
		{
				TIM_cnt = 0;
				Send_Status=1;	
        IR_ON;				
		}
		break;	
		case 1:
		{										
				if(TIM_cnt > 15) IR_OFF;		
				if(TIM_cnt > 22)
				{
						IR_Byte=Send_Byte[0];			
						Send_Status=3;
						TIM_cnt = 0;
				}				
		}
		break;					
		case 2:
		{
		}
		break;
		case 3:
		{
				 buff=TIM_cnt;	
			   buff>>=2;
			   buff&=0x0007;	
				 IR_Bit=(u8)((IR_Byte<<buff)&0x0080);		
				 if(IR_Bit==0) 
				 {				   
						 if(TIM_cnt&0X0002)IR_OFF;	 
					   else  IR_ON;
						 TIM_cnt++;		
					 	 if(buff==7 && (TIM_cnt&0X0002))
						 {									
								cnt=(u8)(TIM_cnt>>5);				  					
								IR_Byte=Send_Byte[cnt+1];						 
						 }							 
				 }
				 else
				 {
						 if(TIM_cnt&0X0003) IR_OFF;	
						 else  IR_ON ;
					 	 if(buff==7 && (TIM_cnt&0X0003)==3)
						 {									
								cnt=(u8)(TIM_cnt>>5);				  					
								IR_Byte=Send_Byte[cnt+1];						 
						 }	
			   }			 
		}
		break;
	}
}
/*******************************************************************************

 ******************************************************************************/
void IR_TIM(u16 arr,u16 psc) 
{
			NVIC_InitTypeDef NVIC_InitStructure;
			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
      TIM_DeInit(TIM2);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
			TIM_TimeBaseStructure.TIM_Period = arr; 
			TIM_TimeBaseStructure.TIM_Prescaler =psc; 
			TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
			TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	
			TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
			TIM_ClearFlag(TIM2, TIM_FLAG_Update);							  
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
			NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);		
}
/*******************************************************************************

 ******************************************************************************/
void TIM2_IRQHandler(void)  
{
	 
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		{
			 if(IR_MODE==IR_TX)
			 {
					TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
					IR_Send();			
					TIM_cnt++;				
					if((TIM_cnt>>5)>(2*IR_NUM-1)) IR_TX_Close();		
			 }
			else
			{
					TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 			
					TIM_cnt++;					
			}							
		}	
}

/*******************************************************************************

 ******************************************************************************/
void RX_IO_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure; 
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO , ENABLE); 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
		EXTI_ClearITPendingBit(EXTI_Line5);

		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 			
}

/*******************************************************************************

 ******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
	u8 cnt,mark=1;
	if (EXTI_GetITStatus(EXTI_Line5) != RESET ) 
	{
		  EXTI_ClearITPendingBit(EXTI_Line5);
		  IR_Recv();		
      TIM_cnt=0;				
	}	
}
/*******************************************************************************

 ******************************************************************************/
void RX_Init(void)
{
	  RX_IO_Init();
    IR_MODE=IR_RX;
    IR_NUM=0;		
		IR_TIM(10,71); 
		TIM_Cmd(TIM2, ENABLE);
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);					
}
/*******************************************************************************

 ******************************************************************************/
void IR_Recv(void)
{ 
	 u16 cnt,buff;
	 switch(Recv_Status)
	 {
			case 0:
			{
				  if(TIM_cnt>TimeB && TIM_cnt<TimeA) Recv_Status = 2;
				  IR_Byte=0;
				  IR_NUM=0;					
			}
			break;	
			case 1:
			{		
			}
			break;					
			case 2:
			{
					 if(TIM_cnt>TimeD && TIM_cnt<TimeC) 
						{
								IR_Byte+=0;
								IR_NUM++;
						}
					  else if(TIM_cnt>TimeC && TIM_cnt<TimeB) 
						{
								IR_Byte+=1;
								IR_NUM++;
						}
					  else Recv_Status = 0;	
					  if(Recv_Status)
						{		
									if(IR_NUM&0x07)	IR_Byte<<=1; 
									else 
									{
											cnt=IR_NUM>>3;
											Recv_Byte[cnt-1]=IR_Byte;
											IR_Byte=0;
									}		
						}	
			}
			break;
	}
}
/*******************************************************************************

 ******************************************************************************/

void IR_PWM_OCInit(u8 chan,u8 Rate)
{
		uint16_t TimerPeriod = 0;  
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		TimerPeriod = (SystemCoreClock / 38500 ) - 1;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  	
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE); 	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		TIM_DeInit(TIM4);
		TIM_TimeBaseStructure.TIM_Prescaler = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
		TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

		TIM_OCInitStructure.TIM_OCMode =TIM_OCMode_PWM2;//TIM_OCMode_Toggle;// 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_Low;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;		
    Pulse_Rate = (uint32_t)(TimerPeriod - 1);
		TIM_OCInitStructure.TIM_Pulse =(u16)(Pulse_Rate/Rate);
		if(chan&0x01) TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		if(chan&0x02) TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		if(chan&0x04) TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		if(chan&0x08) TIM_OC4Init(TIM4, &TIM_OCInitStructure);	
		TIM_Cmd(TIM4, DISABLE);
		TIM_CtrlPWMOutputs(TIM4, DISABLE);	
}

void IR_Pulse_ON(u16 Pulse)
{
	TIM_Cmd(TIM4, ENABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);	
	TIM_SetCompare1(TIM4,Pulse);
}

void IR_Pulse_OFF(void)
{
		TIM_Cmd(TIM4, DISABLE);
		TIM_CtrlPWMOutputs(TIM4, DISABLE);	
		TIM_SetCompare1(TIM4,0);
}

	
