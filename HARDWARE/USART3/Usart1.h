#ifndef __USART1_H
#define	__USART1_H

#include "Public.h"

typedef  void(*RecvFunction)(u8 *buf,u16 len);
#define MAXUSART3OBSERVER	10
#define MAXUSART3BUFLEN		1024
#define DMARECVCHANNEL		DMA1_Channel5
#define DMASENDCHANNEL		DMA1_Channel4

typedef enum
{
	COM1INIT,
	COM1IDLE,
	COM1RECV,
}_COM1Status;

typedef struct
{
	_COM1Status Status;
	u8 TXBUF[MAXUSART3BUFLEN];
	u8 RXBUF[MAXUSART3BUFLEN];
	u16 RxCnt;
	u16 TxCnt;
	u32 ObsererFlag;
	RecvFunction UsartRecvDeal[MAXUSART3OBSERVER];
}_COM1;


void usart1_init(u32 pclk1,u32 bound);
void COM1SendBuf(u8 *buf,u16 len);
void COM1SendBuf_Enter(u8 *buf,u16 len);
void COM1AddObserver(RecvFunction function);
void COM1DelObserver(RecvFunction function);
void COM1Task(void);

#endif 
