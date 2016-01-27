#include "drv_pwm.h"
#include "usart.h"
#include "delay.h"
#include "led.h"

#define PwmMaxValue 2200
#define PwmMimValue 900

//TIM2
u8 TIM2CH1_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM2CH1_CAPTURE_UPVAL;
u16 TIM2CH1_CAPTURE_DOWNVAL;

u8 TIM2CH2_CAPTURE_STA = 0;	//通道2输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM2CH2_CAPTURE_UPVAL;
u16 TIM2CH2_CAPTURE_DOWNVAL;

u8 TIM2CH3_CAPTURE_STA = 0;	//通道3输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM2CH3_CAPTURE_UPVAL;
u16 TIM2CH3_CAPTURE_DOWNVAL;

u8 TIM2CH4_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM2CH4_CAPTURE_UPVAL;
u16 TIM2CH4_CAPTURE_DOWNVAL;

//TIM3
u8 TIM3CH1_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH1_CAPTURE_UPVAL;
u16 TIM3CH1_CAPTURE_DOWNVAL;

u8 TIM3CH2_CAPTURE_STA = 0;	//通道2输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH2_CAPTURE_UPVAL;
u16 TIM3CH2_CAPTURE_DOWNVAL;

u8 TIM3CH3_CAPTURE_STA = 0;	//通道3输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH3_CAPTURE_UPVAL;
u16 TIM3CH3_CAPTURE_DOWNVAL;

u8 TIM3CH4_CAPTURE_STA = 0;	//通道1输入捕获标志，高两位做捕获标志，低6位做溢出标志		
u16 TIM3CH4_CAPTURE_UPVAL;
u16 TIM3CH4_CAPTURE_DOWNVAL;

u32 tempup1 = 0;	//捕获总高电平的时间
u32 tempup2 = 0;	//捕获总高电平的时间
u32 tempup3 = 0;	//捕获总高电平的时间
u32 tempup4 = 0;	//捕获总高电平的时间
u32 tempup5 = 0;	//捕获总高电平的时间
u32 tempup6 = 0;	//捕获总高电平的时间
u32 tempup7 = 0;	//捕获总高电平的时间
u32 tempup8 = 0;	//捕获总高电平的时间
u32 tim2_T1;
u32 tim2_T2;
u32 tim2_T3;
u32 tim2_T4;
u32 tim3_T1;
u32 tim3_T2;
u32 tim3_T3;
u32 tim3_T4;
int pwmout1, pwmout2, pwmout3, pwmout4, pwmout5, pwmout6, pwmout7, pwmout8; //捕获的占空比

void Moto_PwmRflash(int16_t MOTO1_PWM, int16_t MOTO2_PWM, int16_t MOTO3_PWM,
		int16_t MOTO4_PWM) {

	//if(MOTO1_PWM>Moto_PwmMax)	MOTO1_PWM = Moto_PwmMax;
	//if(MOTO1_PWM<900)	MOTO1_PWM = 900;

	TIM4->CCR1 = MOTO1_PWM;
	TIM4->CCR2 = MOTO2_PWM;
	TIM4->CCR3 = MOTO3_PWM;
	TIM4->CCR4 = MOTO4_PWM;
}

void Chanel1_Pwm_Set(u16 value) {
	if (value > PwmMaxValue)
		value = PwmMaxValue;
	if (value < PwmMimValue)
		value = PwmMimValue;
	TIM1->CCR1 = value;
}
void Chanel2_Pwm_Set(u16 value) {
	if (value > PwmMaxValue)
		value = PwmMaxValue;
	if (value < PwmMimValue)
		value = PwmMimValue;
	TIM1->CCR4 = value;
}
void Chanel3_Pwm_Set(u16 value) {
	if (value > PwmMaxValue)
		value = PwmMaxValue;
	if (value < PwmMimValue)
		value = PwmMimValue;
	TIM4->CCR1 = value;
}
void Chanel4_Pwm_Set(u16 value) {
	if (value > PwmMaxValue)
		value = PwmMaxValue;
	if (value < PwmMimValue)
		value = PwmMimValue;
	TIM4->CCR2 = value;
}
void Chanel5_Pwm_Set(u16 value) {
	if (value > PwmMaxValue)
		value = PwmMaxValue;
	if (value < PwmMimValue)
		value = PwmMimValue;
	TIM4->CCR3 = value;
}
void Chanel6_Pwm_Set(u16 value) {
	if (value > PwmMaxValue)
		value = PwmMaxValue;
	if (value < PwmMimValue)
		value = PwmMimValue;
	TIM4->CCR4 = value;
}

