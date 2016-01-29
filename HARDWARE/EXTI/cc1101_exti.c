/*
 * cc1101_exti.c
 *	@Description: cc1101�ⲿ�жϽ���
 *  Created on: 2016��1��29��
 *      Author: Sujunqin
 */
#include "cc1101_exti.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "drv_cc1101.h"

#define DEBUG

//�ⲿ�ж�0�������
//��ʼ��PB8Ϊ�ж�����.
void exti_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//��ʼ��PB8 ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIOA.13 �ж����Լ��жϳ�ʼ������
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);

	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;				//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI9_5_IRQHandler(void) {
	//���ָ����EXTI0��·�������������
	if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
#ifdef DEBUG
		printf("EXTI9_5_IRQHandler\r\n");
#endif
		if (getCurrentMode() == TX_MODE) {
			CC1101ClrTXBuff();
#ifdef DEBUG
			printf("send over\r\n");
#endif
		} else {
			RecPacket();
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line8);  //���EXTI0��·����λ
}

