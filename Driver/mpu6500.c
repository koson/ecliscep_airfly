#include "mpu6500.h"
#include "mpu6500_spi.h"
#include "delay.h"

#include "anbt_dmp_driver.h"
#include "anbt_dmp_fun.h"

#define q30  1073741824.0f

#define MPU_DELAY_WITHSLEEP(n) {delay_ms(n);}

int Mpu6500_Init(bool bLowPower);
void Mpu6500_EnterCycle(void);

uint8_t Mpu6500_SingleRead(uint8_t address);
void Mpu6500_SingleWrite(uint8_t address, uint8_t data);
bool Mpu6500_MultiRead(uint8_t address, uint8_t *data, uint8_t len);
bool Mpu6500_MultiWrite(uint8_t address, uint8_t *data, uint8_t len);

void Mpu6500_GetAcc(int16_t *acc);
void Mpu6500_GetGyro(int16_t *gyro);
int Mpu6500_ReadFifo(Mpu6500_DmpData_t* tData);

uint8_t Mpu6500_SingleRead(uint8_t address) {
	uint8_t rData;

	SPI_CS = 0;

	SPIx_ReadWriteByte(128 + address, NULL);
	SPIx_ReadWriteByte(0, &rData);

	SPI_CS = 1;

	return rData;
}

void Mpu6500_SingleWrite(uint8_t address, uint8_t data) {
	SPI_CS = 0;

	SPIx_ReadWriteByte(address, NULL);
	SPIx_ReadWriteByte(data, NULL);

	SPI_CS = 1;
}

bool Mpu6500_MultiRead(uint8_t address, uint8_t *data, uint8_t len) {
	bool bSpi = true;
	int m;

	SPI_CS = 0;

	bSpi &= SPIx_ReadWriteByte(128 + address, NULL);
	for (m = 0; m < len; m = m + 1) {
		bSpi &= SPIx_ReadWriteByte(0, &(data[m]));
	}

	SPI_CS = 1;

	return bSpi;
}

bool Mpu6500_MultiWrite(uint8_t address, uint8_t *data, uint8_t len) {
	bool bSpi = true;
	int m;

	SPI_CS = 0;

	bSpi &= SPIx_ReadWriteByte(address, NULL);
	for (m = 0; m < len; m = m + 1) {
		bSpi &= SPIx_ReadWriteByte(data[m], NULL);
	}

	SPI_CS = 1;

	return bSpi;
}

int Mpu6500_Init(bool bWorkState) {
	int dmpRevtal = 0;

	SPIx_Init();

	if (bWorkState == MPU6500_CYCLE) {
		Mpu6500_EnterCycle();
	} else if (bWorkState == MPU6500_WORK) {
		dmpRevtal = AnBT_DMP_MPU6050_Init();
	}

	return dmpRevtal;
}

void Mpu6500_EnterCycle(void) {
	uint8_t tmpRegister;

	//---wake on motion intrrupt: Ps 29--//

	/*1.reset*/
	tmpRegister = Mpu6500_SingleRead(0x6b);
	tmpRegister |= 0x80;
	Mpu6500_SingleWrite(0x6b, tmpRegister);
	MPU_DELAY_WITHSLEEP(100);
	tmpRegister = 0x07;
	Mpu6500_SingleWrite(0x68, tmpRegister);
	MPU_DELAY_WITHSLEEP(100);
	/*2.cycle=0,sleep=0,standby=0*/
	tmpRegister = Mpu6500_SingleRead(0x6b);
	tmpRegister &= 0x8f;
	Mpu6500_SingleWrite(0x6b, tmpRegister);
	/*3.dis_gyrox=1,dis_gyroy=1,dis_gyroz=1*/
	tmpRegister = Mpu6500_SingleRead(0x6c);
	tmpRegister |= 0x07;
	tmpRegister &= 0x0c7;
	Mpu6500_SingleWrite(0x6c, tmpRegister);
	/*4.bandwidth 184hz*/
	tmpRegister = 0x01;
	Mpu6500_SingleWrite(0x1d, tmpRegister);
	/*5.enable motion interrupt*/
	tmpRegister = 0x40;
	Mpu6500_SingleWrite(0x38, tmpRegister);
	/*6.enable accel hardware intelligence*/
	tmpRegister = 0xc0;
	Mpu6500_SingleWrite(0x69, tmpRegister);
	/*7.set motion thredhold*/
	tmpRegister = 0x10;
	Mpu6500_SingleWrite(0x1f, tmpRegister);
	/*8.set frequency of wake up*/
	tmpRegister = 0x09;
	Mpu6500_SingleWrite(0x1e, tmpRegister);
	/*enable cycle mode*/
	tmpRegister = Mpu6500_SingleRead(0x6b);
	tmpRegister |= 0x20;
	Mpu6500_SingleWrite(0x6b, tmpRegister);

}

