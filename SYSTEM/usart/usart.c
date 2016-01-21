#include "sys.h"
#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
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

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE {
	int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) {
	x = x + 0;
}
//重定义fputc函数 
int fputc(int ch, FILE *f) {
	while ((USART2->SR & 0X40) == 0)
		; //循环发送,直到发送完毕
	USART2->DR = (u8) ch;
	return ch;
}
#endif 

const u16 UART1_MAX_RECEIVE_LEN = 200;
u16 UART1_RX_CNT = 0;
u8 UART1_RX_BUF[UART1_MAX_RECEIVE_LEN];  	//接收缓冲,最大64个字节.

void uart1_init(u32 bound) {
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(
			RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,
			ENABLE);
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启中断
	USART_Cmd(USART1, ENABLE);                    //使能串口

}

void USART1_IRQHandler(void) {
	u8 res;
	//接收到数据
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		res = USART_ReceiveData(USART1); 	//读取接收到的数据
		if (UART1_RX_CNT < UART1_MAX_RECEIVE_LEN) {
			UART1_RX_BUF[UART1_RX_CNT] = res;		//记录接收到的值
			UART1_RX_CNT++;						//接收数据增加1
		}
	}
}
/**
 *串口1接收函数
 */
void USART1_Receive_Data(u8 *buf, u8 *len) {
	u8 rxlen = UART1_RX_CNT;
	u8 i = 0;
	*len = 0;				//默认为0
	delay_ms(2);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	//接收到了数据,且接收完成了
	if (rxlen == UART1_RX_CNT && rxlen) {
		for (i = 0; i < rxlen; i++) {
			buf[i] = UART1_RX_BUF[i];
		}
		*len = UART1_RX_CNT;	//记录本次数据长度
		UART1_RX_CNT = 0;		//清零
	}
}

const u16 UART2_MAX_RECEIVE_LEN = 200;
u16 UART2_RX_CNT = 0;
u8 UART2_RX_BUF[UART2_MAX_RECEIVE_LEN];  	//接收缓冲,最大200个字节.

void uart2_init(u32 bound) {
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//USART1_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启中断
	USART_Cmd(USART2, ENABLE);                    //使能串口
}

void USART2_IRQHandler(void) {
	u8 res;
	//接收到数据
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		res = USART_ReceiveData(USART2); 	//读取接收到的数据
		if (UART2_RX_CNT < UART2_MAX_RECEIVE_LEN) {
			UART2_RX_BUF[UART2_RX_CNT] = res;		//记录接收到的值
			UART2_RX_CNT++;						//接收数据增加1
		}
	}
}

void USART2_Receive_Data(u8 *buf, u8 *len) {
	u8 rxlen = UART2_RX_CNT;
	u8 i = 0;
	*len = 0;				//默认为0
	delay_ms(2);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	//接收到了数据,且接收完成了
	if (rxlen == UART2_RX_CNT && rxlen) {
		for (i = 0; i < rxlen; i++) {
			buf[i] = UART2_RX_BUF[i];
		}
		*len = UART2_RX_CNT;	//记录本次数据长度
		UART2_RX_CNT = 0;		//清零
	}
}

void Usart_Send_Data(USART_TypeDef* USARTx, u8 *buf, u8 len) {
	u8 t;
	if (USARTx == USART1) {
		//循环发送数据
		for (t = 0; t < len; t++) {
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
				;
			USART_SendData(USART1, buf[t]);
		}
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			;
		UART1_RX_CNT = 0;
	}

	if (USARTx == USART2) {
		//循环发送数据
		for (t = 0; t < len; t++) {
			while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
				;
			USART_SendData(USART2, buf[t]);
		}
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
			;
		UART2_RX_CNT = 0;
	}
}

u8 Usart_Send_Byte(USART_TypeDef* USARTx, u8 data) {
	if (USARTx == USART1) {
		while (!(USART1->SR & USART_FLAG_TXE))
			;
		USART1->DR = (data & 0x1FF);
	}

	if (USARTx == USART2) {
		while (!(USART2->SR & USART_FLAG_TXE))
			;
		USART2->DR = (data & 0x1FF);
	}
	return (data);
}
