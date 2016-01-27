/*
 * drv_cc1101.c
 *	@Description: cc1101驱动
 *  Created on: 2016年1月27日
 *      Author: Sujunqin
 */

#include "drv_cc1101.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "string.h"

#define DEBUG

//10, 7, 5, 0, -5, -10, -15, -20, dbm output power, 0x12 == -30dbm
u8 PaTabel[] = { 0xc0, 0xC8, 0x84, 0x60, 0x68, 0x34, 0x1D, 0x0E };

// Sync word qualifier mode = 30/32 sync word bits detected
// CRC autoflush = false
// Channel spacing = 199.951172
// Data format = Normal mode
// Data rate = 2.00224
// RX filter BW = 58.035714
// PA ramping = false
// Preamble count = 4
// Whitening = false
// Address config = No address check
// Carrier frequency = 400.199890
// Device address = 0
// TX power = 10
// Manchester enable = false
// CRC enable = true
// Deviation = 5.157471
// Packet length mode = Variable packet length mode. Packet length configured by the first byte after sync word
// Packet length = 255
// Modulation format = GFSK
// Base frequency = 399.999939
// Modulated = true
// Channel number = 1
// PA table
#define PA_TABLE {0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}

static const u8 CC1101InitData[22][2] = { { CC1101_IOCFG0, 0x06 }, {
CC1101_FIFOTHR, 0x47 }, { CC1101_PKTCTRL0, 0x05 }, { CC1101_CHANNR, 0x01 }, {
CC1101_FSCTRL1, 0x06 }, { CC1101_FREQ2, 0x0F }, { CC1101_FREQ1, 0x62 }, {
CC1101_FREQ0, 0x76 }, { CC1101_MDMCFG4, 0xF6 }, { CC1101_MDMCFG3, 0x43 }, {
CC1101_MDMCFG2, 0x13 }, { CC1101_DEVIATN, 0x15 }, { CC1101_MCSM0, 0x18 }, {
CC1101_FOCCFG, 0x16 }, { CC1101_WORCTRL, 0xFB }, {
CC1101_FSCAL3, 0xE9 }, { CC1101_FSCAL2, 0x2A }, { CC1101_FSCAL1, 0x00 }, {
CC1101_FSCAL0, 0x1F }, { CC1101_TEST2, 0x81 }, {
CC1101_TEST1, 0x35 }, { CC1101_MCSM1, 0x3B }, };

/*read a byte from the specified register*/
u8 GS_CC1101ReadReg(u8 addr);

/*Read some bytes from the rigisters continously*/
void GS_CC1101ReadMultiReg(u8 addr, u8 *buff, u8 size);

/*Write a byte to the specified register*/
void CC1101WriteReg(u8 addr, u8 value);

/*Flush the TX buffer of CC1101*/
void CC1101ClrTXBuff(void);

/*Flush the RX buffer of CC1101*/
void CC1101ClrRXBuff(void);

/*Get received count of CC1101*/
u8 CC1101GetRXCnt(void);

/*Reset the CC1101 device*/
void CC1101Reset(void);

/*Write some bytes to the specified register*/
void CC1101WriteMultiReg(u8 addr, u8 *buff, u8 size);

/*
 ================================================================================
 Function : SPI_ExchangeByte( )
 Initialize the spi send byte function of stm32
 INPUT    : None
 OUTPUT   : None
 ================================================================================
 */
u8 SPI_ExchangeByte(u8 input) {
	return SPI2_ReadWriteByte(input);
}

/*
 ================================================================================
 Function : GS_CC1101WORInit( )
 Initialize the WOR function of CC1101
 INPUT    : None
 OUTPUT   : None
 ================================================================================
 */
void GS_CC1101WORInit(void) {

	CC1101WriteReg(CC1101_MCSM0, 0x18);
	CC1101WriteReg(CC1101_WORCTRL, 0x78); //Wake On Radio Control
	CC1101WriteReg(CC1101_MCSM2, 0x00);
	CC1101WriteReg(CC1101_WOREVT1, 0x8C);
	CC1101WriteReg(CC1101_WOREVT0, 0xA0);

	CC1101WriteCmd( CC1101_SWORRST);
}
/*
 ================================================================================
 Function : GS_CC1101ReadReg( )
 read a byte from the specified register
 INPUT    : addr, The address of the register
 OUTPUT   : the byte read from the rigister
 ================================================================================
 */
