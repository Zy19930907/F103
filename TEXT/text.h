#ifndef __TEXT_H__
#define __TEXT_H__	 
#include <stm32f10x.h>
#include "fontupd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32F103开发板V1
//汉字显示 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 					     
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);			//得到汉字的点阵码
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);					//在指定位置显示一个汉字
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);	//在指定位置显示一个字符串 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void showFont(u16 x,u16 y,u16 width,u8*str,u16 fontColor,u16 bkColor);
#endif
