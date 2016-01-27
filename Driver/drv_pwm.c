#include "drv_pwm.h"
#include "usart.h"
#include "delay.h"
#include "led.h"

#define PwmMaxValue 2200
#define PwmMimValue 900

//TIM2
u8 TIM2CH1_CAPTURE_STA = 0;	//ͨ��1���벶���־������λ�������־����6λ�������־		
u16 TIM2CH1_CAPTURE_UPVAL;
u16 TIM2CH1_CAPTURE_DOWNVAL;

u8 TIM2CH2_CAPTURE_STA = 0;	//ͨ��2���벶���־������λ�������־����6λ�������־		
u16 TIM2CH2_CAPTURE_UPVAL;
u16 TIM2CH2_CAPTURE_DOWNVAL;

u8 TIM2CH3_CAPTURE_STA = 0;	//ͨ��3���벶���־������λ�������־����6λ�������־		
u16 TIM2CH3_CAPTURE_UPVAL;
u16 TIM2CH3_CAPTURE_DOWNVAL;

u8 TIM2CH4_CAPTURE_STA = 0;	//ͨ��1���벶���־������λ�������־����6λ�������־		
u16 TIM2CH4_CAPTURE_UPVAL;
u16 TIM2CH4_CAPTURE_DOWNVAL;

//TIM3
u8 TIM3CH1_CAPTURE_STA = 0;	//ͨ��1���벶���־������λ�������־����6λ�������־		
u16 TIM3CH1_CAPTURE_UPVAL;
u16 TIM3CH1_CAPTURE_DOWNVAL;

u8 TIM3CH2_CAPTURE_STA = 0;	//ͨ��2���벶���־������λ�������־����6λ�������־		
u16 TIM3CH2_CAPTURE_UPVAL;
u16 TIM3CH2_CAPTURE_DOWNVAL;

u8 TIM3CH3_CAPTURE_STA = 0;	//ͨ��3���벶���־������λ�������־����6λ�������־		
u16 TIM3CH3_CAPTURE_UPVAL;
u16 TIM3CH3_CAPTURE_DOWNVAL;

u8 TIM3CH4_CAPTURE_STA = 0;	//ͨ��1���벶���־������λ�������־����6λ�������־		
u16 TIM3CH4_CAPTURE_UPVAL;
u16 TIM3CH4_CAPTURE_DOWNVAL;

u32 tempup1 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tempup2 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tempup3 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tempup4 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tempup5 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tempup6 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tempup7 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tempup8 = 0;	//�����ܸߵ�ƽ��ʱ��
u32 tim2_T1;
u32 tim2_T2;
u32 tim2_T3;
u32 tim2_T4;
u32 tim3_T1;
u32 tim3_T2;
u32 tim3_T3;
u32 tim3_T4;
int pwmout1, pwmout2, pwmout3, pwmout4, pwmout5, pwmout6, pwmout7, pwmout8; //�����ռ�ձ�

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

void Tim4_init(u16 arr, u16 psc)	//PWMƵ��50Hz
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��

	TIM_TimeBaseStructure.TIM_Period = arr;	//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���������ϼ���
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);	//��ʱ��2��ʼ��

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIM_CNT<TIMX_CCRXʱ�����Ϊ��Ч��ƽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;	//��ʼռ�ձ�Ϊ0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//������Ч��ƽΪ�ߵ�ƽ�����PWMģʽ�������õ���PWM1
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);	//��ʼ��TIM_OC1
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);	//ʹ��Ԥװ��
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);	//ʹ�ܶ�ʱ��1
}

void Tim1_init(u16 arr, u16 psc)	//PWMƵ��50Hz
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��

	TIM_TimeBaseStructure.TIM_Period = arr;	//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���������ϼ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);	//��ʱ��2��ʼ��

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//TIM_CNT<TIMX_CCRXʱ�����Ϊ��Ч��ƽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;	//��ʼռ�ձ�Ϊ0
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//������Ч��ƽΪ�ߵ�ƽ�����PWMģʽ�������õ���PWM1

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);	//��ʼ��TIM_OC1
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);	//ʹ��Ԥװ��

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
	TIM_Cmd(TIM1, ENABLE); //ʹ�ܶ�ʱ1
	//һ��Ҫ�ӣ����������
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Pwm_Ouput_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //GPIOBʱ��ʹ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
			| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO����50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���ù������
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��
	Tim4_init(2000, 72); //��ʱ��4PWM��ʼ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //GPIOAʱ��ʹ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //IO����50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���ù������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��
	Tim1_init(2000, 72); //��ʱ��4PWM��ʼ��

}

//��ʱ��2 3����
TIM_ICInitTypeDef TIM2_ICInitStructure;
TIM_ICInitTypeDef TIM3_ICInitStructure;