u32 Chanel1_Pwm_Get(void) {
	return pwmout1;
}
u32 Chanel2_Pwm_Get(void) {
	return pwmout2;
}
u32 Chanel3_Pwm_Get(void) {
	return pwmout3;
}
u32 Chanel4_Pwm_Get(void) {
	return pwmout4;
}
u32 Chanel5_Pwm_Get(void) {
	return pwmout5;
}
u32 Chanel6_Pwm_Get(void) {
	return pwmout6;
}
u32 Chanel7_Pwm_Get(void) {
	return pwmout7;
}
u32 Chanel8_Pwm_Get(void) {
	return pwmout8;
}

void Tim4_init(u16 arr, u16 psc)	//PWM频率50Hz
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器2时钟

	TIM_TimeBaseStructure.TIM_Period = arr;	//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器向上记数
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);	//定时器2初始化

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIM_CNT<TIMX_CCRX时，输出为有效电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;	//初始占空比为0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//设置有效电平为高电平，结合PWM模式，上面用的是PWM1
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);	//初始化TIM_OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);	//使能预装载
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);	//使能定时器1
}

void Tim1_init(u16 arr, u16 psc)	//PWM频率50Hz
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//使能定时器2时钟

	TIM_TimeBaseStructure.TIM_Period = arr;	//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器向上记数
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);	//定时器2初始化

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIM_CNT<TIMX_CCRX时，输出为有效电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;	//初始占空比为0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//设置有效电平为高电平，结合PWM模式，上面用的是PWM1

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);	//初始化TIM_OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);	//使能预装载

	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	//???? 
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable; //???????
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable; //?????????
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1; //????,????1
	TIM_BDTRInitStructure.TIM_DeadTime = 0x0F; //????
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable; //??????
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low; //??????,????????GND?,PWM??
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable; //??????

	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
	TIM_Cmd(TIM1, ENABLE); //使能定时1
	//一定要加，否则不能输出
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Pwm_Ouput_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //GPIOB时钟使能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
			| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO速率50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用功能输出
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化
	Tim4_init(2000, 72); //定时器4PWM初始化

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //GPIOA时钟使能
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO速率50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用功能输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化
	Tim1_init(2000, 72); //定时器4PWM初始化

}

//定时器2 3配置
TIM_ICInitTypeDef TIM2_ICInitStructure;
TIM_ICInitTypeDef TIM3_ICInitStructure;

void Pwm_Cap_Init(u16 arr, u16 psc) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能TIM3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIOB时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
			| GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;  //PA0,1,2,3,6,7 清除之前设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0,1,2,3 输入 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,
			GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6
					| GPIO_Pin_7); //PA0,1,2,3,6,7 下拉

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  //PB0,1 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB0,1 输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1); //PB0,1下拉

	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1; 	//预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	//初始化定时器3 TIM3	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//预分频器 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	//初始化TIM2输入捕获参数 通道1
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//初始化TIM3输入捕获参数 通道1
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//初始化TIM2输入捕获参数 通道2
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//初始化TIM3输入捕获参数 通道2
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//初始化TIM2输入捕获参数 通道3
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//初始化TIM3输入捕获参数 通道3
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//初始化TIM2输入捕获参数 通道4
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//初始化TIM3输入捕获参数 通道4
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //配置输入分频,不分频 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);   //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);   //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 

	TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4,
			ENABLE);   //不允许更新中断，允许CC1IE,CC2IE,CC3IE,CC4IE捕获中断	

	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4,
			ENABLE);   //不允许更新中断，允许CC1IE,CC2IE,CC3IE,CC4IE捕获中断	

	TIM_Cmd(TIM2, ENABLE); 		//使能定时器2
	TIM_Cmd(TIM3, ENABLE); 		//使能定时器2
}

