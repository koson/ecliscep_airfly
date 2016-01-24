/*
 * drv_sim5320.h
 *
 *  Created on: 2016Äê1ÔÂ20ÈÕ
 *      Author: Sujunqin
 */

#ifndef DRV_SIM5320_H_
#define DRV_SIM5320_H_
#include "sys.h"

#define   SIM5320_USART USART1

void SendToGsm(char* p, u8 len);
void Sim5320_Receive_Data(u8 *buf, u8 *len);

void Send_AT(void);
void Send_ATE0(void);
void Check_Net_Register(void);
void Set_MODE(u8 m);
void Set_CNMI(void);
void Set_CNMI(void);
void Set_CMGD(void);

void Set_IPR9600(void);
void Set_IPR115200(void);

void OPEN_NET(void);
void CONNECT_SEV(void);
void CIPHEAD(void);
void CIPSRIP(void);
void Check_Packet_Domain_Attach(void);
void Set_APN(void);
void Send_String_To_Server(char string[]);
void Close_Network(void);
u8 Get_Connect_Flag(void);

void GPRS_INT(void);
void Open_GPS(void);
void Close_GPS(void);
void Get_GPS_Info(char *info);
u8 Get_Gps_Statue_Flag(void);

#endif /* DRV_SIM5320_H_ */
