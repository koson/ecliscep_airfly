/*
 * drv_sim5320.c
 *
 *  Created on: 2016��1��20��
 *      Author: Sujunqin
 */
#include "drv_sim5320.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"

struct {
	float lat;

} gps_info;

u8 network_connect_flag = 0;
u8 gps_flag = 0;

u8 AT[] = "AT+"; //��������
u8 ATE0[] = "ATE0"; //ȡ������
u8 Creg_Mes[] = "CREG?";    //ע��
u8 TxtMode[] = "CMGF=1"; //AT+CMGF=1����ģʽ����Text;AT+CMGF=0 PDUģʽ
u8 PDUMode[] = "CMGF=0"; //AT+CMGF=1����ģʽ����Text;AT+CMGF=0 PDUģʽ
u8 Send_Mes[] = "CMGS=";  //������Ϣ
u8 IND_Mes[] = "CNMI=2,1";  //������Ϣ
u8 clear_Mes[] = "CMGD=1";  //������һ��λ����Ϣ
u8 get_Mes[] = "CMGR=1";  //������һ��λ����Ϣ
u8 MEG_CONST[] = "XI LAN HUA JI SHU ";  //��������
u8 MSG_PHO[] = "18312666591";  //���պ���
u8 IPR[] = "IPR=9600";  //�޸Ĳ�����Ϊ9600
u8 IPR1[] = "IPR=115200";  //�޸Ĳ�����Ϊ115200
u8 CGATT[] = "CGATT?";

const u8 *modetbl[2] = { "TCP", "UDP" };  //
const u8 port[] = "7015";	//
u8 ipbuf[] = "120.27.100.18"; 	//IP

const u8 MAX_RECEIVE_LEN = 200;
u8 receive_buf[MAX_RECEIVE_LEN];
u8 receive_buf_counter = 0;        //����1���ջ�����ָ��
u8 receive_buf_start = 0;          //����1���ջ�������ʼλ
u8 receive_buf_end = 0;            //����1���ջ���������λ

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
	receive_buf_counter = 0;
	receive_buf_start = 0;
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

/***********************�ַ�������********************************
 *��    ��: �����ַ���
 *��    ��: char *s, char *t ;��s�в���t
 *�� �� ֵ: s_temp(t��s�е�λ��)�ɹ�     0 ��ʧ�� ��
 *��    ע:
 *****************************************************************/
u8 *mystrstr(u8 *s, u8 *t) {
	u8 *s_temp; /*the s_temp point to the s*/
	u8 *m_temp; /*the mv_tmp used to move in the loop*/
	u8 *t_temp; /*point to the pattern string*/

	if (NULL == s || NULL == t)
		return NULL;

	/*s_temp point to the s string*/
	for (s_temp = s; *s_temp != '\0'; s_temp++) {
		/*the move_tmp used for pattern loop*/
		m_temp = s_temp;
		/*the pattern string loop from head every time*/
		for (t_temp = t; *t_temp == *m_temp; t_temp++, m_temp++)
			;
		/*if at the tail of the pattern string return s_tmp*/
		if (*t_temp == '\0')
			return s_temp;

	}
	return NULL;
}

/***********************��������ָ��AT******************************
 *��    ��: ���ڷ����������GPRSģ�飬"AT",	 ����OK
 *��    ��:
 *�� �� ֵ:
 *��    ע: ����GSMģ���Ƿ�������ȷ
 *****************************************************************/
void Send_AT(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "AT\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Send_AT Reiceive OK\r\n");
			break;
		}
	}
}

/***********************����ȡ������ָ��******************************
 *��    ��: ���ڷ����������SIM5320��ATE0
 *��    ��:
 *�� �� ֵ:
 *��    ע: �����޻��Թ���
 *****************************************************************/
void Send_ATE0(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s\r\n", ATE0);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Send_ATE0 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************����ע��״̬******************************
 *��    ��: ���ڷ����������SIM5320��"AT+CREG?",
 *��    ��:
 *�� �� ֵ:
 *��    ע: ��ѯ����ע��״+CREG: 0,2//ûע�� +CREG: 0,1//ע������
 *****************************************************************/
void Check_Net_Register(void) {
	u8 *p, i = 20, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, Creg_Mes);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Send_ATE0 Reiceive OK\r\n");
			p = mystrstr(receive_buf, "+CREG: 0,1");
			if (p != NULL) {
				printf("sim has register\r\n");
			}
			p = mystrstr(receive_buf, "+CREG: 0,2");
			if (p != NULL) {
				printf("sim not register\r\n");
			}
			break;
		}
	}
}

/***********************���ö���Ϣģʽ******************************
 *��    ��: ���ڷ����������SIM5320��AT+CMGF=1 or AT+CMGF=0
 *��    ��:    uchar m =1 textģʽ      m=0 PDUģʽ
 *�� �� ֵ:
 *��    ע:    1 TEXT
 *****************************************************************/
void Set_MODE(u8 m) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		if (m) {
			sprintf(cmd, "%s%s\r\n", AT, TxtMode);
			SendToGsm(cmd, strlen(cmd));
		} else {
			sprintf(cmd, "%s%s\r\n", AT, PDUMode);
			SendToGsm(cmd, strlen(cmd));
		}
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_MODE Reiceive OK\r\n");
			break;
		}
	}

}