//定时器2中断服务程序
void TIM2_IRQHandler(void) {
	if ((TIM2CH1_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) 		//捕获1发生捕获事件
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC1); 		//清除中断标志位
			if (TIM2CH1_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM2CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM2);//记录下此时的定时器计数值
				if (TIM2CH1_CAPTURE_DOWNVAL < TIM2CH1_CAPTURE_UPVAL) {
					tim2_T1 = 65535;
				} else
					tim2_T1 = 0;
				tempup1 = TIM2CH1_CAPTURE_DOWNVAL - TIM2CH1_CAPTURE_UPVAL
						+ tim2_T1;		//得到总的高电平的时间
				pwmout1 = tempup1;		//总的高电平的时间
				TIM2CH1_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM2CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM2);		//获取上升沿数据
				TIM2CH1_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}

	if ((TIM2CH2_CAPTURE_STA & 0X80) == 0)		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)		//捕获2发生捕获事件
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);		//清除中断标志位
			if (TIM2CH2_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM2CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM2);//记录下此时的定时器计数值
				if (TIM2CH2_CAPTURE_DOWNVAL < TIM2CH2_CAPTURE_UPVAL) {
					tim2_T2 = 65535;
				} else
					tim2_T2 = 0;
				tempup2 = TIM2CH2_CAPTURE_DOWNVAL - TIM2CH2_CAPTURE_UPVAL
						+ tim2_T2;		//得到总的高电平的时间
				pwmout2 = tempup2;		//总的高电平的时间
				TIM2CH2_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM2CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM2);		//获取上升沿数据
				TIM2CH2_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}

	if ((TIM2CH3_CAPTURE_STA & 0X80) == 0)		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)		//捕获3发生捕获事件
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);		//清除中断标志位
			if (TIM2CH3_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM2CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM2);//记录下此时的定时器计数值
				if (TIM2CH3_CAPTURE_DOWNVAL < TIM2CH3_CAPTURE_UPVAL) {
					tim2_T3 = 65535;
				} else
					tim2_T3 = 0;
				tempup3 = TIM2CH3_CAPTURE_DOWNVAL - TIM2CH3_CAPTURE_UPVAL
						+ tim2_T3;		//得到总的高电平的时间
				pwmout3 = tempup3;		//总的高电平的时间
				TIM2CH3_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM2CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM2);		//获取上升沿数据
				TIM2CH3_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}

	if ((TIM2CH4_CAPTURE_STA & 0X80) == 0)		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)		//捕获4发生捕获事件
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);		//清除中断标志位
			if (TIM2CH4_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM2CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM2);//记录下此时的定时器计数值
				if (TIM2CH4_CAPTURE_DOWNVAL < TIM2CH4_CAPTURE_UPVAL) {
					tim2_T4 = 65535;
				} else
					tim2_T4 = 0;
				tempup4 = TIM2CH4_CAPTURE_DOWNVAL - TIM2CH4_CAPTURE_UPVAL
						+ tim2_T4;		//得到总的高电平的时间
				pwmout4 = tempup4;		//总的高电平的时间
				TIM2CH4_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM2CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM2);		//获取上升沿数据
				TIM2CH4_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}
}

