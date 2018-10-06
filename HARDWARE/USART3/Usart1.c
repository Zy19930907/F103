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
//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������ 
void usart1_init(u32 pclk1,u32 bound)
{  	
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp = (float)(pclk1*1000000)/(bound*16);//�õ�USARTDIV
	mantissa = temp;				 //�õ���������
	fraction = (temp-mantissa)*16; //�õ�С������	 
    mantissa <<= 4;
	mantissa += fraction; 
	//DMA��������
	RCC ->AHBENR |= 0x01; //ʹ��DMAʱ��
	DMASENDCHANNEL ->CPAR = (u32)&(USART1->DR);  //�����ַ
	DMASENDCHANNEL ->CMAR = (u32)&Com1.TXBUF[0];//�洢����ַ
	DMASENDCHANNEL ->CCR = 0x00000000;         //��λ
	DMASENDCHANNEL ->CCR |= 1<<4;             //���䷽�򣬴Ӵ洢�����������������
	DMASENDCHANNEL ->CCR |= 0<<5;             //��ִ��ѭ������
	DMASENDCHANNEL ->CCR |= 0<<6;             //�����ַ������
	DMASENDCHANNEL ->CCR |= 1<<7;             //�洢����ַ����
	DMASENDCHANNEL ->CCR |= 0<<8;            //�������ݿ��8BIT
	DMASENDCHANNEL ->CCR |= 0<<10;           //�洢���ݿ��8bit
	DMASENDCHANNEL ->CCR |= 1<<12;           //�е����ȼ�
	DMASENDCHANNEL ->CCR |= 0<<14;            //�Ǵ洢�����洢��
	
	//DMA��������
	DMARECVCHANNEL ->CPAR = (u32)&(USART1->DR);         //�����ַ
	DMARECVCHANNEL ->CMAR = (u32)&Com1.RXBUF[0];         //�洢����ַ
	DMARECVCHANNEL ->CNDTR = MAXUSART3BUFLEN;
	DMARECVCHANNEL ->CCR = 0x00000000;           //��λ
	DMARECVCHANNEL ->CCR |= 0<<4;             //���䷽�򣬴���������ݵ��洢��
	DMARECVCHANNEL ->CCR |= 1<<5;               //ִ��ѭ������
	DMARECVCHANNEL ->CCR |= 0<<6;             //�����ַ������
	DMARECVCHANNEL ->CCR |= 1<<7;               //�洢����ַ����
	DMARECVCHANNEL ->CCR |= 0<<8;               //�������ݿ��8BIT
	DMARECVCHANNEL ->CCR |= 0<<10;              //�洢���ݿ��8bit
	DMARECVCHANNEL ->CCR |= 1<<12;              //�е����ȼ�
	DMARECVCHANNEL ->CCR |= 0<<14;            //�Ǵ洢�����洢�� 
	DMARECVCHANNEL ->CCR |= 0x01;          //ͨ��ʹ�� 
	
	RCC->APB2ENR |= 1<<2;   	//ʹ��PORTA��ʱ��  
 	GPIOA->CRH &= 0XFFFFF00F;	//IO״̬����
	GPIOA->CRH |= 0X000008B0;	//IO״̬���� 
	
	RCC->APB2ENR |= 1<<14;  	//ʹ�ܴ���ʱ�� 	 
	RCC->APB2RSTR |= 1<<14;   //��λ����3
	RCC->APB2RSTR &= ~(1<<14);//ֹͣ��λ	
	//����������
 	USART1->BRR=mantissa;// ����������	 
	
	USART1->CR3 |= 0xC0; 	//ʹ�ܴ���DMA�շ�
	USART1->CR1 |= 0X200C;  	//1λֹͣ,��У��λ.
	//ʹ�ܽ����ж� 
	USART1->CR1 |= 1<<4;    	//idle�ж�	    	
	MY_NVIC_Init(0,1,USART1_IRQn,2);//��2
}

void COM1SendBuf(u8 *buf,u16 len)
{
	BufCopy(&Com1.TXBUF[0],buf,len);
	DMA1->IFCR = 0x0FFFFFFF;
	DMASENDCHANNEL ->CCR &= ~0x01;
	DMASENDCHANNEL ->CPAR = (u32)&(USART1->DR);  //�����ַ
	DMASENDCHANNEL ->CMAR = (u32)&Com1.TXBUF[0];//�洢����ַ
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
	DMASENDCHANNEL ->CPAR = (u32)&(USART1->DR);  //�����ַ
	DMASENDCHANNEL ->CMAR = (u32)&Com1.TXBUF[0];//�洢����ַ
	DMASENDCHANNEL ->CNDTR = len+2;
	DMASENDCHANNEL ->CCR |= 0x01;
}

void USART1_IRQHandler(void)
{
	u16 temp;
	if(USART1 -> SR & 0x10)    // �����ж� , �ȹر�DMAͨ��������������ڴ�
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
