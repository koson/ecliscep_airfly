/*
 * drv_sim5320.h
 *
 *  Created on: 2016年1月20日
 *      Author: Sujunqin
 */

#ifndef DRV_SIM5320_H_
#define DRV_SIM5320_H_

#include "sys.h"

//GSM设置
#define   ATwaits  10 //等待发送时间
#define   Sendwaits 5
#define   SIM5320_USART USART1

void SendToGsm(u8* p, u8 len);
void Set_IPR115200(void);
void Sim5320_Receive_Data(u8 *buf, u8 *len);
void Send_AT(void);
void Send_ATE0(void);
void ASK_CREG(void);
void Set_MODE(u8 m);
void Set_CNMI(void);
void Set_CNMI(void);
void Set_CMGD(void);
void Set_IPR9600(void);
void Set_IPR115200(void);
void OPEN_NET(void);
void CONNECT_SEV(void);
void SEND_DATA10(void);
void SEND_DATA3200(void);
void CIPHEAD(void);
void CIPSRIP(void);
void TransmitText(u8 *dialnum, u8 *text);

#endif /* DRV_SIM5320_H_ */