void Pwm_Cap_Init(u16 arr, u16 psc) {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ��TIM2ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ��TIM3ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
			| GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;  //PA0,1,2,3,6,7 ���֮ǰ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0,1,2,3 ���� 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,
			GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6
					| GPIO_Pin_7); //PA0,1,2,3,6,7 ����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;  //PB0,1 ���֮ǰ����  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PB0,1 ����
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1); //PB0,1����

	//��ʼ����ʱ��2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler = psc - 1; 	//Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	//��ʼ����ʱ��3 TIM3	 
	TIM_TimeBaseStructure.TIM_Period = arr; //�趨�������Զ���װֵ 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//Ԥ��Ƶ�� 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//��ʼ��TIM2���벶����� ͨ��1
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//��ʼ��TIM3���벶����� ͨ��1
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//��ʼ��TIM2���벶����� ͨ��2
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//��ʼ��TIM3���벶����� ͨ��2
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//��ʼ��TIM2���벶����� ͨ��3
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//��ʼ��TIM3���벶����� ͨ��3
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//��ʼ��TIM2���벶����� ͨ��4
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	//��ʼ��TIM3���벶����� ͨ��4
	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	  //���������Ƶ,����Ƶ 
	TIM3_ICInitStructure.TIM_ICFilter = 0x00;	  //IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM3, &TIM3_ICInitStructure);

	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);   //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 

	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);   //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 

	TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4,
			ENABLE);   //����������жϣ�����CC1IE,CC2IE,CC3IE,CC4IE�����ж�	

	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4,
			ENABLE);   //����������жϣ�����CC1IE,CC2IE,CC3IE,CC4IE�����ж�	

	TIM_Cmd(TIM2, ENABLE); 		//ʹ�ܶ�ʱ��2
	TIM_Cmd(TIM3, ENABLE); 		//ʹ�ܶ�ʱ��2
}

