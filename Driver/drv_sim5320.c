/*
 * drv_sim5320.c
 *
 *  Created on: 2016年1月20日
 *      Author: Sujunqin
 */
#include "drv_sim5320.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

#define DEBUG

//GSM设置
#define   AT_CMD_REPEAT_TIMES  10 //等待回复时间
struct GPS_INFO {
	float lat; //经度
	/*0 - north, 1 - south*/
	u8 NS_Indicator; //南北方向
	float log; //续度
	/*0 - east, 1 - west*/
	u8 EW_Indicator; //东西方向
	u16 date; //Date. Output format is ddmmyy
	float UTCtime; //UTC Time. Output format is hhmmss.s
	float alt; //MSL Altitude. Unit is meters.
	float speed; //Speed Over Ground. Unit is knots.
	float course; //Course. Degrees
	u8 time; //range is 0-255, unit is second, after set <time> will report the GPS information every the seconds.
} gps_info;

/*0-未连接，1-已连接*/
u8 network_connect_flag = 0; //GPRS连接状态

/*0-未打开，1-已打开*/
u8 gps_flag = 0; //GPS开启/关闭状态

const u8 AT[] = "AT+"; //联机命令
const u8 *modetbl[2] = { "TCP", "UDP" }; //连接方式
const u8 port[] = "7015"; //端口
const u8 ipbuf[] = "120.27.100.18"; //IP地址

const u8 MAX_RECEIVE_LEN = 200;
u8 receive_buf[MAX_RECEIVE_LEN];

void Sim5320_Receive_Data(u8 *buf, u8 *len) {
	USART1_Receive_Data(buf, len);
}

/**************************************
 ** 函数功能 ：清除接收缓冲区
 ** 函数说明 ：
 ** 入口参数 ：
 ** 出口参数 ：
 **************************************/
void CLR_RBUF(void) {
	u16 i;
	for (i = 0; i < MAX_RECEIVE_LEN; i++) {
		receive_buf[i] = 0x00;
	}
}

/**************************************
 ** 函数功能 ：发送指定长度字符串到串口
 ** 函数说明 ：
 ** 入口参数 ：字符串，字符长度
 ** 出口参数 ：
 **************************************/
void SendToGsm(char* p, u8 len) {
	Usart_Send_Data(SIM5320_USART, (u8*) p, len);
}

/**
 * @Description: 发送指定到SIM5320
 * @param cmd:命令字符串, answer:期待回复字符串， wait_time:发送失败时，等待重发时间
 * @return 0-发送失败，1-发送成功
 * @author Sujunqin
 */
u8 send_cmd(char * cmd, const char* answer, u16 wait_ime) {
	u8 i = AT_CMD_REPEAT_TIMES, len;
	char *p;
#ifdef DEBUG
	printf("Send to Sim5320:%s", cmd);
#endif
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		SendToGsm(cmd, strlen(cmd));
		//等待应答"OK"
		delay_ms(wait_ime);
		Sim5320_Receive_Data(receive_buf, &len);
		p = strstr((const char *) receive_buf, answer);
		if (p != NULL) {
			return 1;
		}
	}
	return 0;
}

/***********************发送联机指令AT******************************
 *功    能: 串口发送数组命令到GPRS模块，"AT",	 返回OK
 *形    参:
 *返 回 值:
 *备    注: 测试GSM模块是否连接正确
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

/***********************发送取消回显指令******************************
 *功    能: 串口发送数组命令到SIM5320，ATE0
 *形    参:
 *返 回 值:
 *备    注: 设置无回显功能
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

/***********************网络注册状态******************************
 *功    能: 串口发送数组命令到SIM5320，"AT+CREG?",
 *形    参:
 *返 回 值:
 *备    注: 查询网络注册状+CREG: 0,2//没注册 +CREG: 0,1//注册上了
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

/***********************波特率设置******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+IPR=9600
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************波特率设置******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+IPR=115200
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************查看附着GPRS情况******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+CGATT?
 *形    参:
 *返 回 值:+CGATT: 1 //已注册
 *备    注:
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

/***********************设置APN****************************
 *功    能: 串口发送数组命令到SIM5320E， AT+CGSOCKCONT=1,"IP","3gnet"
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************打开网络******************************
 *功    能: 串口发送数组命令到SIM5320E， OPNET[]="NETOPEN="TCP",56302;//打开网络
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************链接服务器******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+TCPCONNECT="220.170.79.231",56302;//链接服务器
 *形    参:
 *返 回 值:
 *备    注:
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
	//测试10次，在某一次成功就退出
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
				break;   //接收到"OK"
			}
		}
	}
}

/***********************开启接受数据显示****************************
 *功    能: 串口发送数组命令到SIM5320E，   CIPHEAD[]="CIPHEAD=0";//开启接受数据显示
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************关闭显示源数据IP地址和端口****************************
 *功    能: 串口发送数组命令到SIM5320E，    CIPSRIP[]="CIPSRIP=0";//关闭显示源数据IP地址和端口
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************关闭网络连接******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+NETCLOSE
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************开启GPS功能******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+CGPS=1
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************关闭GPS功能******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+CGPS=0
 *形    参:
 *返 回 值:
 *备    注:
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

/***********************获取GPS信息******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+CGPSINFO
 *形    参:
 *返 回 值:
 *备    注:
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
 *功    能:获取网络连接状态
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
u8 Get_Connect_Flag(void) {
	return network_connect_flag;
}

/*****************************************************
 *功    能:GPS开启标志
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
u8 Get_Gps_Statue_Flag(void) {
	return gps_flag;
}

/**
 * @Description:SIM5320模块初始化
 * @param
 * @return
 * @author Sujunqin
 */
void SIM5320_INT(void) {
	//等待SIM5320硬件初始化
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

