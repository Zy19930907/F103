#include "Led.h"

void LED_Init(void)
{
	RCC->APB2ENR |= 0x48; //使能PE、PB时钟
	
	GPIOE->CRL &= ~(0x0F << 20);//清除PE5
	GPIOE->CRL |= (0x03 << 20);//PE5配置为推挽输出，速度50MHz
	
	GPIOB->CRL &= ~(0x0F << 20);//清除PB5
	GPIOB->CRL |= (0x03 << 20);//PB5配置为推挽输出，速度50MHz
	
	GPIOE->ODR |= (0x01 << 5);//PE5拉高，关闭LED0
	GPIOB->ODR &= ~(0x01 << 5);//PB5拉高，关闭LED1
}
