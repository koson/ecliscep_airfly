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

const u8 *modetbl[2] = { "TCP", "UDP" };  //
const u8 port[] = "35286";	//
u8 ipbuf[] = "220.170.79.231"; 	//IP
const u8 PROT1[] = "80";	//PROT1

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
 ** 入口参数 ：
 ** 出口参数 ：
 **************************************/
void SendToGsm(u8* p, u8 len) //字符串，字符长度
{
	Usart_Send_Data(SIM5320_USART, p, len);
}

//发送回车符,设置短消息模式 1 TEXT
void Send_DA(void) {
	Usart_Send_Byte(SIM5320_USART, 0x0D);
	Usart_Send_Byte(SIM5320_USART, 0x0A);
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
	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		SendToGsm(AT, 2);  //"AT"
		Send_DA();  //回车符
		delay_ms(300);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在RsBuf
		if (p != NULL) //接收到"OK"
		{
			printf("Send_AT Reiceive OK\r\n");
			break;
		}
	}
}

/***********************发送取消回显指令******************************
 *功    能: 串口发送数组命令到TC35，"AT0",
 *形    参:
 *返 回 值:
 *备    注: 设置无回显功能
 *****************************************************************/
void Send_ATE0(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		// 本来只有\r 我添加的\n 后来证明 可以不加的
		SendToGsm(ATE0, 4);    //"AT"
		Send_DA();    //回车符
		delay_ms(400);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL)
			printf("Send_ATE0 Reiceive OK\r\n");
		break;   //接收到"OK"
	}
}

/***********************网络注册状态******************************
 *功    能: 串口发送数组命令到TC35，"AT+CREG?",
 *形    参:
 *返 回 值:
 *备    注: 查询网络注册状+CREG: 0,2//没注册 +CREG: 0,1//注册上了
 *****************************************************************/
void ASK_CREG(void) {
	u8 i = 20, len; //
	while (i--) //测试10次，在某一次成功就退出
	{

		CLR_RBUF();
		SendToGsm(AT, 3);    //"AT"
		SendToGsm(Creg_Mes, 5);    //"AT+CREG?"
		Send_DA();    //回车符
		delay_ms(800);
		Sim5320_Receive_Data(receive_buf, &len);
		//表示注册上了网络31注册本地网 35注册漫游
		if ((receive_buf[11] == 0x31) | (receive_buf[11] == 0x35)) {
			CLR_RBUF();
			printf("ASK_CREG Reiceive OK\r\n");
			break; //接收到"OK"
		}
	}
}

/***********************设置短消息模式******************************
 *功    能: 串口发送数组命令到TC35，AT+CMGF=1
 *形    参:    uchar m =1 text模式      m=0 PDU模式
 *返 回 值:
 *备    注:    1 TEXT
 *****************************************************************/
