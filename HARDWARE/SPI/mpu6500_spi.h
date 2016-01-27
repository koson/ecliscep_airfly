/*
 * mpu6500_spi.h
 *	@Description: TODO
 *  Created on: 2016年1月27日
 *      Author: Sujunqin
 */

#ifndef MPU6500_SPI_H_
#define MPU6500_SPI_H_
#include "sys.h"
#include "stdbool.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SPI 驱动函数
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#define	SPI_CS 		PAout(2)  //选中FLASH
#define SPI_INT		PAin(3)

void SPIx_Init(void);			 //初始化SPI口
void SPIx_SetSpeed(u8 SpeedSet); //设置SPI速度
//u8 SPIx_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
bool SPIx_ReadWriteByte(u8 tData,u8 *rData);

#endif /* MPU6500_SPI_H_ */
