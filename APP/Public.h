#ifndef __PUBLIC_H
#define __PUBLIC_H

#include "sys.h"
#include "delay.h"
#include "ILI93xx.h"
#include "malloc.h"	    
#include "gui.h"
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "string.h"
#include "w25qxx.h" 
#include "Can.h"
#include "Usart1.h"
#include "WM.h"
#include "delay.h"
#include "touch.h" 
#include "Led.h"
#include "BUTTON.h"
#include "LISTWHEEL.h"
#include "MULTIEDIT.h"
#include "LISTVIEW.h"
#include "FRAMEWIN.h"
#include "TaskManger.h"
#include "EmWinHZFont.h"

void BufCopy(u8 *d,u8 * s,u16 len);
void BufSet(u8 *d,u8 val,u16 len);

#endif
