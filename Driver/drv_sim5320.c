/*
 * drv_sim5320.c
 *
 *  Created on: 2016��1��20��
 *      Author: Sujunqin
 */
#include "drv_sim5320.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

#define DEBUG

//GSM����
#define   AT_CMD_REPEAT_TIMES  10 //�ȴ��ظ�ʱ��
struct GPS_INFO {
	float lat; //����
	/*0 - north, 1 - south*/
	u8 NS_Indicator; //�ϱ�����
	float log; //����
	/*0 - east, 1 - west*/
	u8 EW_Indicator; //��������
	u16 date; //Date. Output format is ddmmyy
	float UTCtime; //UTC Time. Output format is hhmmss.s
	float alt; //MSL Altitude. Unit is meters.
	float speed; //Speed Over Ground. Unit is knots.
	float course; //Course. Degrees
	u8 time; //range is 0-255, unit is second, after set <time> will report the GPS information every the seconds.
} gps_info;

/*0-δ���ӣ�1-������*/
u8 network_connect_flag = 0; //GPRS����״̬

/*0-δ�򿪣�1-�Ѵ�*/
u8 gps_flag = 0; //GPS����/�ر�״̬

const u8 AT[] = "AT+"; //��������
const u8 *modetbl[2] = { "TCP", "UDP" }; //���ӷ�ʽ
const u8 port[] = "7015"; //�˿�
const u8 ipbuf[] = "120.27.100.18"; //IP��ַ

const u8 MAX_RECEIVE_LEN = 200;
u8 receive_buf[MAX_RECEIVE_LEN];

void Sim5320_Receive_Data(u8 *buf, u8 *len) {
	USART1_Receive_Data(buf, len);
}

/**************************************
 ** �������� ��������ջ�����
 ** ����˵�� ��
 ** ��ڲ��� ��
 ** ���ڲ��� ��
 **************************************/
void CLR_RBUF(void) {
	u16 i;
	for (i = 0; i < MAX_RECEIVE_LEN; i++) {
		receive_buf[i] = 0x00;
	}
}

/**************************************
 ** �������� ������ָ�������ַ���������
 ** ����˵�� ��
 ** ��ڲ��� ���ַ������ַ�����
 ** ���ڲ��� ��
 **************************************/
void SendToGsm(char* p, u8 len) {
	Usart_Send_Data(SIM5320_USART, (u8*) p, len);
}

/**
 * @Description: ����ָ����SIM5320
 * @param cmd:�����ַ���, answer:�ڴ��ظ��ַ����� wait_time:����ʧ��ʱ���ȴ��ط�ʱ��
 * @return 0-����ʧ�ܣ�1-���ͳɹ�
 * @author Sujunqin
 */
u8 send_cmd(char * cmd, const char* answer, u16 wait_ime) {
	u8 i = AT_CMD_REPEAT_TIMES, len;
	char *p;
#ifdef DEBUG
	printf("Send to Sim5320:%s", cmd);
#endif
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		SendToGsm(cmd, strlen(cmd));
		//�ȴ�Ӧ��"OK"
		delay_ms(wait_ime);
		Sim5320_Receive_Data(receive_buf, &len);
		p = strstr((const char *) receive_buf, answer);
		if (p != NULL) {
			return 1;
		}
	}
	return 0;
}

/***********************��������ָ��AT******************************
 *��    ��: ���ڷ����������GPRSģ�飬"AT",	 ����OK
 *��    ��:
 *�� �� ֵ:
 *��    ע: ����GSMģ���Ƿ�������ȷ
 *****************************************************************/
void Send_AT(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "%s\r\n", "AT");
	result = send_cmd(cmd, "OK", 500);

#ifdef DEBUG
	if (result == 1) {
		printf("Send_AT Reiceive OK\r\n");
	} else {
		printf("Send_AT fail to reiceive OK\r\n");
	}
#endif
}

/***********************����ȡ������ָ��******************************
 *��    ��: ���ڷ����������SIM5320��ATE0
 *��    ��:
 *�� �� ֵ:
 *��    ע: �����޻��Թ���
 *****************************************************************/
void Send_ATE0(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "%s\r\n", "ATE0");
	result = send_cmd(cmd, "OK", 500);

#ifdef DEBUG
	if (result == 1) {
		printf("Send_ATE0 Reiceive OK\r\n");
	} else {
		printf("Send_ATE0 fail to reiceive OK\r\n");
	}
#endif
}

/***********************����ע��״̬******************************
 *��    ��: ���ڷ����������SIM5320��"AT+CREG?",
 *��    ��:
 *�� �� ֵ:
 *��    ע: ��ѯ����ע��״+CREG: 0,2//ûע�� +CREG: 0,1//ע������
 *****************************************************************/