u8 GS_CC1101ReadReg(u8 addr) {
	u8 i;
	CC_CSN_LOW( )
	;
	SPI_ExchangeByte(addr | READ_SINGLE);
	i = SPI_ExchangeByte(0xFF);
	CC_CSN_HIGH( );
	return i;
}
/*
 ================================================================================
 Function : GS_CC1101ReadMultiReg( )
 Read some bytes from the rigisters continously
 INPUT    : addr, The address of the register
 buff, The buffer stores the data
 size, How many bytes should be read
 OUTPUT   : None
 ================================================================================
 */
void GS_CC1101ReadMultiReg(u8 addr, u8 *buff, u8 size) {
	u8 i, j;
	CC_CSN_LOW( )
	;
	SPI_ExchangeByte(addr | READ_BURST);
	for (i = 0; i < size; i++) {
		for (j = 0; j < 20; j++)
			;
		*(buff + i) = SPI_ExchangeByte(0xFF);
	}
	CC_CSN_HIGH( );
}
/*
 ================================================================================
 Function : CC1101ReadStatus( )
 Read a status register
 INPUT    : addr, The address of the register
 OUTPUT   : the value read from the status register
 ================================================================================
 */
u8 CC1101ReadStatus(u8 addr) {
	u8 i;
	CC_CSN_LOW( )
	;
	SPI_ExchangeByte(addr | READ_BURST);
	i = SPI_ExchangeByte(0xFF);
	CC_CSN_HIGH( );
	return i;
}
/*
 ================================================================================
 Function : CC1101SetTRMode( )
 Set the device as TX mode or RX mode
 INPUT    : mode selection
 OUTPUT   : None
 ================================================================================
 */
void CC1101SetTRMode(TRMODE mode) {
	if (mode == TX_MODE) {
		CC1101WriteReg(CC1101_IOCFG0, 0x46);
		CC1101WriteCmd( CC1101_STX);
	} else if (mode == RX_MODE) {
		CC1101WriteReg(CC1101_IOCFG0, 0x46);
		CC1101WriteCmd( CC1101_SRX);
	}
}
/*
 ================================================================================
 Function : CC1101WriteReg( )
 Write a byte to the specified register
 INPUT    : addr, The address of the register
 value, the byte you want to write
 OUTPUT   : None
 ================================================================================
 */
void CC1101WriteReg(u8 addr, u8 value) {
	CC_CSN_LOW( )
	;
	SPI_ExchangeByte(addr);
	SPI_ExchangeByte(value);
	CC_CSN_HIGH( );
}
/*
 ================================================================================
 Function : CC1101WriteMultiReg( )
 Write some bytes to the specified register
 INPUT    : addr, The address of the register
 buff, a buffer stores the values
 size, How many byte should be written
 OUTPUT   : None
 ================================================================================
 */
void CC1101WriteMultiReg(u8 addr, u8 *buff, u8 size) {
	u8 i;
	CC_CSN_LOW( )
	;
	SPI_ExchangeByte(addr | WRITE_BURST);
	for (i = 0; i < size; i++) {
		SPI_ExchangeByte(*(buff + i));
	}
	CC_CSN_HIGH( );
}
/*
 ================================================================================
 Function : CC1101WriteCmd( )
 Write a command byte to the device
 INPUT    : command, the byte you want to write
 OUTPUT   : None
 ================================================================================
 */
void CC1101WriteCmd(u8 command) {
	CC_CSN_LOW( )
	;
	SPI_ExchangeByte(command);
	CC_CSN_HIGH( );
}
/*
 ================================================================================
 Function : CC1101Reset( )
 Reset the CC1101 device
 INPUT    : None
 OUTPUT   : None
 ================================================================================
 */
