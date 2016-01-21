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
 ** ��ڲ��� ��
 ** ���ڲ��� ��
 **************************************/
void SendToGsm(u8* p, u8 len) //�ַ������ַ�����
{
	Usart_Send_Data(SIM5320_USART, p, len);
}

//���ͻس���,���ö���Ϣģʽ 1 TEXT
void Send_DA(void) {
	Usart_Send_Byte(SIM5320_USART, 0x0D);
	Usart_Send_Byte(SIM5320_USART, 0x0A);
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
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		SendToGsm(AT, 2);  //"AT"
		Send_DA();  //�س���
		delay_ms(300);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���RsBuf
		if (p != NULL) //���յ�"OK"
		{
			printf("Send_AT Reiceive OK\r\n");
			break;
		}
	}
}

/***********************����ȡ������ָ��******************************
 *��    ��: ���ڷ����������TC35��"AT0",
 *��    ��:
 *�� �� ֵ:
 *��    ע: �����޻��Թ���
 *****************************************************************/
void Send_ATE0(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		// ����ֻ��\r ����ӵ�\n ����֤�� ���Բ��ӵ�
		SendToGsm(ATE0, 4);    //"AT"
		Send_DA();    //�س���
		delay_ms(400);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL)
			printf("Send_ATE0 Reiceive OK\r\n");
		break;   //���յ�"OK"
	}
}

/***********************����ע��״̬******************************
 *��    ��: ���ڷ����������TC35��"AT+CREG?",
 *��    ��:
 *�� �� ֵ:
 *��    ע: ��ѯ����ע��״+CREG: 0,2//ûע�� +CREG: 0,1//ע������
 *****************************************************************/
void ASK_CREG(void) {
	u8 i = 20, len; //
	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{

		CLR_RBUF();
		SendToGsm(AT, 3);    //"AT"
		SendToGsm(Creg_Mes, 5);    //"AT+CREG?"
		Send_DA();    //�س���
		delay_ms(800);
		Sim5320_Receive_Data(receive_buf, &len);
		//��ʾע����������31ע�᱾���� 35ע������
		if ((receive_buf[11] == 0x31) | (receive_buf[11] == 0x35)) {
			CLR_RBUF();
			printf("ASK_CREG Reiceive OK\r\n");
			break; //���յ�"OK"
		}
	}
}

/***********************���ö���Ϣģʽ******************************
 *��    ��: ���ڷ����������TC35��AT+CMGF=1
 *��    ��:    uchar m =1 textģʽ      m=0 PDUģʽ
 *�� �� ֵ:
 *��    ע:    1 TEXT
 *****************************************************************/