void Check_Net_Register(void) {
	char cmd[50];
	char *p;
	u8 result = 0;
	sprintf(cmd, "%s%s\r\n", AT, "CREG?");
	result = send_cmd(cmd, "OK", 500);

#ifdef DEBUG
	if (result == 1) {
		printf("Check_Net_Register Reiceive OK\r\n");
		p = strstr((const char*) receive_buf, "+CREG: 0,1");
		if (p != NULL) {
			printf("sim has register\r\n");
		}
		p = strstr((const char*) receive_buf, "+CREG: 0,2");
		if (p != NULL) {
			printf("sim not register\r\n");
		}
	} else {
		printf("Check_Net_Register fail to reiceive OK\r\n");
	}
#endif
}

/***********************����������******************************
 *��    ��: ���ڷ����������SIM5320E��AT+IPR=9600
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_IPR9600(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "%s%s\r\n", AT, "IPR=9600");
	result = send_cmd(cmd, "OK", 500);

#ifdef DEBUG
	if (result == 1) {
		printf("Set_IPR9600 Reiceive OK\r\n");
	} else {
		printf("Set_IPR9600 fail to reiceive OK\r\n");
	}
#endif
}

/***********************����������******************************
 *��    ��: ���ڷ����������SIM5320E��AT+IPR=115200
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_IPR115200(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "%s%s\r\n", AT, "IPR=115200");
	result = send_cmd(cmd, "OK", 500);

#ifdef DEBUG
	if (result == 1) {
		printf("Set_IPR115200 Reiceive OK\r\n");
	} else {
		printf("Set_IPR115200 fail to reiceive OK\r\n");
	}
#endif
}

/***********************�鿴����GPRS���******************************
 *��    ��: ���ڷ����������SIM5320E��AT+CGATT?
 *��    ��:
 *�� �� ֵ:+CGATT: 1 //��ע��
 *��    ע:
 *****************************************************************/
void Check_Packet_Domain_Attach(void) {
	char cmd[50];
	char *p;
	u8 result = 0;
	sprintf(cmd, "%s%s\r\n", AT, "CGATT?");
	result = send_cmd(cmd, "OK", 500);

#ifdef DEBUG
	if (result == 1) {
		printf("Check_Packet_Domain_Attach Reiceive OK\r\n");
		p = strstr((const char*) receive_buf, "+CGATT: 1");
		if (p != NULL) {
			printf("sim has Domain_Attach\r\n");
		}
		p = strstr((const char*) receive_buf, "+CGATT: 0");
		if (p != NULL) {
			printf("sim not Domain_Attach\r\n");
		}
	} else {
		printf("Check_Packet_Domain_Attach fail to reiceive OK\r\n");
	}
#endif
}

