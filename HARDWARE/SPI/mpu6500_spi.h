/*
 * mpu6500_spi.h
 *	@Description: TODO
 *  Created on: 2016��1��27��
 *      Author: Sujunqin
 */

#ifndef MPU6500_SPI_H_
#define MPU6500_SPI_H_
#include "sys.h"
#include "stdbool.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//SPI ��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#define	SPI_CS 		PAout(2)  //ѡ��FLASH
#define SPI_INT		PAin(3)

void SPIx_Init(void);			 //��ʼ��SPI��
void SPIx_SetSpeed(u8 SpeedSet); //����SPI�ٶ�
//u8 SPIx_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
bool SPIx_ReadWriteByte(u8 tData,u8 *rData);

#endif /* MPU6500_SPI_H_ */
