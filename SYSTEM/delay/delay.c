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
	if(TIM3->SR&0x0001)//溢出中断
		SYS_TICK ++;
		OS_TimeMS++;
	TIM3->SR &= ~(1<<0);//清除中断标志位 		   
}
//初始化系统时钟，1ms累加
void SysTickInit(void)
{
	RCC->APB1ENR |= 1<<1;//TIM3时钟使能    
 	TIM3->ARR = 1000;  //设定计数器自动重装值//刚好1ms    
	TIM3->PSC = 71;  //预分频器72,得到1MHz的计数时钟

	TIM3->DIER |= 1<<0;   //允许更新中断				
	TIM3->DIER |= 1<<6;   //允许触发中断	   
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