void CC1101Reset(void) {
	u8 x;

	CC_CSN_HIGH( );
	CC_CSN_LOW( )
	;
	CC_CSN_HIGH( );
	for (x = 0; x < 100; x++)
		;
	CC1101WriteCmd( CC1101_SRES);
}
/*
 ================================================================================
 Function : CC1101SetIdle( )
 Set the CC1101 into IDLE mode
 INPUT    : None
 OUTPUT   : None
 ================================================================================
 */
void CC1101SetIdle(void) {
	CC1101WriteCmd(CC1101_SIDLE);
}
/*
 ================================================================================
 Function : CC1101ClrTXBuff( )
 Flush the TX buffer of CC1101
 INPUT    : None
 OUTPUT   : None
 ================================================================================
 */
void CC1101ClrTXBuff(void) {
	CC1101SetIdle(); //MUST BE IDLE MODE
	CC1101WriteCmd( CC1101_SFTX);
}
/*
 ================================================================================
 Function : CC1101ClrRXBuff( )
 Flush the RX buffer of CC1101
 INPUT    : None
 OUTPUT   : None
 ================================================================================
 */
void CC1101ClrRXBuff(void) {
	CC1101SetIdle(); //MUST BE IDLE MODE
	CC1101WriteCmd( CC1101_SFRX);
}
/*
 ================================================================================
 Function : CC1101SendPacket( )
 Send a packet
 INPUT    : txbuffer, The buffer stores data to be sent
 size, How many bytes should be sent
 mode, Broadcast or address check packet
 OUTPUT   : None
 ================================================================================
 */
void CC1101SendPacket(u8 *txbuffer, u8 size, TX_DATA_MODE mode) {
	u8 address;
	if (mode == BROADCAST) {
		address = 0;
	} else if (mode == ADDRESS_CHECK) {
		address = GS_CC1101ReadReg( CC1101_ADDR);
	}

	CC1101ClrTXBuff();

	if ((GS_CC1101ReadReg( CC1101_PKTCTRL1) & ~0x03) != 0) {
		CC1101WriteReg( CC1101_TXFIFO, size + 1);
		CC1101WriteReg( CC1101_TXFIFO, address);
	} else {
		CC1101WriteReg( CC1101_TXFIFO, size);
	}

	CC1101WriteMultiReg( CC1101_TXFIFO, txbuffer, size);
	CC1101SetTRMode(TX_MODE);
	while (GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_2) != 0)
		;
	while (GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_2) == 0)
		;

	CC1101ClrTXBuff();
}
/*
 ================================================================================
 Function : CC1101GetRXCnt( )
 Get received count of CC1101
 INPUT    : None
 OUTPUT   : How many bytes hae been received
 ================================================================================
 */
u8 CC1101GetRXCnt(void) {
	return (CC1101ReadStatus( CC1101_RXBYTES) & BYTES_IN_RXFIFO);
}
/*
 ================================================================================
 Function : CC1101SetAddress( )
 Set the address and address mode of the CC1101
 INPUT    : address, The address byte
 AddressMode, the address check mode
 OUTPUT   : None
 ================================================================================
 */
void CC1101SetAddress(u8 address, ADDR_MODE AddressMode) {
	u8 btmp = GS_CC1101ReadReg( CC1101_PKTCTRL1) & ~0x03;
	CC1101WriteReg(CC1101_ADDR, address);
	if (AddressMode == BROAD_ALL) {
	} else if (AddressMode == BROAD_NO) {
		btmp |= 0x01;
	} else if (AddressMode == BROAD_0) {
		btmp |= 0x02;
	} else if (AddressMode == BROAD_0AND255) {
		btmp |= 0x03;
	}
}
/*
 ================================================================================
 Function : CC1101SetSYNC( )
 Set the SYNC bytes of the CC1101
 INPUT    : sync, 16bit sync
 OUTPUT   : None
 ================================================================================
 */
void CC1101SetSYNC(u16 sync) {
	CC1101WriteReg(CC1101_SYNC1, 0xFF & (sync >> 8));
	CC1101WriteReg(CC1101_SYNC0, 0xFF & sync);
}
/*
 ================================================================================
 Function : CC1101RecPacket( )
 Receive a packet
 INPUT    : rxBuffer, A buffer store the received data
 OUTPUT   : 1:received count, 0:no data
 ================================================================================
 */