void Mpu6500_GetAcc(int16_t *acc) {
	uint8_t data[6];

	data[0] = Mpu6500_SingleRead(0x3b);
	data[1] = Mpu6500_SingleRead(0x3c);
	data[2] = Mpu6500_SingleRead(0x3d);
	data[3] = Mpu6500_SingleRead(0x3e);
	data[4] = Mpu6500_SingleRead(0e3f);
	data[5] = Mpu6500_SingleRead(0x40);

	acc[0] = (data[0] << 8) + data[1];
	acc[1] = (data[2] << 8) + data[3];
	acc[2] = (data[4] << 8) + data[5];

}

void Mpu6500_GetGyro(int16_t *gyro) {
	uint8_t data[6];

	data[0] = Mpu6500_SingleRead(0x43);
	data[1] = Mpu6500_SingleRead(0x44);
	data[2] = Mpu6500_SingleRead(0x45);
	data[3] = Mpu6500_SingleRead(0x46);
	data[4] = Mpu6500_SingleRead(0x47);
	data[5] = Mpu6500_SingleRead(0x48);

	gyro[0] = (data[0] << 8) + data[1];
	gyro[1] = (data[2] << 8) + data[3];
	gyro[2] = (data[4] << 8) + data[5];

}

int Mpu6500_ReadFifo(Mpu6500_DmpData_t* tData) {
	int retval;

	unsigned long sensor_timestamp;
	short sensors;
	unsigned char more;

	float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
	float Roll, Pitch, Yaw;

	/*尽可能快执行，不设置延时，否则fifo溢出*/
	retval = dmp_read_fifo(tData->arrGyro, tData->arrAcc, tData->arrQuat,
			&sensor_timestamp, &sensors, &more);

	if (sensors & INV_WXYZ_QUAT) {
		if (tData->bCalculateAngle) {
			q0 = tData->arrQuat[0] / q30;
			q1 = tData->arrQuat[1] / q30;
			q2 = tData->arrQuat[2] / q30;
			q3 = tData->arrQuat[3] / q30;
			Pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3; // pitch
			Roll = atan2(2 * q2 * q3 + 2 * q0 * q1,
					-2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3; // roll
			Yaw = atan2(2 * (q1 * q2 + q0 * q3),
					q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3;	//感觉没有价值，注掉
			//printf("pitch: %.2f    roll:%.2f		yaw:%.2f\r\n",Pitch,Roll,Yaw);		//普通串口输出

			//a=Pitch*100;
			//b=Roll*100;
			//c=Yaw*100;
			tData->arrAngle[0] = Pitch * 100;
			tData->arrAngle[1] = Roll * 100;
			tData->arrAngle[2] = Yaw * 100;
		}
	}
	if (sensors & INV_XYZ_ACCEL) {

	}
	if (sensors & INV_XYZ_GYRO) {

	}

	return retval;
}

void mpu6500_test(void) {
	/*姿态*/
//	short gyro[3], accel[3];
//	long quat[4];
//
//	int16_t ogyro[3], oacc[3];
//	short angle[3];

	Mpu6500_DmpData_t dmpData;

	Mpu6500_Init(MPU6500_CYCLE);
	dmpData.bCalculateAngle = true;

	delay_ms(1000);

	while(1)
	{
		if(SPI_INT == 0)
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		}

		printf("num0= %d\t",dmpData.arrAngle[0]);
		printf("num1= %d\t",dmpData.arrAngle[1]);
		printf("num2= %d\r\n",dmpData.arrAngle[2]);
		//Get_Gyro(ogyro);
		//delay_ms(10);

		Mpu6500_ReadFifo(&dmpData);

	}
}
