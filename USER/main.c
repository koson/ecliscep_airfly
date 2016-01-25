/*
 实验1：UCOSII
 1.	实验目的：测试UCOSII多任务的创建和运行
 2.	实现现象：LED0，LED1循环闪烁，LCD部分区域循环变色，触摸屏下半部分具有触摸画板功能，按下KEY0按键可以进入触摸校准界面。
 3.	用到的UCOSII函数简析：
 UCOSII初始化函数：void  OSInit (void)
 任务创建函数：
 INT8U  OSTaskCreate (void (*task)(void *pd), void *pdata, OS_STK *ptos, INT8U prio)；
 启动任务函数：void  OSStart (void);
 
 这里我们着重介绍一下OSTaskCreate()函数：
 这个函数的入口参数是任务指针，任务传递参数，任务堆栈栈顶指针以及任务优先级。
 在这里我们介绍一下这几个入口参数的作用：
 1)	任务优先级：多任务执行的时候，操作系统必须选择一个任务来执行，因为一个CPU同一时间只能执行一个任务，这里UCOSII是按优先级抢占式规则来选择任务的，所以对于每个任务，都会定义一个优先级，而且优先级是唯一的。
 2)	任务堆栈：存储器中按先进后(LIF0)出原则组织的连续存储空间，作用是满足任务切换和相应中断时保存CPU寄存器的内容和任务调用其他函数的需要。这里学过单片机的人都知道中断的时候有一个现场保护的概念，现场保护就是用到堆栈。
 定义堆栈的方式：
 OS_STK  TASK_START_STK[START_STK_SIZE];
 其中START_STK_SIZE是我们宏定义的任务堆栈的大小。
 3)	任务指针：就是指向任务执行入口地址的指针了。C语言里面函数名字就可以看做函数的入口地址了。
 4)	任务传递参数：这个就不用讲解，顾名思义就是传递给任务的参数了。

 任务初始化函数在系统启动之后必须先调用此函数初始化UCOSII之后才能调用创建函数OSTaskStart以及启动任务函数OSStart();
 对于启动任务函数OSStart()，UCOSII要求是在调用此函数之前系统必须创建至少一个任务，这里我们就创建了TaskStart任务，在这个任务中，我们完成其他任务的创建。
 */
#include "board.h"

//设置任务堆栈大小
#define START_STK_SIZE   512
#define CONTROL_STK_SIZE 64

//设置任务优先级
#define START_TASK_Prio     10
#define CONTROL_TASK_Prio   2

//任务堆栈
OS_STK TASK_START_STK[START_STK_SIZE];
OS_STK TASK_CONTROL_STK[CONTROL_STK_SIZE];

void hardware_init(void);
//任务申明
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

//开始任务
void TaskStart(void * pdata) {
	OS_ENTER_CRITICAL();
	OSTaskCreate(TaskControl, (void *) 0,
			(OS_STK *) &TASK_CONTROL_STK[CONTROL_STK_SIZE - 1],
			CONTROL_TASK_Prio);

	OSTaskSuspend(START_TASK_Prio);	//suspend but not delete
	OS_EXIT_CRITICAL();
}

//任务1-控制
void TaskControl(void *pdata) {
	for (;;) {
		sim5320_test();
	}
}

//硬件初始化
void hardware_init() {
	SystemInit(); //系统初始化 72M
	delay_init(72); //延时初始化
	NVIC_Configuration();
	uart1_init(115200); //串口初始化，波特率115200
	uart2_init(115200); //串口初始化，波特率115200
	printf("complier time:%s,%s\r\n", __DATE__, __TIME__);
	LED_Init();
	//	Pwm_Cap_Init(0xffff, 72); //PWM捕获初始化,以1Mhz的频率计数
	//	Pwm_Ouput_Init();                //初始化4路PWM输出
	SIM5320_INT();
}

