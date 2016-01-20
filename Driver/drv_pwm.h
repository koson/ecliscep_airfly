#pragma once
#include "sys.h"


void Pwm_Ouput_Init(void);
void Moto_PwmRflash(int16_t MOTO1_PWM,int16_t MOTO2_PWM,int16_t MOTO3_PWM,int16_t MOTO4_PWM);

void Chanel1_Pwm_Set(u16 value);
void Chanel2_Pwm_Set(u16 value);
void Chanel3_Pwm_Set(u16 value);
void Chanel4_Pwm_Set(u16 value);
void Chanel5_Pwm_Set(u16 value);
void Chanel6_Pwm_Set(u16 value);

void Pwm_Cap_Init(u16 arr, u16 psc);
u32 Chanel1_Pwm_Get(void);
u32 Chanel2_Pwm_Get(void);
u32 Chanel3_Pwm_Get(void);
u32 Chanel4_Pwm_Get(void);
u32 Chanel5_Pwm_Get(void);
u32 Chanel6_Pwm_Get(void);
u32 Chanel7_Pwm_Get(void);
u32 Chanel8_Pwm_Get(void);
