/*
 * drv_sim5320.c
 *
 *  Created on: 2016年1月20日
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

u8 AT[] = "AT+"; //联机命令
u8 ATE0[] = "ATE0"; //取消回显
u8 Creg_Mes[] = "CREG?";    //注册
u8 TxtMode[] = "CMGF=1"; //AT+CMGF=1短信模式设置Text;AT+CMGF=0 PDU模式
u8 PDUMode[] = "CMGF=0"; //AT+CMGF=1短信模式设置Text;AT+CMGF=0 PDU模式
u8 Send_Mes[] = "CMGS=";  //发送消息
u8 IND_Mes[] = "CNMI=2,1";  //接收消息
u8 clear_Mes[] = "CMGD=1";  //消除第一个位置消息
u8 get_Mes[] = "CMGR=1";  //消除第一个位置消息
u8 MEG_CONST[] = "XI LAN HUA JI SHU ";  //短信内容
u8 MSG_PHO[] = "18312666591";  //接收号码
u8 IPR[] = "IPR=9600";  //修改波特率为9600
u8 IPR1[] = "IPR=115200";  //修改波特率为115200
u8 CGATT[] = "CGATT?";

const u8 *modetbl[2] = { "TCP", "UDP" };  //
const u8 port[] = "7015";	//
u8 ipbuf[] = "120.27.100.18"; 	//IP

const u8 MAX_RECEIVE_LEN = 200;
u8 receive_buf[MAX_RECEIVE_LEN];
u8 receive_buf_counter = 0;        //串口1接收缓冲区指针
u8 receive_buf_start = 0;          //串口1接收缓冲区起始位
u8 receive_buf_end = 0;            //串口1接收缓冲区结束位

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
	receive_buf_counter = 0;
	receive_buf_start = 0;
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

/***********************字符串查找********************************
 *功    能: 查找字符串
 *形    参: char *s, char *t ;在s中查找t
 *返 回 值: s_temp(t在s中的位置)成功     0 （失败 ）
 *备    注:
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

/***********************发送联机指令AT******************************
 *功    能: 串口发送数组命令到GPRS模块，"AT",	 返回OK
 *形    参:
 *返 回 值:
 *备    注: 测试GSM模块是否连接正确
 *****************************************************************/
void Send_AT(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "AT\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Send_AT Reiceive OK\r\n");
			break;
		}
	}
}

/***********************发送取消回显指令******************************
 *功    能: 串口发送数组命令到SIM5320，ATE0
 *形    参:
 *返 回 值:
 *备    注: 设置无回显功能
 *****************************************************************/
void Send_ATE0(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s\r\n", ATE0);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Send_ATE0 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************网络注册状态******************************
 *功    能: 串口发送数组命令到SIM5320，"AT+CREG?",
 *形    参:
 *返 回 值:
 *备    注: 查询网络注册状+CREG: 0,2//没注册 +CREG: 0,1//注册上了
 *****************************************************************/
void Check_Net_Register(void) {
	u8 *p, i = 20, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, Creg_Mes);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
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

/***********************设置短消息模式******************************
 *功    能: 串口发送数组命令到SIM5320，AT+CMGF=1 or AT+CMGF=0
 *形    参:    uchar m =1 text模式      m=0 PDU模式
 *返 回 值:
 *备    注:    1 TEXT
 *****************************************************************/
void Set_MODE(u8 m) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
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
		//等待应答"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_MODE Reiceive OK\r\n");
			break;
		}
	}

}

/***********************设置短消息模式******************************
 *功    能: 串口发送数组命令到SIM5320，接收消息  AT+CNMI=2,1
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Set_CNMI(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, IND_Mes);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_CNMI Reiceive OK\r\n");
			break;
		}
	}
}

/***********************设置短消息模式******************************
 *功    能: 串口发送数组命令到SIM5320，消除第一个位置消息  AT+CMGD=1
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Set_CMGD(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, clear_Mes);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_CMGD Reiceive OK\r\n");
			break;
		}
	}
}

/***********************波特率设置******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+IPR=9600
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Set_IPR9600(void) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, IPR);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
		delay_ms(500);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_IPR9600 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************波特率设置******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+IPR=115200
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Set_IPR115200(void) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, IPR1);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
		delay_ms(2000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_IPR115200 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************查看附着GPRS情况******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+CGATT?
 *形    参:
 *返 回 值:+CGATT: 1 //已注册
 *备    注:
 *****************************************************************/
void Check_Packet_Domain_Attach(void) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//测试10次，在某一次成功就退出
	while (i--) {
		CLR_RBUF();
		sprintf(cmd, "%s%s\r\n", AT, CGATT);
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
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

/***********************设置APN****************************
 *功    能: 串口发送数组命令到SIM5320E， AT+CGSOCKCONT=1,"IP","3gnet"
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Set_APN(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		sprintf(cmd, "AT+CGSOCKCONT=1,\"IP\",\"3gnet\"\r\n");
		SendToGsm(cmd, strlen(cmd));
		printf("Send to Sim5320:%s", cmd);
		//等待应答"OK"
		delay_ms(5000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");
		if (p != NULL) {
			printf("Set_APN Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}

/***********************打开网络******************************
 *功    能: 串口发送数组命令到SIM5320E， OPNET[]="NETOPEN="TCP",56302;//打开网络
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void OPEN_NET(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //测试10次，在某一次成功就退出
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

/***********************链接服务器******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+TCPCONNECT="220.170.79.231",56302;//链接服务器
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void CONNECT_SEV(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //测试10次，在某一次成功就退出
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
			break;   //接收到"OK"
		}
	}
}

void Send_String_To_Server(char string[]) {
	u8 *p, i = ATwaits, len;
	char cmd[50];
	//测试10次，在某一次成功就退出

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
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
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
			break;   //接收到"OK"
		}
	}
}

/***********************关闭显示源数据IP地址和端口****************************
 *功    能: 串口发送数组命令到SIM5320E，    CIPSRIP[]="CIPSRIP=0";//关闭显示源数据IP地址和端口
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void CIPSRIP(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	//测试10次，在某一次成功就退出
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
			break;   //接收到"OK"
		}
	}
}

/***********************关闭网络连接******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+NETCLOSE
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Close_Network(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //测试10次，在某一次成功就退出
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
			break;   //接收到"OK"
		}
	}
}

/***********************开启GPS功能******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+CGPS=1
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Open_GPS(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //测试10次，在某一次成功就退出
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
			break;   //接收到"OK"
		}
	}
}

/***********************关闭GPS功能******************************
 *功    能: 串口发送数组命令到SIM5320E， AT+CGPS=0
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Close_GPS(void) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //测试10次，在某一次成功就退出
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
			break;   //接收到"OK"
		}
	}
}

/***********************获取GPS信息******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+CGPSINFO
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Get_GPS_Info(char *info) {
	u8 *p, i = ATwaits, len; //
	char cmd[50];
	while (i--) //测试10次，在某一次成功就退出
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
			break;   //接收到"OK"
		}
	}
}

/***********************GPRS初始化******************************
 *功    能:
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void GPRS_INT(void) {
	delay_ms(5000);
	delay_ms(5000);
	delay_ms(5000);
	delay_ms(5000);
	delay_ms(5000);   //等待SIM5320硬件初始化
//	Set_IPR115200();
	Send_AT();		//AT联机测试
	Send_ATE0();	//取消回显
//	Set_CNMI();		//設置短信提示
//	Set_MODE(1);	//设置短信格式文本
	Check_Net_Register();		//查询网络注册
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
