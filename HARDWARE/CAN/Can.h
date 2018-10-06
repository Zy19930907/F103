#ifndef CAN_H
#define CAN_H

#include "Public.h"

#define CANBAUD			5000
#define MAXCANOBSERVER	10
typedef  void(*CanRecvFunction)(u32 Id,u8 *buf,u8 len);

typedef enum
{
	CANINIT,
	CANIDLE,
}_CanStatus;

typedef struct
{
	_CanStatus Status;
    u8  Buf[8];
    u8  Len;
    u16 Tick[4];
    u32 ID;
	u32 ObserverFlag;
	CanRecvFunction RecvFunctions[MAXCANOBSERVER];
}_Can;

void Can1Init(void);
void Can1RecvFunc(void);
void CANTask(void);

#endif	/* CAN_H */

