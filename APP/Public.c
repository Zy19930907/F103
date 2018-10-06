#include "Public.h"

void BufCopy(u8 *d,u8 * s,u16 len)
{
	do{
		*(d++) = *(s++);
	}while(--len);
}

void BufSet(u8 *d,u8 val,u16 len)
{
	do{
		*(d++) = val;
	}while(--len);
}

