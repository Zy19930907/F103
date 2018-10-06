#include "Led.h"

void LED_Init(void)
{
	RCC->APB2ENR |= 0x48; //ʹ��PE��PBʱ��
	
	GPIOE->CRL &= ~(0x0F << 20);//���PE5
	GPIOE->CRL |= (0x03 << 20);//PE5����Ϊ����������ٶ�50MHz
	
	GPIOB->CRL &= ~(0x0F << 20);//���PB5
	GPIOB->CRL |= (0x03 << 20);//PB5����Ϊ����������ٶ�50MHz
	
	GPIOE->ODR |= (0x01 << 5);//PE5���ߣ��ر�LED0
	GPIOB->ODR &= ~(0x01 << 5);//PB5���ߣ��ر�LED1
}
