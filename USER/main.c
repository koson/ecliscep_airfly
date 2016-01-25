/*
 ʵ��1��UCOSII
 1.	ʵ��Ŀ�ģ�����UCOSII������Ĵ���������
 2.	ʵ������LED0��LED1ѭ����˸��LCD��������ѭ����ɫ���������°벿�־��д������幦�ܣ�����KEY0�������Խ��봥��У׼���档
 3.	�õ���UCOSII����������
 UCOSII��ʼ��������void  OSInit (void)
 ���񴴽�������
 INT8U  OSTaskCreate (void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio)��
 ������������void  OSStart (void);
 
 �����������ؽ���һ��OSTaskCreate()������
 �����������ڲ���������ָ�룬���񴫵ݲ����������ջջ��ָ���Լ��������ȼ���
 ���������ǽ���һ���⼸����ڲ��������ã�
 1)	�������ȼ���������ִ�е�ʱ�򣬲���ϵͳ����ѡ��һ��������ִ�У���Ϊһ��CPUͬһʱ��ֻ��ִ��һ����������UCOSII�ǰ����ȼ���ռʽ������ѡ������ģ����Զ���ÿ�����񣬶��ᶨ��һ�����ȼ����������ȼ���Ψһ�ġ�
 2)	�����ջ���洢���а��Ƚ���(LIF0)��ԭ����֯�������洢�ռ䣬���������������л�����Ӧ�ж�ʱ����CPU�Ĵ��������ݺ��������������������Ҫ������ѧ����Ƭ�����˶�֪���жϵ�ʱ����һ���ֳ������ĸ���ֳ����������õ���ջ��
 �����ջ�ķ�ʽ��
 OS_STK  TASK_START_STK[START_STK_SIZE];
 ����START_STK_SIZE�����Ǻ궨��������ջ�Ĵ�С��
 3)	����ָ�룺����ָ������ִ����ڵ�ַ��ָ���ˡ�C�������溯�����־Ϳ��Կ�����������ڵ�ַ�ˡ�
 4)	���񴫵ݲ���������Ͳ��ý��⣬����˼����Ǵ��ݸ�����Ĳ����ˡ�

 �����ʼ��������ϵͳ����֮������ȵ��ô˺�����ʼ��UCOSII֮����ܵ��ô�������OSTaskStart�Լ�����������OSStart();
 ��������������OSStart()��UCOSIIҪ�����ڵ��ô˺���֮ǰϵͳ���봴������һ�������������Ǿʹ�����TaskStart��������������У����������������Ĵ�����
 */
#include "board.h"

//���������ջ��С
#define START_STK_SIZE   512
#define CONTROL_STK_SIZE 64

//�����������ȼ�
#define START_TASK_Prio     10
#define CONTROL_TASK_Prio   2

//�����ջ
OS_STK TASK_START_STK[START_STK_SIZE];
OS_STK TASK_CONTROL_STK[CONTROL_STK_SIZE];

void hardware_init(void);
//��������
void TaskStart(void *pdata);
void TaskControl(void *pdata);

int main(void) {
	hardware_init();

	OSInit();
	OSTaskCreate(TaskStart,	//task pointer
			(void *) 0,	//parameter
			(OS_STK *) &TASK_START_STK[START_STK_SIZE - 1],	//task stack top pointer
			START_TASK_Prio);	//task priority
	OSStart();
	return 0;

}

//��ʼ����
void TaskStart(void * pdata) {
	OS_ENTER_CRITICAL();
	OSTaskCreate(TaskControl, (void *) 0,
			(OS_STK *) &TASK_CONTROL_STK[CONTROL_STK_SIZE - 1],
			CONTROL_TASK_Prio);

	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
}

//����1-����
void TaskControl(void *pdata) {
	for (;;) {
		sim5320_test();
	}
}

//Ӳ����ʼ��
void hardware_init() {
	SystemInit(); //ϵͳ��ʼ�� 72M
	delay_init(72); //��ʱ��ʼ��
	NVIC_Configuration();
	uart1_init(115200); //���ڳ�ʼ����������115200
	uart2_init(115200); //���ڳ�ʼ����������115200
	printf("complier time:%s,%s\r\n", __DATE__, __TIME__);
	LED_Init();
	//	Pwm_Cap_Init(0xffff, 72); //PWM�����ʼ��,��1Mhz��Ƶ�ʼ���
	//	Pwm_Ouput_Init();                //��ʼ��4·PWM���
	SIM5320_INT();
}

