#include "IR.h"
#include "delay.h"

 	  		  
u32 Remote_Odr=0;  	 //�����ݴ洦
u8  Remote_Cnt=0;    //��������,�˴ΰ��¼��Ĵ���
u8  Remote_Rdy=0;    //������յ�����    
//��ʼ������������ŵ�����
//�����ж�,��ӳ�� 
void Remote_Init(void)
{							 
	RCC->APB2ENR|=1<<2;       //PAʱ��ʹ��		  
	GPIOA->CRL&=0XFFFFFF0F;
	GPIOA->CRL|=0X00000080;	//PA1����	 
	GPIOA->ODR|=1<<1;		//PA.1����      
	Ex_NVIC_Config(GPIO_A,1,FTIR);//��line1ӳ�䵽PA.1���½��ش���.
	MY_NVIC_Init(2,1,EXTI1_IRQChannel,2);
}   
//����������
//�����Ϊ5ms
//����ֵ:x,��������Ϊx*20us(x=1~250);
u8 Pulse_Width_Check(void)
{
    u8 t=0;	 
    while(RDATA)
    {	 
		t++;delay_us(20);					 
        if(t==250)return t; //��ʱ���
    }
    return t;
}			   
//�����������  
/*-------------------------Э��--------------------------
��ʼ����9ms,������һ��4.5ms�ĸ�����,֪ͨ������ʼ����������
�����Ƿ���4��8λ��������,��һ������ң��ʶ����(REMOTE_ID),��һ��Ϊ
����(0),�ڶ���Ϊ����(255),�������������Ǽ�ֵ,��һ��Ϊ����
�ڶ���Ϊ����.�������40ms,ң���ٷ���һ��9ms��,2ms�ߵ�����,
��ʾ�����Ĵ���,����һ����֤��ֻ������һ��,������ֶ��,���
����Ϊ�ǳ������¸ü�.
---------------------------------------------------------*/		 
//�ⲿ�жϷ������	   
void EXTI1_IRQHandler(void)
{       
	u8 res=0;
    u8 OK=0; 
    u8 RODATA=0;   		 
	while(1)
    {        
        if(RDATA)//�и��������
        {
            res=Pulse_Width_Check();//��ô˴θ��������       
            if(res==250)break;//�������ź�
            if(res>=200&&res<250)OK=1; //���ǰ��λ(4.5ms)
            else if(res>=85&&res<200)  //����������һ(2ms)
            {  							    		 
                Remote_Rdy=1;//���ܵ�����
                Remote_Cnt++;//������������
                break;
            }
            else if(res>=50&&res<85)RODATA=1;//1.5ms
            else if(res>=10&&res<50)RODATA=0;//500us
            if(OK)
            {
                Remote_Odr<<=1;
                Remote_Odr+=RODATA; 
                Remote_Cnt=0; //������������
            }   
        }			 						 
    } 	 	    
	EXTI->PR=1<<1;      //����жϱ�־λ        
}  
//�����������
//������Ӧ�ļ�ֵ
u8 Remote_Process(void)
{               
    u8 t1,t2;   
    t1=Remote_Odr>>24; //�õ���ַ��
    t2=(Remote_Odr>>16)&0xff;//�õ���ַ���� 
    Remote_Rdy=0;//������ 		      
    if(t1==(u8)~t2&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
    { 
        t1=Remote_Odr>>8;
        t2=Remote_Odr; 	
        if(t1==(u8)~t2)return t1; //������ֵ  
    }     
    return 0;
}






