//定时器3中断服务程序
void TIM3_IRQHandler(void) {
	if ((TIM3CH1_CAPTURE_STA & 0X80) == 0) 		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) 		//捕获1发生捕获事件
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); 		//清除中断标志位
			if (TIM3CH1_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM3CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM3);//记录下此时的定时器计数值
				if (TIM3CH1_CAPTURE_DOWNVAL < TIM3CH1_CAPTURE_UPVAL) {
					tim3_T1 = 65535;
				} else
					tim3_T1 = 0;
				tempup5 = TIM3CH1_CAPTURE_DOWNVAL - TIM3CH1_CAPTURE_UPVAL
						+ tim3_T1;		//得到总的高电平的时间
				pwmout5 = tempup5;		//总的高电平的时间
				TIM3CH1_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM3CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM3);		//获取上升沿数据
				TIM3CH1_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}

	if ((TIM3CH2_CAPTURE_STA & 0X80) == 0)		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)		//捕获2发生捕获事件
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);		//清除中断标志位
			if (TIM3CH2_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM3CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM3);//记录下此时的定时器计数值
				if (TIM3CH2_CAPTURE_DOWNVAL < TIM3CH2_CAPTURE_UPVAL) {
					tim3_T2 = 65535;
				} else
					tim3_T2 = 0;
				tempup6 = TIM3CH2_CAPTURE_DOWNVAL - TIM3CH2_CAPTURE_UPVAL
						+ tim3_T2;		//得到总的高电平的时间
				pwmout6 = tempup6;		//总的高电平的时间
				TIM3CH2_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM3CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM3);		//获取上升沿数据
				TIM3CH2_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}

	if ((TIM3CH3_CAPTURE_STA & 0X80) == 0)		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)		//捕获3发生捕获事件
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);		//清除中断标志位
			if (TIM3CH3_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM3CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM3);//记录下此时的定时器计数值
				if (TIM3CH3_CAPTURE_DOWNVAL < TIM3CH3_CAPTURE_UPVAL) {
					tim3_T3 = 65535;
				} else
					tim3_T3 = 0;
				tempup7 = TIM3CH3_CAPTURE_DOWNVAL - TIM3CH3_CAPTURE_UPVAL
						+ tim3_T3;		//得到总的高电平的时间
				pwmout7 = tempup7;		//总的高电平的时间
				TIM3CH3_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC3PolarityConfig(TIM3, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM3CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM3);		//获取上升沿数据
				TIM3CH3_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC3PolarityConfig(TIM3, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}

	if ((TIM3CH4_CAPTURE_STA & 0X80) == 0)		//还未成功捕获	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)		//捕获4发生捕获事件
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);		//清除中断标志位
			if (TIM3CH4_CAPTURE_STA & 0X40)		//捕获到一个下降沿
					{
				TIM3CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM3);//记录下此时的定时器计数值
				if (TIM3CH4_CAPTURE_DOWNVAL < TIM3CH4_CAPTURE_UPVAL) {
					tim3_T4 = 65535;
				} else
					tim3_T4 = 0;
				tempup8 = TIM3CH4_CAPTURE_DOWNVAL - TIM3CH4_CAPTURE_UPVAL
						+ tim3_T4;		//得到总的高电平的时间
				pwmout8 = tempup8;		//总的高电平的时间
				TIM3CH4_CAPTURE_STA = 0;		//捕获标志位清零
				TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Rising); //设置为上升沿捕获		  
			} else //发生捕获时间但不是下降沿，第一次捕获到上升沿，记录此时的定时器计数值
			{
				TIM3CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM3);		//获取上升沿数据
				TIM3CH4_CAPTURE_STA |= 0X40;		//标记已捕获到上升沿
				TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Falling);//设置为下降沿捕获
			}
		}
	}
}

void pwm_test(void) {
	Pwm_Cap_Init(0xffff, 72); //PWM捕获初始化,以1Mhz的频率计数
	Pwm_Ouput_Init();                //初始化4路PWM输出
	while (1) {
		LED0 = !LED0;
		printf("C1:%d\tC2:%d\tC3:%d\tC4:%d\n\r", Chanel1_Pwm_Get(),
				Chanel2_Pwm_Get(), Chanel3_Pwm_Get(), Chanel4_Pwm_Get()); //输出遥控器4路脉宽时间us
		printf("C5:%d\tC6:%d\tC7:%d\tC8:%d\n\r", Chanel5_Pwm_Get(),
				Chanel6_Pwm_Get(), Chanel7_Pwm_Get(), Chanel8_Pwm_Get()); //输出遥控器4路脉宽时间us
		Moto_PwmRflash(Chanel1_Pwm_Get(), Chanel2_Pwm_Get(), Chanel3_Pwm_Get(),
				Chanel4_Pwm_Get());
		Chanel1_Pwm_Set(1400);
		Chanel2_Pwm_Set(1400);
		delay_ms(500);
	}
}

