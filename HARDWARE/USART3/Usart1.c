#include "Usart1.h"

_COM1 Com1;

void COM1TEST(u8 *buf,u16 len)
{
	COM1SendBuf(buf,len);
}

void COM1AddObserver(RecvFunction function)
{
	u8 i;
	for(i=0;i<MAXUSART3OBSERVER;i++)
	{
		if((Com1.ObsererFlag >> i) & 0x01)
			continue;
		Com1.UsartRecvDeal[i] = function;
		Com1.ObsererFlag |= (1<<i);
		return;
	}
}

void COM1DelObserver(RecvFunction function)
{
	u8 i;
	for(i=0;i<MAXUSART3OBSERVER;i++)
	{
		if(!((Com1.ObsererFlag >> i) & 0x01))
			continue;
		if(Com1.UsartRecvDeal[i] == function)
			Com1.ObsererFlag &= ~(1<<i);
	}
}

void COM1NoticeAllObserver(void)
{
	u8 i;
	for(i=0;i<MAXUSART3OBSERVER;i++)
	{
		if(!((Com1.ObsererFlag >> i) & 0x01))
			continue;
		Com1.UsartRecvDeal[i](&Com1.RXBUF[0],Com1.RxCnt);
	}
}
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void usart1_init(u32 pclk1,u32 bound)
{  	
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp = (float)(pclk1*1000000)/(bound*16);//得到USARTDIV
	mantissa = temp;				 //得到整数部分
	fraction = (temp-mantissa)*16; //得到小数部分	 
    mantissa <<= 4;
	mantissa += fraction; 
	//DMA发送配置
	RCC ->AHBENR |= 0x01; //使能DMA时钟
	DMASENDCHANNEL ->CPAR = (u32)&(USART1->DR);  //外设地址
	DMASENDCHANNEL ->CMAR = (u32)&Com1.TXBUF[0];//存储器地址
	DMASENDCHANNEL ->CCR = 0x00000000;         //复位
	DMASENDCHANNEL ->CCR |= 1<<4;             //传输方向，从存储器读数据输出到外设
	DMASENDCHANNEL ->CCR |= 0<<5;             //不执行循环操作
	DMASENDCHANNEL ->CCR |= 0<<6;             //外设地址不递增
	DMASENDCHANNEL ->CCR |= 1<<7;             //存储器地址递增
	DMASENDCHANNEL ->CCR |= 0<<8;            //外设数据宽度8BIT
	DMASENDCHANNEL ->CCR |= 0<<10;           //存储数据宽度8bit
	DMASENDCHANNEL ->CCR |= 1<<12;           //中等优先级
	DMASENDCHANNEL ->CCR |= 0<<14;            //非存储器到存储器
	
	//DMA接收配置
	DMARECVCHANNEL ->CPAR = (u32)&(USART1->DR);         //外设地址
	DMARECVCHANNEL ->CMAR = (u32)&Com1.RXBUF[0];         //存储器地址
	DMARECVCHANNEL ->CNDTR = MAXUSART3BUFLEN;
	DMARECVCHANNEL ->CCR = 0x00000000;           //复位
	DMARECVCHANNEL ->CCR |= 0<<4;             //传输方向，从外设读数据到存储器
	DMARECVCHANNEL ->CCR |= 1<<5;               //执行循环操作
	DMARECVCHANNEL ->CCR |= 0<<6;             //外设地址不递增
	DMARECVCHANNEL ->CCR |= 1<<7;               //存储器地址递增
	DMARECVCHANNEL ->CCR |= 0<<8;               //外设数据宽度8BIT
	DMARECVCHANNEL ->CCR |= 0<<10;              //存储数据宽度8bit
	DMARECVCHANNEL ->CCR |= 1<<12;              //中等优先级
	DMARECVCHANNEL ->CCR |= 0<<14;            //非存储器到存储器 
	DMARECVCHANNEL ->CCR |= 0x01;          //通道使能 
	
	RCC->APB2ENR |= 1<<2;   	//使能PORTA口时钟  
 	GPIOA->CRH &= 0XFFFFF00F;	//IO状态设置
	GPIOA->CRH |= 0X000008B0;	//IO状态设置 
	
	RCC->APB2ENR |= 1<<14;  	//使能串口时钟 	 
	RCC->APB2RSTR |= 1<<14;   //复位串口3
	RCC->APB2RSTR &= ~(1<<14);//停止复位	
	//波特率设置
 	USART1->BRR=mantissa;// 波特率设置	 
	
	USART1->CR3 |= 0xC0; 	//使能串口DMA收发
	USART1->CR1 |= 0X200C;  	//1位停止,无校验位.
	//使能接收中断 
	USART1->CR1 |= 1<<4;    	//idle中断	    	
	MY_NVIC_Init(0,1,USART1_IRQn,2);//组2
}

void COM1SendBuf(u8 *buf,u16 len)
{
	BufCopy(&Com1.TXBUF[0],buf,len);
	DMA1->IFCR = 0x0FFFFFFF;
	DMASENDCHANNEL ->CCR &= ~0x01;
	DMASENDCHANNEL ->CPAR = (u32)&(USART1->DR);  //外设地址
	DMASENDCHANNEL ->CMAR = (u32)&Com1.TXBUF[0];//存储器地址
	DMASENDCHANNEL ->CNDTR = len;
	DMASENDCHANNEL ->CCR |= 0x01;
}

void COM1SendBuf_Enter(u8 *buf,u16 len)
{
	BufCopy(&Com1.TXBUF[0],buf,len);
	Com1.TXBUF[len] = 0x0D;
	Com1.TXBUF[len+1] = 0x0A;
	DMA1->IFCR = 0x0FFFFFFF;
	DMASENDCHANNEL ->CCR &= ~0x01;
	DMASENDCHANNEL ->CPAR = (u32)&(USART1->DR);  //外设地址
	DMASENDCHANNEL ->CMAR = (u32)&Com1.TXBUF[0];//存储器地址
	DMASENDCHANNEL ->CNDTR = len+2;
	DMASENDCHANNEL ->CCR |= 0x01;
}

void USART1_IRQHandler(void)
{
	u16 temp;
	if(USART1 -> SR & 0x10)    // 空闲中断 , 先关闭DMA通道，清零计数，在打开
	{
		temp = MAXUSART3BUFLEN - DMARECVCHANNEL->CNDTR;
		Com1.RxCnt = temp;
		Com1.Status = COM1RECV;
		temp |= USART1->DR;
		DMARECVCHANNEL ->CCR &= ~0x01;   
		DMARECVCHANNEL ->CNDTR = MAXUSART3BUFLEN;
		DMARECVCHANNEL ->CCR |=  0x01; 
	}
}

void COM1Task(void)
{
	switch(Com1.Status)
	{
		case COM1INIT:
			usart1_init(72,460800);
			COM1AddObserver((RecvFunction)COM1TEST);
			Com1.Status = COM1IDLE;
			break;
		
		case COM1IDLE:
			break;
		
		case COM1RECV:
			COM1NoticeAllObserver();
			Com1.Status = COM1IDLE;
			break;
	}
}
