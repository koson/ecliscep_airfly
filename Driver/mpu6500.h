#ifndef __MPU6500_H
#define __MPU6500_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

enum Mpu6500_WorkState_e {
	MPU6500_WORK = 0, MPU6500_CYCLE,
};

enum Mpu6500_Dmp_e {
	MPU6500_WITHOUTDMP = 0, MPU6500_WITHDMP,
};

typedef struct Mpu6500_DmpData_t {
	uint8_t bCalculateAngle :1;
	uint8_t bReserve :7;

	short arrGyro[3];
	short arrAcc[3];
	short arrAngle[3];
	long arrQuat[4];

} Mpu6500_DmpData_t;

int Mpu6500_Init(bool bLowPower);
void Mpu6500_EnterCycle(void);

uint8_t Mpu6500_SingleRead(uint8_t address);
void Mpu6500_SingleWrite(uint8_t address, uint8_t data);
bool Mpu6500_MultiRead(uint8_t address, uint8_t *data, uint8_t len);
bool Mpu6500_MultiWrite(uint8_t address, uint8_t *data, uint8_t len);

void Mpu6500_GetGyro(int16_t *data);
void Mpu6500_GetAcc(int16_t *data);
int Mpu6500_ReadFifo(Mpu6500_DmpData_t* tData);

void mpu6500_test(void);
#endif 
