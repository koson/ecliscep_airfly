#ifndef __USART_H
#define __USART_H
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/5/27
//版本：V1.3
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
////////////////////////////////////////////////////////////////////////////////// 

#define USART_PRINTF USART1

void uart1_init(u32 bound);
void uart2_init(u32 bound);

u8 Usart_Send_Byte(USART_TypeDef* USARTx, u8 data);
void Usart_Send_Data(USART_TypeDef* USARTx, u8 *buf, u8 len);

void USART1_Receive_Data(u8 *buf, u8 *len);
void USART2_Receive_Data(u8 *buf, u8 *len);

#endif
