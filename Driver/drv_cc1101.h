/*
 * drv_cc1101.h
 *	@Description: cc1101引脚定义，类型定义，接口说明
 *  Created on: 2016年1月27日
 *      Author: Sujunqin
 */

#ifndef DRV_CC1101_H_
#define DRV_CC1101_H_
#include "sys.h"
#include "drv_cc1101_reg.h"
#include "cc1101_spi.h"

/*
 ================================================================================
 ------------------------------Internal IMPORT functions-------------------------
 you must offer the following functions for this module
 1. u8 SPI_ExchangeByte( u8 input ); //SPI Send and Receive function
 2. CC_CSN_LOW( );                        //Pull down the CSN line
 3. CC_CSN_HIGH( );                       //Pull up the CSN Line
 ================================================================================
 */
u8 SPI_ExchangeByte(u8 input);

#define SPI_GPIO_PORT		GPIOB
#define GPIO_Pin_SO			GPIO_Pin_14
#define GPIO_Pin_SCLK		GPIO_Pin_13
#define GPIO_Pin_SI			GPIO_Pin_15

#define CC_CSN_IO_PORT_CLK	RCC_APB2Periph_GPIOC
#define READY_IO_PORT_CLK	RCC_APB2Periph_GPIOB
#define INTERUPT_IO_PORT_CLK	RCC_APB2Periph_GPIOB

#define CC_CSN_IO_PORT	GPIOC
#define READY_IO_PORT	GPIOB
#define INTERUPT_IO_PORT GPIOB

#define CC_CSN_IO	GPIO_Pin_9
//GOD2
#define READY_IO	GPIO_Pin_9
//GOD0
#define INTERUPT_IO	GPIO_Pin_8

#define CC_CSN_LOW()   GPIO_ResetBits( CC_CSN_IO_PORT, CC_CSN_IO )
#define CC_CSN_HIGH()  GPIO_SetBits( CC_CSN_IO_PORT, CC_CSN_IO )

/*
 ================================================================================
 -----------------------------------macro definitions----------------------------
 ================================================================================
 */
typedef enum {
	TX_MODE, RX_MODE
} TRMODE;
typedef enum {
	BROAD_ALL, BROAD_NO, BROAD_0, BROAD_0AND255
} ADDR_MODE;
typedef enum {
	BROADCAST, ADDRESS_CHECK
} TX_DATA_MODE;

/*
 ================================================================================
 -------------------------------------exported APIs------------------------------
 ================================================================================
 */

/*read a byte from the specified register*/
u8 CC1101ReadReg(u8 addr);

/*Read a status register*/
u8 CC1101ReadStatus(u8 addr);

/*Set the device as TX mode or RX mode*/
void CC1101SetTRMode(TRMODE mode);

/*Write a command byte to the device*/
void CC1101WriteCmd(u8 command);

/*Set the CC1101 into IDLE mode*/
void CC1101SetIdle(void);

/*Send a packet*/
void CC1101SendPacket(u8 *txbuffer, u8 size, TX_DATA_MODE mode);

/*Set the address and address mode of the CC1101*/
void CC1101SetAddress(u8 address, ADDR_MODE AddressMode);

/*Set the SYNC bytes of the CC1101*/
void CC1101SetSYNC(u16 sync);

/*Receive a packet*/
u8 CC1101RecPacket(u8 *rxBuffer);

/*Initialize the WOR function of CC1101*/
void CC1101WORInit(void);

/*Initialize the CC1101, User can modify it*/
void CC1101Init(void);

void cc1101_tx_test(void);
void cc1101_rx_test(void);
#endif /* DRV_CC1101_H_ */