//��ʱ��2�жϷ������
void TIM2_IRQHandler(void) {
	if ((TIM2CH1_CAPTURE_STA & 0X80) == 0) 		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) 		//����1���������¼�
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC1); 		//����жϱ�־λ
			if (TIM2CH1_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM2CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM2);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM2CH1_CAPTURE_DOWNVAL < TIM2CH1_CAPTURE_UPVAL) {
					tim2_T1 = 65535;
				} else
					tim2_T1 = 0;
				tempup1 = TIM2CH1_CAPTURE_DOWNVAL - TIM2CH1_CAPTURE_UPVAL
						+ tim2_T1;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout1 = tempup1;		//�ܵĸߵ�ƽ��ʱ��
				TIM2CH1_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM2CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM2);		//��ȡ����������
				TIM2CH1_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC1PolarityConfig(TIM2, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}

	if ((TIM2CH2_CAPTURE_STA & 0X80) == 0)		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)		//����2���������¼�
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);		//����жϱ�־λ
			if (TIM2CH2_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM2CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM2);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM2CH2_CAPTURE_DOWNVAL < TIM2CH2_CAPTURE_UPVAL) {
					tim2_T2 = 65535;
				} else
					tim2_T2 = 0;
				tempup2 = TIM2CH2_CAPTURE_DOWNVAL - TIM2CH2_CAPTURE_UPVAL
						+ tim2_T2;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout2 = tempup2;		//�ܵĸߵ�ƽ��ʱ��
				TIM2CH2_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM2CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM2);		//��ȡ����������
				TIM2CH2_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}

	if ((TIM2CH3_CAPTURE_STA & 0X80) == 0)		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)		//����3���������¼�
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);		//����жϱ�־λ
			if (TIM2CH3_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM2CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM2);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM2CH3_CAPTURE_DOWNVAL < TIM2CH3_CAPTURE_UPVAL) {
					tim2_T3 = 65535;
				} else
					tim2_T3 = 0;
				tempup3 = TIM2CH3_CAPTURE_DOWNVAL - TIM2CH3_CAPTURE_UPVAL
						+ tim2_T3;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout3 = tempup3;		//�ܵĸߵ�ƽ��ʱ��
				TIM2CH3_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM2CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM2);		//��ȡ����������
				TIM2CH3_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC3PolarityConfig(TIM2, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}

	if ((TIM2CH4_CAPTURE_STA & 0X80) == 0)		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET)		//����4���������¼�
				{
			TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);		//����жϱ�־λ
			if (TIM2CH4_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM2CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM2);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM2CH4_CAPTURE_DOWNVAL < TIM2CH4_CAPTURE_UPVAL) {
					tim2_T4 = 65535;
				} else
					tim2_T4 = 0;
				tempup4 = TIM2CH4_CAPTURE_DOWNVAL - TIM2CH4_CAPTURE_UPVAL
						+ tim2_T4;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout4 = tempup4;		//�ܵĸߵ�ƽ��ʱ��
				TIM2CH4_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM2CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM2);		//��ȡ����������
				TIM2CH4_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC4PolarityConfig(TIM2, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void) {
	if ((TIM3CH1_CAPTURE_STA & 0X80) == 0) 		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) 		//����1���������¼�
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC1); 		//����жϱ�־λ
			if (TIM3CH1_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM3CH1_CAPTURE_DOWNVAL = TIM_GetCapture1(TIM3);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM3CH1_CAPTURE_DOWNVAL < TIM3CH1_CAPTURE_UPVAL) {
					tim3_T1 = 65535;
				} else
					tim3_T1 = 0;
				tempup5 = TIM3CH1_CAPTURE_DOWNVAL - TIM3CH1_CAPTURE_UPVAL
						+ tim3_T1;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout5 = tempup5;		//�ܵĸߵ�ƽ��ʱ��
				TIM3CH1_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM3CH1_CAPTURE_UPVAL = TIM_GetCapture1(TIM3);		//��ȡ����������
				TIM3CH1_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC1PolarityConfig(TIM3, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}

	if ((TIM3CH2_CAPTURE_STA & 0X80) == 0)		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)		//����2���������¼�
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);		//����жϱ�־λ
			if (TIM3CH2_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM3CH2_CAPTURE_DOWNVAL = TIM_GetCapture2(TIM3);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM3CH2_CAPTURE_DOWNVAL < TIM3CH2_CAPTURE_UPVAL) {
					tim3_T2 = 65535;
				} else
					tim3_T2 = 0;
				tempup6 = TIM3CH2_CAPTURE_DOWNVAL - TIM3CH2_CAPTURE_UPVAL
						+ tim3_T2;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout6 = tempup6;		//�ܵĸߵ�ƽ��ʱ��
				TIM3CH2_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM3CH2_CAPTURE_UPVAL = TIM_GetCapture2(TIM3);		//��ȡ����������
				TIM3CH2_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}

	if ((TIM3CH3_CAPTURE_STA & 0X80) == 0)		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)		//����3���������¼�
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);		//����жϱ�־λ
			if (TIM3CH3_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM3CH3_CAPTURE_DOWNVAL = TIM_GetCapture3(TIM3);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM3CH3_CAPTURE_DOWNVAL < TIM3CH3_CAPTURE_UPVAL) {
					tim3_T3 = 65535;
				} else
					tim3_T3 = 0;
				tempup7 = TIM3CH3_CAPTURE_DOWNVAL - TIM3CH3_CAPTURE_UPVAL
						+ tim3_T3;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout7 = tempup7;		//�ܵĸߵ�ƽ��ʱ��
				TIM3CH3_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC3PolarityConfig(TIM3, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM3CH3_CAPTURE_UPVAL = TIM_GetCapture3(TIM3);		//��ȡ����������
				TIM3CH3_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC3PolarityConfig(TIM3, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}

	if ((TIM3CH4_CAPTURE_STA & 0X80) == 0)		//��δ�ɹ�����	
			{
		if (TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)		//����4���������¼�
				{
			TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);		//����жϱ�־λ
			if (TIM3CH4_CAPTURE_STA & 0X40)		//����һ���½���
					{
				TIM3CH4_CAPTURE_DOWNVAL = TIM_GetCapture4(TIM3);//��¼�´�ʱ�Ķ�ʱ������ֵ
				if (TIM3CH4_CAPTURE_DOWNVAL < TIM3CH4_CAPTURE_UPVAL) {
					tim3_T4 = 65535;
				} else
					tim3_T4 = 0;
				tempup8 = TIM3CH4_CAPTURE_DOWNVAL - TIM3CH4_CAPTURE_UPVAL
						+ tim3_T4;		//�õ��ܵĸߵ�ƽ��ʱ��
				pwmout8 = tempup8;		//�ܵĸߵ�ƽ��ʱ��
				TIM3CH4_CAPTURE_STA = 0;		//�����־λ����
				TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Rising); //����Ϊ�����ز���		  
			} else //��������ʱ�䵫�����½��أ���һ�β��������أ���¼��ʱ�Ķ�ʱ������ֵ
			{
				TIM3CH4_CAPTURE_UPVAL = TIM_GetCapture4(TIM3);		//��ȡ����������
				TIM3CH4_CAPTURE_STA |= 0X40;		//����Ѳ���������
				TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Falling);//����Ϊ�½��ز���
			}
		}
	}
}

void pwm_test(void) {
	Pwm_Cap_Init(0xffff, 72); //PWM�����ʼ��,��1Mhz��Ƶ�ʼ���
	Pwm_Ouput_Init();                //��ʼ��4·PWM���
	while (1) {
		LED0 = !LED0;
		printf("C1:%d\tC2:%d\tC3:%d\tC4:%d\n\r", Chanel1_Pwm_Get(),
				Chanel2_Pwm_Get(), Chanel3_Pwm_Get(), Chanel4_Pwm_Get()); //���ң����4·����ʱ��us
		printf("C5:%d\tC6:%d\tC7:%d\tC8:%d\n\r", Chanel5_Pwm_Get(),
				Chanel6_Pwm_Get(), Chanel7_Pwm_Get(), Chanel8_Pwm_Get()); //���ң����4·����ʱ��us
		Moto_PwmRflash(Chanel1_Pwm_Get(), Chanel2_Pwm_Get(), Chanel3_Pwm_Get(),
				Chanel4_Pwm_Get());
		Chanel1_Pwm_Set(1400);
		Chanel2_Pwm_Set(1400);
		delay_ms(500);
	}
}