void Set_MODE(u8 m) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		if (m) {
			SendToGsm(AT, 3);    //"AT"
			SendToGsm(TxtMode, 6);    //"AT"
			Send_DA();    //回车符 //设置短消息模式 1 TEXT
		} else {
			SendToGsm(AT, 3);
			SendToGsm(PDUMode, 6);
			Send_DA();    //回车符 //设置短消息模式 0  PDU模式
		}
		//****************************等待应答"OK"
		delay_ms(400);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("Set_MODE Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}
void Set_CNMI(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(IND_Mes, 8);    //CMTI=1，2
		Send_DA();    //回车符//设置短信提示
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("Set_CNMI Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}

void Set_CMGD(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(clear_Mes, 6);    //CMGD=1
		Send_DA();    //回车符//设置短信提示
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("Set_CMGD Reiceive OK\r\n");
			CLR_RBUF();
			break;   //接收到"OK"
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
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(IPR, 8);    //"CGPSIPR=9600"	如果改为115200 为14个字符
		Send_DA();    //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("Set_IPR9600 Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}

/***********************波特率设置******************************
 *功    能: 串口发送数组命令到SIM5320E，AT+IPR=9600
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void Set_IPR115200(void) {
	u8 *p, i = ATwaits, len;

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(IPR1, 14);    //"CGPSIPR=9600"	如果改为115200 为14个字符
		Send_DA();    //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("Set_IPR115200 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************打开网络******************************
 *功    能: 串口发送数组命令到SIM5320E， OPNET[]="NETOPEN="TCP",80";//打开网络
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void OPEN_NET(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		SendToGsm(AT, 3);    //"AT+"
//	 SendToGsm(OPNET,16);    //"CGPSIPR=9600"	如果改为115200 为14个字符
		printf("NETOPEN=\"%s\",%s", modetbl[0], PROT1);
		Send_DA();    //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("OPEN_NET Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}

/***********************链接服务器******************************
 *功    能: 串口发送数组命令到SIM5320E，  TCPCONNEXT[]="TCPCONNECT="220.170.79.231",56302";//链接服务器
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void CONNECT_SEV(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		// 本来只有\r 我添加的\n 后来证明 可以不加的
//	 SendToGsm(AT,3);    //"AT+"
		//SendToGsm(TCPCONNEXT,33);    //
		printf("AT+TCPCONNECT=\"%s\",%s", ipbuf, port);
		Send_DA();	 //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("CONNECT_SEV Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}

/***********************发送10个字节数据*****************************
 *功    能: 串口发送数组命令到SIM5320E，  TCPWRITE[]="TCPWRITE=10";//发送数据
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void SEND_DATA10(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		// 本来只有\r 我添加的\n 后来证明 可以不加的
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(TCPWRITE,11);    //
		printf("TCPWRITE=10");
		Send_DA();	 //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(10000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("SEND_DATA10 Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}

/***********************发送3200个字节数据*****************************
 *功    能: 串口发送数组命令到SIM5320E，  TCPWRITE[]="TCPWRITE=10";//发送数据
 *形    参:
 *返 回 值:
 *备    注:
 *****************************************************************/
void SEND_DATA3200(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		// 本来只有\r 我添加的\n 后来证明 可以不加的
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(TCPWRITE,11);    //
		printf("TCPWRITE=3200");
		Send_DA();	 //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(10000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("SEND_DATA3200 Reiceive OK\r\n");
			break;   //接收到"OK"
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

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		// 本来只有\r 我添加的\n 后来证明 可以不加的
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(CIPHEAD,9);    //
		printf("CIPHEAD=0");
		Send_DA();	 //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
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

	while (i--) //测试10次，在某一次成功就退出
	{
		CLR_RBUF();
		//清串口1数据缓冲区
		// 本来只有\r 我添加的\n 后来证明 可以不加的
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(CIPSRIP,9);    //
		printf("CIPSRIP=0");
		Send_DA();	 //回车符//设置短消息模式 1 TEXT
		//****************************等待应答"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) {
			printf("CIPHEAD Reiceive OK\r\n");
			break;   //接收到"OK"
		}
	}
}

/***********************发送TEXT短信息********************************
 *功    能: TEXT模式发送短信息
 *形    参: char *dialnum 目的号码13512345678    char *text发送内容
 *返 回 值: 无   SEND_OK=1;//发送成功
 *备    注:发送TEXT短信息 之前应该先 1，设置短消息模式   AT+CMGF=? (0)=PDU (1)=TEXT
 声明： 2，设置短消息中心     AT+CSCA=“+8613800531500”此内容无需设置

 *****************************************************************/
void TransmitText(u8 *dialnum, u8 *text)   //发送号码 发送内容，字母或数字
{
	u8 i = Sendwaits, j = Sendwaits, len;
	u8 *p;   //temp 求异或

	CLR_RBUF();
	SendToGsm(AT, 3);    //"AT+"
	SendToGsm(Send_Mes, 5);    //"CMGS="
	Usart_Send_Byte(SIM5320_USART, '"'); //设置短消息模式 1 TEXT
	SendToGsm(dialnum, 11); //"CMGS="SendString(Dialnum);   //手机号码的引号丢了！！！！！！！！！ 已经加上
	Usart_Send_Byte(SIM5320_USART, '"');

	Send_DA();    //回车符  //发送回车指令//
	while (i--) {
		delay_ms(400);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //接收到的数据存在sci1_rbuf
		if (p != NULL) //如果接受到 > 发送text
		{
			///////Get">"////////////////////////////
			CLR_RBUF(); //清空接收缓冲区
			//发送内容
			//数据
			SendToGsm(text, 18);	 //总计数据个数 次数*个数11
			Usart_Send_Byte(SIM5320_USART, '\x1a');	//'\x1a'结束符(相当CTRL+Z) '\r'回车符
			Send_DA();	 //回车符   //发送回车指令//
			//delayms_100ms();
			while (j--) {
				delay_ms(1200);
				Sim5320_Receive_Data(receive_buf, &len);
				p = mystrstr(receive_buf, "OK");
				if (p != NULL) {
					CLR_RBUF();
					printf("TX MESSEAGE OK\r\n");
					break;
				}
			}
		}
	}
}

void GPRS_INT(void)			//GPRS初始化
{
	Set_IPR115200();
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	Send_AT();			//AT联机测试
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	//取消回显
	Send_ATE0();
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	//設置短信提示
	Set_CNMI();
	//设置短信格式
	Set_MODE(1);			//文本
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	//第三步：查询信号有信号才可以发短信
	ASK_CREG();
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
}

void GPRS_TEST(void) {

	OPEN_NET();
	delay_ms(200);
	CONNECT_SEV();
	delay_ms(200);
	CIPHEAD();
	delay_ms(200);
	CIPSRIP();
	SEND_DATA10();
	delay_ms(200);
	// printf("发送图片吧");
}
