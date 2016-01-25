/*
 * module_test.c
 *	@Description: TODO
 *  Created on: 2016Äê1ÔÂ25ÈÕ
 *      Author: Sujunqin
 */
#include "board.h"

void sim5320_test(void) {
	u16 times = 0;
	char* p;
	u8 usart1_receive[200];
	u8 usart1_re_len;
	u8 usart2_receive[200];
	u8 usart2_re_len;
	char gps_info[100];
	while (1) {

		LED1 = 1;
		OSTimeDlyHMSM(0, 0, 0, 500);
		LED1 = 0;
		OSTimeDlyHMSM(0, 0, 0, 500);

		USART2_Receive_Data(usart2_receive, &usart2_re_len);
		Sim5320_Receive_Data(usart1_receive, &usart1_re_len);
		if (usart1_re_len > 0) {
			Usart_Send_Data(USART2, usart1_receive, usart1_re_len);
		}

		p = strstr((const char*) usart2_receive, "exit");
		if (p != NULL && Get_Connect_Flag() == 1) {
			Close_Network();
			Close_GPS();
		}

		p = strstr((const char*) usart2_receive, "connect");
		if (p != NULL && Get_Connect_Flag() == 0) {
			Open_GPS();
			OPEN_NET();
			CONNECT_SEV();
		}

		if (usart2_re_len > 0 && Get_Connect_Flag() == 1) {
			Send_String_To_Server((char*) usart2_receive);
		} else if (usart2_re_len > 0 && Get_Connect_Flag() == 0) {
			SendToGsm((char*) usart2_receive, usart2_re_len);
		}

		times++;
		if (times > 120) {
			times = 0;
			if (Get_Gps_Statue_Flag() == 1) {

				Get_GPS_Info(gps_info);
				Usart_Send_Data(USART2, (u8 *) gps_info,
						strlen((const char*) gps_info));
				if (Get_Connect_Flag() == 1) {
					Send_String_To_Server(gps_info);
				} else {
					CONNECT_SEV();
				}
			}
		}
	}
}
