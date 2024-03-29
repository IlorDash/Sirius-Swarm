    
//Либа для ДУС
#include "ITG3205.h"
#include "QMC5883L.h"

#define GYRO 0x68 //  когда AD0 подключен к GND, адресс гироскопа 0x68
//#define GYRO 0x69   когда AD0 подключен к GND, адресс гироскопа 0x69
#define GYRO_ADDR 0x1D
#define G_SMPLRT_DIV 0x15
#define G_DLPF_FS 0x16
#define G_INT_CFG 0x17
#define G_PWR_MGM 0x3E
#define G_TO_READ 6 // по 2 байта на каждую ось x, y, z
/* Смещение (менять не нужно) ******/
int g_offx = 0; /* Смещение по x */
int g_offy = 0; /* Смещение по y */
int g_offz = 0; /* Смещение по z */

/* Буфера данных (менять не нужно) *********************/
byte buff[G_TO_READ]; /* Буфер сырых данных гироскопа */
int rawDataGyro[3];
// Калибровка ************************************************************************************************/
void GyroCalibrate() {
	/* Переменные временного смещения *****************/
	static float tmpx = 0; /* временное смещения по x*/
	static float tmpy = 0; /* временное смещения по y*/
	static float tmpz = 0; /* временное смещения по z*/

	/* Определние смещения */
	for (uint8_t i = 0; i < 10; i++) /* Берем среднее значение с 10 считываний */
	{								 /* */
		HAL_Delay(10);
		I2Cdev_readBytes(GYRO, GYRO_ADDR, 6, buff, 0);

		rawDataGyro[0] = ((buff[0] << 8) + buff[1]);
		rawDataGyro[1] = ((buff[2] << 8) + buff[3]);
		rawDataGyro[2] = ((buff[4] << 8) + buff[5]);

		for (int i = 0; i < 3; i++) {
			if (rawDataGyro[i] > 32768) {
				rawDataGyro[i] = rawDataGyro[i] - 65536; //проверка на переполнение
			}
		}

		tmpx += rawDataGyro[0]; 
		tmpy += rawDataGyro[1]; 
		tmpz += rawDataGyro[2]; 
	}							
	/* и делим на текущее значение *************************************/
	g_offx = tmpx / 10;
	g_offy = tmpy / 10;
	g_offz = tmpz / 10;

	switch (SAT_NUMBER){
		case 1: 
            g_offx = -61;
            g_offy = 3; 
            g_offz = 1; 
            break;
        case(2):
            g_offx = 27; 
            g_offy = -259;
            g_offz = 34; 
            break;
        case(3):
            g_offx = 20;
            g_offy = -7;
            g_offz = 35;
            break;
        default:
          break;
    } 
}
/*************************************************************************************************************/

/* ИНИЦИАЛИЗАЦИЯ ГИРОСКОПА ************************/
void initGyro() {
	/*****************************************
	 * ITG 3200
	 * power management set to:
	 * clock select = internal oscillator
	 * no reset, no sleep mode
	 * no standby mode
	 * sample rate to = 125Hz
	 * parameter to +/- 2000 degrees/sec
	 * low pass filter = 5Hz
	 * no interrupt
	 ******************************************/

	/* Настройка параметров (лучше не менять) ****************************************/
	I2Cdev_writeByte(GYRO, G_PWR_MGM, 0x00);	/* */
	I2Cdev_writeByte(GYRO, G_SMPLRT_DIV, 0x07); // EB, 50, 80, 7F, DE, 23, 20, FF   /
	I2Cdev_writeByte(GYRO, G_DLPF_FS, 0x1E);	// +/- 2000 dgrs/sec, 1KHz, 1E, 19 /
	I2Cdev_writeByte(GYRO, G_INT_CFG, 0x00);	/* */
	GyroCalibrate();							// калибровка
}

/* Считывание данных с гироскопа (записывается в result[]) *****/
void getGyroscopeData(float *result) {
	/***************************************
	Gyro ITG-3200 I2C
	registers:
	temp MSB = 1B, temp LSB = 1C
	x axis MSB = 1D, x axis LSB = 1E
	y axis MSB = 1F, y axis LSB = 20
	z axis MSB = 21, z axis LSB = 22
	**************************************/

	I2Cdev_readBytes(GYRO, GYRO_ADDR, 6, buff, 0);

	rawDataGyro[0] = ((buff[0] << 8) + buff[1]);
	rawDataGyro[1] = ((buff[2] << 8) + buff[3]);
	rawDataGyro[2] = ((buff[4] << 8) + buff[5]);
	for (int i = 0; i < 3; i++) {
		if (rawDataGyro[i] > 32768) {
			rawDataGyro[i] = rawDataGyro[i] - 65536; //проверка на переполнение
		}

		result[0] = (rawDataGyro[0] - g_offx) / 14.375; /* Скорость вращения по x*/
		result[1] = (rawDataGyro[1] - g_offy) / 14.375; /* Скорость вращения по y*/
		result[2] = (rawDataGyro[2] - g_offz) / 14.375; /* Скорость вращения по z*/
	}
}
/**********************************************/

void resetGyroscope() {
    //    int regAddress = 0x1D;
    //	I2Cdev_readBytes(GYRO, regAddress, 6, buff, 0);
}