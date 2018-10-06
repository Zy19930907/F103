#include "delay.h"

vu32 SYS_TICK;
extern __IO int32_t OS_TimeMS;

u32 MsTickDiff(u32 tick)
{
	if(tick <= SYS_TICK)
		return (SYS_TICK - tick);
	else
		return (0xFFFFFFFF - tick + SYS_TICK);
}

void TIM3_IRQHandler(void)
{ 	
	if(TIM3->SR&0x0001)//����ж�
		SYS_TICK ++;
		OS_TimeMS++;
	TIM3->SR &= ~(1<<0);//����жϱ�־λ 		   
}
//��ʼ��ϵͳʱ�ӣ�1ms�ۼ�
void SysTickInit(void)
{
	RCC->APB1ENR |= 1<<1;//TIM3ʱ��ʹ��    
 	TIM3->ARR = 1000;  //�趨�������Զ���װֵ//�պ�1ms    
	TIM3->PSC = 71;  //Ԥ��Ƶ��72,�õ�1MHz�ļ���ʱ��

	TIM3->DIER |= 1<<0;   //��������ж�				
	TIM3->DIER |= 1<<6;   //�������ж�	   
	TIM3->CR1 |= 0x00000001;
  	MY_NVIC_Init(1,3,0x1D,2);
}

void delay_ms(u16 nms)
{
	u32 tick = SYS_TICK;
	while(MsTickDiff(tick) < nms);
}

void delay_us(u32 nus)
{
	if(nus > 0)
	do{
		__nop();
	}while(--nus);
}