/***********************����APN****************************
 *��    ��: ���ڷ����������SIM5320E�� AT+CGSOCKCONT=1,"IP","3gnet"
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_APN(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "AT+CGSOCKCONT=1,\"IP\",\"3gnet\"\r\n");
	result = send_cmd(cmd, "OK", 2000);

#ifdef DEBUG
	if (result == 1) {
		printf("Set_APN Reiceive OK\r\n");
	} else {
		printf("Set_APN fail to reiceive OK\r\n");
	}
#endif
}

/***********************������******************************
 *��    ��: ���ڷ����������SIM5320E�� OPNET[]="NETOPEN="TCP",56302;//������
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void OPEN_NET(void) {
	char cmd[50];
	char *p;
	u8 result = 0;
	sprintf(cmd, "AT+NETOPEN=\"%s\",%s\r\n", modetbl[0], port);
	result = send_cmd(cmd, "opened", 2000);

#ifdef DEBUG
	if (result == 1) {
		printf("OPEN_NET Reiceive OK\r\n");
		p = strstr((const char*) receive_buf, "Network opened");
		if (p != NULL) {
			printf("OPEN_NET Network opened\r\n");
		}
		p = strstr((const char*) receive_buf, "Network is already opened");
		if (p != NULL) {
			printf("OPEN_NET Network is already opened\r\n");
		}
	} else {
		printf("OPEN_NET fail to reiceive OK\r\n");
	}
#endif
}

/***********************���ӷ�����******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+TCPCONNECT="220.170.79.231",56302;//���ӷ�����
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void CONNECT_SEV(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "AT+TCPCONNECT=\"%s\",%s\r\n", ipbuf, port);
	result = send_cmd(cmd, "OK", 2000);

	if (result == 1) {
		network_connect_flag = 1;
#ifdef DEBUG
		printf("CONNECT_SEV Reiceive OK\r\n");
#endif
	} else {
		network_connect_flag = 0;
#ifdef DEBUG
		printf("CONNECT_SEV fail to reiceive OK\r\n");
#endif
	}
}

void Send_String_To_Server(char string[]) {
	u8 i = AT_CMD_REPEAT_TIMES, len;
	char *p;
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	if (network_connect_flag == 0) {
		CONNECT_SEV();
	} else {
		while (i--) {
			CLR_RBUF();
			sprintf(cmd, "%sTCPWRITE=%d\r\n", AT, strlen(string));
			SendToGsm(cmd, strlen(cmd));
#ifdef DEBUG
			printf("Send to Sim5320:%s", cmd);
#endif
			delay_ms(1000);
			SendToGsm(string, strlen(string));
			printf("Send to Sim5320:%s", string);
			delay_ms(2000);
			Sim5320_Receive_Data(receive_buf, &len);
			p = strstr((const char*) receive_buf, "Send ok");
			if (p != NULL) {
#ifdef DEBUG
				printf("Send_String_To_Server Reiceive OK\r\n");
#endif
				break;   //���յ�"OK"
			}
		}
	}
}

/***********************��������������ʾ****************************
 *��    ��: ���ڷ����������SIM5320E��   CIPHEAD[]="CIPHEAD=0";//��������������ʾ
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void CIPHEAD(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "%sCIPHEAD=0\r\n", AT);
	result = send_cmd(cmd, "OK", 1000);

#ifdef DEBUG
	if (result == 1) {
		printf("CIPHEAD Reiceive OK\r\n");
	} else {
		printf("CIPHEAD fail to reiceive OK\r\n");
	}
#endif
}

/***********************�ر���ʾԴ����IP��ַ�Ͷ˿�****************************
 *��    ��: ���ڷ����������SIM5320E��    CIPSRIP[]="CIPSRIP=0";//�ر���ʾԴ����IP��ַ�Ͷ˿�
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void CIPSRIP(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "%sCIPSRIP=0\r\n", AT);
	result = send_cmd(cmd, "OK", 1000);

#ifdef DEBUG
	if (result == 1) {
		printf("CIPSRIP Reiceive OK\r\n");
	} else {
		printf("CIPSRIP fail to reiceive OK\r\n");
	}
#endif
}

/***********************�ر���������******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+NETCLOSE
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Close_Network(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "AT+NETCLOSE\r\n");
	result = send_cmd(cmd, "OK", 2000);

	if (result == 1) {
		network_connect_flag = 0;
#ifdef DEBUG
		printf("Close_Network Reiceive OK\r\n");
#endif
	} else {
#ifdef DEBUG
		printf("Close_Network fail to reiceive OK\r\n");
#endif
	}
}

/***********************����GPS����******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+CGPS=1
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Open_GPS(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "AT+CGPS=1\r\n");
	result = send_cmd(cmd, "OK", 1000);

	if (result == 1) {
		gps_flag = 1;
#ifdef DEBUG
		printf("Open_GPS Reiceive OK\r\n");
#endif
	} else {
		gps_flag = 0;
#ifdef DEBUG
		printf("Open_GPS fail to reiceive OK\r\n");
#endif
	}
}

/***********************�ر�GPS����******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+CGPS=0
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Close_GPS(void) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "AT+CGPS=0\r\n");
	result = send_cmd(cmd, "OK", 1000);

	if (result == 1) {
		gps_flag = 0;
#ifdef DEBUG
		printf("Close_GPS Reiceive OK\r\n");
#endif
	} else {
#ifdef DEBUG
		printf("Close_GPS fail to reiceive OK\r\n");
#endif
	}
}

/***********************��ȡGPS��Ϣ******************************
 *��    ��: ���ڷ����������SIM5320E��AT+CGPSINFO
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Get_GPS_Info(char *info) {
	char cmd[50];
	u8 result = 0;
	sprintf(cmd, "AT+CGPSINFO\r\n");
	result = send_cmd(cmd, "OK", 2000);

	if (result == 1) {
		sprintf(info, "%s\r\n", receive_buf);
#ifdef DEBUG
		printf("Get_GPS_Info Reiceive OK\r\n");
#endif
	} else {
#ifdef DEBUG
		printf("Get_GPS_Info fail to reiceive OK\r\n");
#endif
	}
}

/*****************************************************
 *��    ��:��ȡ��������״̬
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
u8 Get_Connect_Flag(void) {
	return network_connect_flag;
}

/*****************************************************
 *��    ��:GPS������־
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
u8 Get_Gps_Statue_Flag(void) {
	return gps_flag;
}

/**
 * @Description:SIM5320ģ���ʼ��
 * @param
 * @return
 * @author Sujunqin
 */
void SIM5320_INT(void) {
	//�ȴ�SIM5320Ӳ����ʼ��
	int var = 0;
	for (var = 0; var < 5; ++var) {
		delay_ms(5000);
	}
	Set_IPR115200();
	Send_AT();
	Send_ATE0();
	Check_Net_Register();
	Check_Packet_Domain_Attach();
	Open_GPS();
	Set_APN();
	OPEN_NET();
	CONNECT_SEV();
	CIPHEAD();
	CIPSRIP();
}

