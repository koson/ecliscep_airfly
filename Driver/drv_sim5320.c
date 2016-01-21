/*
 * drv_sim5320.c
 *
 *  Created on: 2016年1月20日
 *      Author: Sujunqin
 */
#include "drv_sim5320.h"

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

//void  CLR_RBUF1(void)
//{
//   u16 i;
//  for(i=0;i<200;i++)
//  {
//   sci1_rbuf[i]=0x00;
//  }
//  sci1_counter=0;
//  sci1_start=0;
//}
