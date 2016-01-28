#include "sys.h"
#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
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

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE {
	int handle;

};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) {
	x = x + 0;
}
//�ض���fputc���� 
int fputc(int ch, FILE *f) {
	while ((USART2->SR & 0X40) == 0)
		; //ѭ������,ֱ���������
	USART2->DR = (u8) ch;
	return ch;
}
#endif 

const u16 UART1_MAX_RECEIVE_LEN = 200;
u16 UART1_RX_CNT = 0;
u8 UART1_RX_BUF[UART1_MAX_RECEIVE_LEN];  	//���ջ���,���64���ֽ�.

void uart1_init(u32 bound) {
	//GPIO�˿�����
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

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;	//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���

}

void USART1_IRQHandler(void) {
	u8 res;
	//���յ�����
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		res = USART_ReceiveData(USART1); 	//��ȡ���յ�������
		if (UART1_RX_CNT < UART1_MAX_RECEIVE_LEN) {
			UART1_RX_BUF[UART1_RX_CNT] = res;		//��¼���յ���ֵ
			UART1_RX_CNT++;						//������������1
		}
	}
}
/**
 *����1���պ���
 */
void USART1_Receive_Data(u8 *buf, u8 *len) {
	u8 rxlen = UART1_RX_CNT;
	u8 i = 0;
	*len = 0;				//Ĭ��Ϊ0
	delay_ms(2);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	//���յ�������,�ҽ��������
	if (rxlen == UART1_RX_CNT && rxlen) {
		for (i = 0; i < rxlen; i++) {
			buf[i] = UART1_RX_BUF[i];
		}
		*len = UART1_RX_CNT;	//��¼�������ݳ���
		UART1_RX_CNT = 0;		//����
	}
}

const u16 UART2_MAX_RECEIVE_LEN = 200;
u16 UART2_RX_CNT = 0;
u8 UART2_RX_BUF[UART2_MAX_RECEIVE_LEN];  	//���ջ���,���200���ֽ�.

void uart2_init(u32 bound) {
	//GPIO�˿�����
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

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;	//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//�����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���
}

void USART2_IRQHandler(void) {
	u8 res;
	//���յ�����
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		res = USART_ReceiveData(USART2); 	//��ȡ���յ�������
		if (UART2_RX_CNT < UART2_MAX_RECEIVE_LEN) {
			UART2_RX_BUF[UART2_RX_CNT] = res;		//��¼���յ���ֵ
			UART2_RX_CNT++;						//������������1
		}
	}
}

void USART2_Receive_Data(u8 *buf, u8 *len) {
	u8 rxlen = UART2_RX_CNT;
	u8 i = 0;
	*len = 0;				//Ĭ��Ϊ0
	delay_ms(2);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	//���յ�������,�ҽ��������
	if (rxlen == UART2_RX_CNT && rxlen) {
		for (i = 0; i < rxlen; i++) {
			buf[i] = UART2_RX_BUF[i];
		}
		*len = UART2_RX_CNT;	//��¼�������ݳ���
		UART2_RX_CNT = 0;		//����
	}
}

void Usart_Send_Data(USART_TypeDef* USARTx, u8 *buf, u8 len) {
	u8 t;
	if (USARTx == USART1) {
		//ѭ����������
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
		//ѭ����������
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
