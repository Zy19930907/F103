#ifndef __LED_H
#define __LED_H

#include "Public.h"

#define LED0CHANGE		GPIOE->ODR ^= (1 << 5)
#define LED1CHANGE		GPIOB->ODR ^= (1 << 5)

void LED_Init(void);//³õÊ¼»¯

#endif
