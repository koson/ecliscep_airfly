#ifndef __USART_H
#define __USART_H
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/5/27
//�汾��V1.3
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
////////////////////////////////////////////////////////////////////////////////// 

#define USART_PRINTF USART1

void uart1_init(u32 bound);
void uart2_init(u32 bound);

u8 Usart_Send_Byte(USART_TypeDef* USARTx, u8 data);
void Usart_Send_Data(USART_TypeDef* USARTx, u8 *buf, u8 len);

void USART1_Receive_Data(u8 *buf, u8 *len);
void USART2_Receive_Data(u8 *buf, u8 *len);

#endif
