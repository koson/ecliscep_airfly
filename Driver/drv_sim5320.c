/*
 * drv_sim5320.c
 *
 *  Created on: 2016��1��20��
 *      Author: Sujunqin
 */
#include "drv_sim5320.h"

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
