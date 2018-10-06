#include "ctiic.h"
#include "delay.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//4.3��/7����ݴ���������-IICͨ�Ų���	  
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved	 
//////////////////////////////////////////////////////////////////////////////////

//����I2C�ٶȵ���ʱ
void CT_Delay(void)
{
	delay_us(5);
} 
//���ݴ���оƬIIC�ӿڳ�ʼ��
void CT_IIC_Init(void)
{	
	RCC->APB2ENR |=	(0x11 << 3);//ʹ��PB��PFʱ��
	GPIOB->CRL &= 0xFFFFFF0F;
	GPIOB->CRL |= 0x00000030; //PB1�������
	GPIOB->ODR |= 1<<1;//PB1����ߵ�ƽ
	
	GPIOF->CRH &= 0xFFFFFF0F;
	GPIOF->CRH |= 0x00000030;
 	GPIOF->ODR |= 1<<9;	
}
//����IIC��ʼ�ź�
void CT_IIC_Start(void)
{
	CT_SDA_OUT();     //sda�����
	CT_IIC_SDA=1;	  	  
	CT_IIC_SCL=1;
	delay_us(30);
 	CT_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda�����
	CT_IIC_SCL=1;
	delay_us(30);
	CT_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	CT_Delay();
	CT_IIC_SDA=1;//����I2C���߽����ź�  
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	CT_SDA_IN();      //SDA����Ϊ����  
	CT_IIC_SDA=1;	   
	CT_IIC_SCL=1;
	CT_Delay();
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void CT_IIC_Ack(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=0;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}
//������ACKӦ��		    
void CT_IIC_NAck(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=1;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void CT_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	CT_SDA_OUT(); 	    
    CT_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	CT_Delay();
	for(t=0;t<8;t++)
    {              
        CT_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	      
		CT_IIC_SCL=1; 
		CT_Delay();
		CT_IIC_SCL=0;	
		CT_Delay();
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	CT_SDA_IN();//SDA����Ϊ����
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL=0; 	    	   
		CT_Delay();
		CT_IIC_SCL=1;	 
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//����nACK
	else CT_IIC_Ack(); //����ACK   
 	return receive;
}



























