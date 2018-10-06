#include "Can.h"

_Can Can;

void CanTest(u32 Id,u8 *buf,u8 len)
{
	COM1SendBuf(buf,len);
}

void CanAddObserver(CanRecvFunction function)
{
	u8 i;
	for(i=0;i<MAXCANOBSERVER;i++)
	{
		if((Can.ObserverFlag >> i) & 0x01)
			continue;
		Can.RecvFunctions[i] = function;
		Can.ObserverFlag |= (1<<i);
		return;
	}
}

void CanDelObserver(CanRecvFunction function)
{
	u8 i;
	for(i=0;i<MAXCANOBSERVER;i++)
	{
		if(!((Can.ObserverFlag >> i) & 0x01))
			continue;
		if(Can.RecvFunctions[i] == function)
			Can.ObserverFlag &= ~(1<<i);
	}
}

void CanNoticeAllObserver(void)
{
	u8 i;
	for(i=0;i<MAXCANOBSERVER;i++)
	{
		if(!((Can.ObserverFlag >> i) & 0x01))
			continue;
		Can.RecvFunctions[i](Can.ID,&Can.Buf[0],Can.Len);
	}
}

u32 CanBaudValue(u8 Pclk,u16 Baud)
{
	u32 BtrValue = 0;
	BtrValue += 0x00;
	BtrValue <<= 4;//��ͬ����Ծ���1tq
	BtrValue += 0x07;
	BtrValue <<= 4;//ʱ���2 8tq
	BtrValue += 0x07;
	BtrValue <<= 16;//ʱ���1 8tq
	BtrValue += (((((u32)Pclk*1000000)/17/Baud) -1) & 0x000003FF);
	return BtrValue;
}

void CAN_IO_Init(void)
{
	RCC->APB2ENR |= (1<<0);//ʹ�ܸ��ù���ʱ��
	RCC->APB2ENR |= (1<<2);//ʹ��GPIOAʱ��
	GPIOA->CRH &= 0xFFF00FFF;
	GPIOA->CRH |= 0x000B8000;
}

void CanRegisterConfig(void)
{     
	u8 i;
	
	RCC->APB1RSTR |= 1 << 25;//CANʱ�Ӹ�λ
	RCC->APB1RSTR &= ~(1 << 25);
	RCC->APB1ENR  |= 1 << 25;//CANʱ��ʹ��
    CAN1->MCR |= (1 << 0); //�������CAN����ģʽ
	delay_ms(5);
	while((CAN1->MSR & 0x00000001) != 0x00000001)
	{
		if(i++>200)
			return;
	}//�ȴ�����CAN����ģʽ
	i=0;
	CAN1->MCR &= ~(1<<1);//�˳�˯��ģʽ
	
	CAN1->BTR = CanBaudValue(36,CANBAUD);//������
     
	CAN1->FMR |= 1<<0;//����������ģʽ
	
	CAN1->FS1R |= 0x00000003;//��������1��2λ��32λ
	CAN1->FM1R &= ~0x00000003;//��������1��2����λģʽ
	CAN1->FFA1R &= ~0x00000001;
	CAN1->FFA1R |=0x00000002;//��������1����FIFO0����������2����FIFO1
		
	CAN1->sFilterRegister[0].FR1 = 0x00000000;
	CAN1->sFilterRegister[0].FR2 = 0x00000000;
	
	CAN1->sFilterRegister[1].FR1 = 0x00000000;
	CAN1->sFilterRegister[1].FR2 = 0x00000000;
	CAN1->FA1R |= 0x00000003;//���������1��2
	CAN1->FMR &= 0<<0;
	CAN1->MCR |= (1<<6);//����ģʽ�Զ��ָ�
	CAN1->MCR &= ~(1 << 0); //�������CAN����ģʽ
	delay_ms(5);
	while((CAN1->MSR & 0x00000001) != 0x00000000)
	{
		if(i++>200)
			return;
	}//�ȴ�����CAN����ģʽ
}