void Set_MODE(u8 m) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		if (m) {
			SendToGsm(AT, 3);    //"AT"
			SendToGsm(TxtMode, 6);    //"AT"
			Send_DA();    //�س��� //���ö���Ϣģʽ 1 TEXT
		} else {
			SendToGsm(AT, 3);
			SendToGsm(PDUMode, 6);
			Send_DA();    //�س��� //���ö���Ϣģʽ 0  PDUģʽ
		}
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(400);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("Set_MODE Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}
void Set_CNMI(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(IND_Mes, 8);    //CMTI=1��2
		Send_DA();    //�س���//���ö�����ʾ
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("Set_CNMI Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

void Set_CMGD(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(clear_Mes, 6);    //CMGD=1
		Send_DA();    //�س���//���ö�����ʾ
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("Set_CMGD Reiceive OK\r\n");
			CLR_RBUF();
			break;   //���յ�"OK"
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
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(IPR, 8);    //"CGPSIPR=9600"	�����Ϊ115200 Ϊ14���ַ�
		Send_DA();    //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("Set_IPR9600 Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************����������******************************
 *��    ��: ���ڷ����������SIM5320E��AT+IPR=9600
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void Set_IPR115200(void) {
	u8 *p, i = ATwaits, len;

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		SendToGsm(AT, 3);    //"AT+"
		SendToGsm(IPR1, 14);    //"CGPSIPR=9600"	�����Ϊ115200 Ϊ14���ַ�
		Send_DA();    //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("Set_IPR115200 Reiceive OK\r\n");
			break;
		}
	}
}

/***********************������******************************
 *��    ��: ���ڷ����������SIM5320E�� OPNET[]="NETOPEN="TCP",80";//������
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void OPEN_NET(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		SendToGsm(AT, 3);    //"AT+"
//	 SendToGsm(OPNET,16);    //"CGPSIPR=9600"	�����Ϊ115200 Ϊ14���ַ�
		printf("NETOPEN=\"%s\",%s", modetbl[0], PROT1);
		Send_DA();    //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("OPEN_NET Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************���ӷ�����******************************
 *��    ��: ���ڷ����������SIM5320E��  TCPCONNEXT[]="TCPCONNECT="220.170.79.231",56302";//���ӷ�����
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void CONNECT_SEV(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		// ����ֻ��\r ����ӵ�\n ����֤�� ���Բ��ӵ�
//	 SendToGsm(AT,3);    //"AT+"
		//SendToGsm(TCPCONNEXT,33);    //
		printf("AT+TCPCONNECT=\"%s\",%s", ipbuf, port);
		Send_DA();	 //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("CONNECT_SEV Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************����10���ֽ�����*****************************
 *��    ��: ���ڷ����������SIM5320E��  TCPWRITE[]="TCPWRITE=10";//��������
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void SEND_DATA10(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		// ����ֻ��\r ����ӵ�\n ����֤�� ���Բ��ӵ�
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(TCPWRITE,11);    //
		printf("TCPWRITE=10");
		Send_DA();	 //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(10000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("SEND_DATA10 Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************����3200���ֽ�����*****************************
 *��    ��: ���ڷ����������SIM5320E��  TCPWRITE[]="TCPWRITE=10";//��������
 *��    ��:
 *�� �� ֵ:
 *��    ע:
 *****************************************************************/
void SEND_DATA3200(void) {
	u8 *p, i = ATwaits, len; //

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		// ����ֻ��\r ����ӵ�\n ����֤�� ���Բ��ӵ�
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(TCPWRITE,11);    //
		printf("TCPWRITE=3200");
		Send_DA();	 //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(10000);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("SEND_DATA3200 Reiceive OK\r\n");
			break;   //���յ�"OK"
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

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		// ����ֻ��\r ����ӵ�\n ����֤�� ���Բ��ӵ�
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(CIPHEAD,9);    //
		printf("CIPHEAD=0");
		Send_DA();	 //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
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

	while (i--) //����10�Σ���ĳһ�γɹ����˳�
	{
		CLR_RBUF();
		//�崮��1���ݻ�����
		// ����ֻ��\r ����ӵ�\n ����֤�� ���Բ��ӵ�
		SendToGsm(AT, 3);    //"AT+"
		//SendToGsm(CIPSRIP,9);    //
		printf("CIPSRIP=0");
		Send_DA();	 //�س���//���ö���Ϣģʽ 1 TEXT
		//****************************�ȴ�Ӧ��"OK"
		delay_ms(200);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) {
			printf("CIPHEAD Reiceive OK\r\n");
			break;   //���յ�"OK"
		}
	}
}

/***********************����TEXT����Ϣ********************************
 *��    ��: TEXTģʽ���Ͷ���Ϣ
 *��    ��: char *dialnum Ŀ�ĺ���13512345678    char *text��������
 *�� �� ֵ: ��   SEND_OK=1;//���ͳɹ�
 *��    ע:����TEXT����Ϣ ֮ǰӦ���� 1�����ö���Ϣģʽ   AT+CMGF=? (0)=PDU (1)=TEXT
 ������ 2�����ö���Ϣ����     AT+CSCA=��+8613800531500����������������

 *****************************************************************/
void TransmitText(u8 *dialnum, u8 *text)   //���ͺ��� �������ݣ���ĸ������
{
	u8 i = Sendwaits, j = Sendwaits, len;
	u8 *p;   //temp �����

	CLR_RBUF();
	SendToGsm(AT, 3);    //"AT+"
	SendToGsm(Send_Mes, 5);    //"CMGS="
	Usart_Send_Byte(SIM5320_USART, '"'); //���ö���Ϣģʽ 1 TEXT
	SendToGsm(dialnum, 11); //"CMGS="SendString(Dialnum);   //�ֻ���������Ŷ��ˣ����������������� �Ѿ�����
	Usart_Send_Byte(SIM5320_USART, '"');

	Send_DA();    //�س���  //���ͻس�ָ��//
	while (i--) {
		delay_ms(400);
		Sim5320_Receive_Data(receive_buf, &len);
		p = mystrstr(receive_buf, "OK");   //���յ������ݴ���sci1_rbuf
		if (p != NULL) //������ܵ� > ����text
		{
			///////Get">"////////////////////////////
			CLR_RBUF(); //��ս��ջ�����
			//��������
			//����
			SendToGsm(text, 18);	 //�ܼ����ݸ��� ����*����11
			Usart_Send_Byte(SIM5320_USART, '\x1a');	//'\x1a'������(�൱CTRL+Z) '\r'�س���
			Send_DA();	 //�س���   //���ͻس�ָ��//
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

void GPRS_INT(void)			//GPRS��ʼ��
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
	Send_AT();			//AT��������
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	//ȡ������
	Send_ATE0();
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	//�O�ö�����ʾ
	Set_CNMI();
	//���ö��Ÿ�ʽ
	Set_MODE(1);			//�ı�
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	delay_ms(200);
	//����������ѯ�ź����źŲſ��Է�����
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
	// printf("����ͼƬ��");
}
