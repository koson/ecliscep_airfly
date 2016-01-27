/*
 * cc1101_spi.h
 *	@Description: TODO
 *  Created on: 2016年1月27日
 *      Author: Sujunqin
 */

#ifndef CC1101_SPI_H_
#define CC1101_SPI_H_
#include "sys.h"

void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度
u8 SPI2_ReadWriteByte(u8 TxData); //SPI总线读写一个字节

#endif /* CC1101_SPI_H_ */