void Can1RecvFunc(void)
{
	u32 t;
	//CAN���ջ�����0
	if(CAN1->RF0R & 0x00000003)
	{
		do
		{
			Can.Len = (CAN1->sFIFOMailBox[0].RDTR & 0x0000000F);
			if(Can.Len >= 8)
				Can.Len = 8;
			t= (u32)0x1FFFFFFF & (CAN1->sFIFOMailBox[0].RIR >> 3);
			Can.ID = (t >> 18) + ((t&0x3FFFF)<<11);
			Can.Buf[0]=CAN1->sFIFOMailBox[0].RDLR&0xFF;
			Can.Buf[1]=(CAN1->sFIFOMailBox[0].RDLR>>8)&0xFF;
			Can.Buf[2]=(CAN1->sFIFOMailBox[0].RDLR>>16)&0xFF;
			Can.Buf[3]=(CAN1->sFIFOMailBox[0].RDLR>>24)&0xFF;    
			Can.Buf[4]= CAN1->sFIFOMailBox[0].RDHR&0xFF;
			Can.Buf[5]=(CAN1->sFIFOMailBox[0].RDHR>>8)&0xFF;
			Can.Buf[6]=(CAN1->sFIFOMailBox[0].RDHR>>16)&0xFF;
			Can.Buf[7]=(CAN1->sFIFOMailBox[0].RDHR>>24)&0xFF;
			CanNoticeAllObserver();			
			CAN1->RF0R |= (1 << 5);//�ͷ�FIFO0
			while(CAN1->RF0R  & 0x00000020);
		}while(CAN1->RF0R & 0x00000003);//FIFO0�л�����δ������
		if(CAN1->RF0R & 0x00000010)
			CAN1->RF0R &= ~(1 << 4);
		if(CAN1->RF0R & 0x00000008)
			CAN1->RF0R &= ~(1 << 3);
     }
	//CAN���ջ�����1
     if(CAN1->RF1R & 0x00000003)
     {
		do
		{
			Can.Len = (CAN1->sFIFOMailBox[1].RDTR & 0x0000000F);
			 if(Can.Len >= 8)
				Can.Len = 8;
			t = (u32)0x1FFFFFFF & (CAN1->sFIFOMailBox[1].RIR >> 3);
			Can.ID = (t >> 18) + ((t&0x3FFFF)<<11);
			Can.Buf[0]=CAN1->sFIFOMailBox[1].RDLR&0xFF;
			Can.Buf[1]=(CAN1->sFIFOMailBox[1].RDLR>>8)&0xFF;
			Can.Buf[2]=(CAN1->sFIFOMailBox[1].RDLR>>16)&0xFF;
			Can.Buf[3]=(CAN1->sFIFOMailBox[1].RDLR>>24)&0xFF;    
			Can.Buf[4]= CAN1->sFIFOMailBox[1].RDHR&0xFF;
			Can.Buf[5]=(CAN1->sFIFOMailBox[1].RDHR>>8)&0xFF;
			Can.Buf[6]=(CAN1->sFIFOMailBox[1].RDHR>>16)&0xFF;
			Can.Buf[7]=(CAN1->sFIFOMailBox[1].RDHR>>24)&0xFF; 
			CanNoticeAllObserver();
			CAN1->RF1R |= (1 << 5);//�ͷ�FIFO1
			while(CAN1->RF1R & 0x00000020);
		}while(CAN1->RF1R & 0x00000003);//FIFO1�л�����δ������
		if(CAN1->RF1R & 0x00000010)
			CAN1->RF1R &= ~(1 << 4);
		if(CAN1->RF1R & 0x00000008)
			CAN1->RF1R &= ~(1 << 3);
     }
}

void Can1Init(void)
{
	CAN_IO_Init();
	CanRegisterConfig();
}

void CANTask(void)
{
	switch(Can.Status)
	{
		case CANIDLE:
			Can1RecvFunc();
			break;
		
		case CANINIT:
			Can1Init();
			//CanAddObserver(CanTest);
			Can.Status = CANIDLE;
			break;
	}
}
