/*
 * cc1101_spi.h
 *	@Description: TODO
 *  Created on: 2016��1��27��
 *      Author: Sujunqin
 */

#ifndef CC1101_SPI_H_
#define CC1101_SPI_H_
#include "sys.h"

void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI�ٶ�
u8 SPI2_ReadWriteByte(u8 TxData); //SPI���߶�дһ���ֽ�

#endif /* CC1101_SPI_H_ */