u8 CC1101RecPacket(u8 *rxBuffer) {
	u8 status[2];
	u8 pktLen;

	if (CC1101GetRXCnt() != 0) {
		pktLen = GS_CC1101ReadReg(CC1101_RXFIFO);           // Read length byte
		if ((GS_CC1101ReadReg( CC1101_PKTCTRL1) & ~0x03) != 0) {
			GS_CC1101ReadReg(CC1101_RXFIFO);
		}
		if (pktLen == 0) {
			return 0;
		} else {
			pktLen--;
		}
		GS_CC1101ReadMultiReg(CC1101_RXFIFO, rxBuffer, pktLen); // Pull data
		GS_CC1101ReadMultiReg(CC1101_RXFIFO, status, 2);   // Read  status bytes

		CC1101ClrRXBuff();

		if (status[1] & CRC_OK) {
			return pktLen;
		} else {
			return 0;
		}
	} else {
		return 0;
	}                               // Error
}
/*
 ================================================================================
 Function : CC1101Init( )
 Initialize the CC1101, User can modify it
 INPUT    : None
 OUTPUT   : None
 ================================================================================
 */
void CC1101Init(void) {
	volatile u8 i, j;

	CC1101Reset();

	for (i = 0; i < 22; i++) {
		CC1101WriteReg(CC1101InitData[i][0], CC1101InitData[i][1]);
	}
	CC1101SetAddress(0x05, BROAD_0AND255);
	CC1101SetSYNC(0x8799);
	CC1101WriteReg(CC1101_MDMCFG1, 0x72); //Modem Configuration

	CC1101WriteMultiReg(CC1101_PATABLE, PaTabel, 8);

	i = CC1101ReadStatus( CC1101_PARTNUM); //for test, must be 0x80
	i = CC1101ReadStatus( CC1101_VERSION); //for test, refer to the datasheet
}

/**
 * @Description: spi片选io初始化
 * @param
 * @return
 * @author Sujunqin
 */
void spi2_cs_io_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(CC_CSN_IO_PORT_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = CC_CSN_IO; //SPI CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_Init(CC_CSN_IO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(CC_CSN_IO_PORT, CC_CSN_IO);

	RCC_APB2PeriphClockCmd(READY_IO_PORT_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = READY_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_Init(READY_IO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(READY_IO_PORT, READY_IO);

	RCC_APB2PeriphClockCmd(RX_INTERUPT_IO_PORT_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = RX_INTERUPT_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_Init(RX_INTERUPT_IO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(RX_INTERUPT_IO_PORT, RX_INTERUPT_IO);
}

/**
 * @Description: 测试cc1101发送
 * @param
 * @return
 * @author Sujunqin
 */
void cc1101_tx_test(void) {
	char sendstring[] = { "www.ashining.com\r\n" };
	u8 *sendbuffer = sendstring;

	spi2_cs_io_init();
	SPI2_Init();
	CC1101Init();

	while (1) {
		//发送数据包，每发送一次，LED闪烁一次
		CC1101SendPacket(sendbuffer, strlen(sendstring), ADDRESS_CHECK);
		LED0 = !LED0;
		delay_ms(1000);
	}
}

/**
 * @Description: 测试cc1101接收
 * @param
 * @return
 * @author Sujunqin
 */
void cc1101_rx_test(void) {
	u8 rxBuffer[100];
	u8 i = 0;

	spi2_cs_io_init();
	SPI2_Init();
	CC1101Init();                        // Write RF settings to config reg
	CC1101SetTRMode(RX_MODE);

	while (1) {
		while (GPIO_ReadInputDataBit( GPIOA, RX_INTERUPT_IO) != 0)
			;
		while (GPIO_ReadInputDataBit( GPIOA, RX_INTERUPT_IO) == 0)
			;
		//如果接收到数据包，LED状态翻转一次
		i = CC1101RecPacket(rxBuffer);
		if (i != 0) {
#ifdef DEBUG
			printf("receive data:%s", rxBuffer);
#endif
		}
		LED0 = !LED0;
		delay_ms(1000);
	}
}
