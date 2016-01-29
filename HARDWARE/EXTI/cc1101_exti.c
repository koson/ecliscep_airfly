/*
 * cc1101_exti.c
 *	@Description: cc1101外部中断接收
 *  Created on: 2016年1月29日
 *      Author: Sujunqin
 */
#include "cc1101_exti.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "drv_cc1101.h"

#define DEBUG

//外部中断0服务程序
//初始化PB8为中断输入.
void exti_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	//初始化PB8 上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIOA.13 中断线以及中断初始化配置
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);

	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2，
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;				//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI9_5_IRQHandler(void) {
	//检查指定的EXTI0线路触发请求发生与否
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
	EXTI_ClearITPendingBit(EXTI_Line8);  //清除EXTI0线路挂起位
}