/***********************���ö���Ϣģʽ******************************
 *��    ��: ���ڷ����������SIM5320��������Ϣ  AT+CNMI=2,1
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_CNMI(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, IND_Mes);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_CNMI Reiceive OK\r\n");
			break;
		}
	}
}

/***********************���ö���Ϣģʽ******************************
 *��    ��: ���ڷ����������SIM5320��������һ��λ����Ϣ  AT+CMGD=1
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_CMGD(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, clear_Mes);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_CMGD Reiceive OK\r\n");
			break;
		}
	}
}

/***********************����������******************************
 *��    ��: ���ڷ����������SIM5320E��AT+IPR=9600
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_IPR9600(void) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, IPR);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_IPR9600 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************����������******************************
 *��    ��: ���ڷ����������SIM5320E��AT+IPR=115200
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_IPR115200(void) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, IPR1);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(2000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_IPR115200 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************�鿴����GPRS���******************************
 *��    ��: ���ڷ����������SIM5320E��AT+CGATT?
 *��    ��:
 *�� �� ֵ:+CGATT: 1 //��ע��
 *��    ע:
 *****************************************************************/
void Check_Packet_Domain_Attach(void) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, CGATT);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_IPR115200 Reiceive OK\r\n");
			p = mystrstr(receive_buf, "+CGATT: 1");
			if (p != NULL) {
				printf("sim net has register\r\n");
			}
			p = mystrstr(receive_buf, "+CGATT: 0");
			if (p != NULL) {
				printf("sim net not register\r\n");
			}
			break;
		}
	}
}

/***********************����APN****************************
 *��    ��: ���ڷ����������SIM5320E�� AT+CGSOCKCONT=1,"IP","3gnet"
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_APN(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		sprintf(cmd, "AT+CGSOCKCONT=1,\"IP\",\"3gnet\"\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//�ȴ�Ӧ��"OK"
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_APN Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************������******************************
 *��    ��: ���ڷ����������SIM5320E�� OPNET[]="NETOPEN="TCP",56302;//������
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void OPEN_NET(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		sprintf(cmd, "AT+NETOPEN=\"%s\",%s\r\n", modetbl[0], port);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "Network opened");
		if (p != NULL) {
			printf("OPEN_NET Network opened\r\n");
			break;
		}
		p = mystrstr(receive_buf, "Network is already opened");
		if (p != NULL) {
			printf("OPEN_NET Network is already opened\r\n");
			break;
		}
	}
}

/***********************���ӷ�����******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+TCPCONNECT="220.170.79.231",56302;//���ӷ�����
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void CONNECT_SEV(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		sprintf(cmd, "AT+TCPCONNECT=\"%s\",%s\r\n", ipbuf, port);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(5000);
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "Connect ok");
		if (p != NULL) {
			network_connect_flag = 1;
			printf("CONNECT_SEV Connect ok\r\n");
			break;   //���յ�"OK"
		}
	}
}

void Send_String_To_Server(char string[]) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�

	if (network_connect_flag == 0) {
		CONNECT_SEV();
	} else {
		while (i--) {
			CLR_RBUF();
			sprintf(cmd, "%sTCPWRITE=%d\r\n", AT, strlen(string));
			SendToGsm(cmd, strlen(cmd));
			printf("Send to Sim5320:%s", cmd);
			delay_ms(2000);
			SendToGsm(string, strlen(string));
			printf("Send to Sim5320:%s", string);
			delay_ms(5000);
			Sim5320_Receive_Data(receive_buf, &len);
			p = mystrstr(receive_buf, "Send ok");
			if (p != NULL) {
				printf("Send_String_To_Server Reiceive OK\r\n");
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
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%sCIPHEAD=0\r\n", AT);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(2000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("CIPHEAD Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************�ر���ʾԴ����IP��ַ�Ͷ˿�****************************
 *��    ��: ���ڷ����������SIM5320E��    CIPSRIP[]="CIPSRIP=0";//�ر���ʾԴ����IP��ַ�Ͷ˿�
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void CIPSRIP(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//����10�Σ���ĳһ�γɹ����˳�
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%sCIPSRIP=0\r\n", AT);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(2000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("CIPSRIP Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************�ر���������******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+NETCLOSE
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Close_Network(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		sprintf(cmd, "AT+NETCLOSE\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "Network closed");
		if (p != NULL) {
			network_connect_flag = 0;
			printf("Close_Network Network closed\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************����GPS����******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+CGPS=1
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Open_GPS(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		sprintf(cmd, "AT+CGPS=1\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			gps_flag = 1;
			printf("Open_GPS OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************�ر�GPS����******************************
 *��    ��: ���ڷ����������SIM5320E�� AT+CGPS=0
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Close_GPS(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		sprintf(cmd, "AT+CGPS=0\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			gps_flag = 0;
			printf("Close_GPS OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************��ȡGPS��Ϣ******************************
 *��    ��: ���ڷ����������SIM5320E��AT+CGPSINFO
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Get_GPS_Info(char *info) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		sprintf(cmd, "AT+CGPSINFO\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			sprintf(info, "%s\r\n", receive_buf);
			printf("Get_GPS_Info OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************GPRS��ʼ��******************************
 *��    ��:
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void GPRS_INT(void) {
	delay_ms(5000);
	delay_ms(5000);
	delay_ms(5000);
	delay_ms(5000);
	delay_ms(5000);   //�ȴ�SIM5320Ӳ����ʼ��
//	Set_IPR115200();
	Send_AT();		//AT��������
	Send_ATE0();	//ȡ������
//	Set_CNMI();		//�O�ö�����ʾ
//	Set_MODE(1);	//���ö��Ÿ�ʽ�ı�
	Check_Net_Register();		//��ѯ����ע��
	Check_Packet_Domain_Attach();
	Open_GPS();
	Set_APN();
	OPEN_NET();
	CONNECT_SEV();
	CIPHEAD();
	CIPSRIP();
//	delay_ms(200);
//	Send_String_To_Server("this is a test message");
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
