/* =========================================================
 *                       B M P 0 8 5 . c
 *
 *      Author: Lars Heinrichs           Date: Feb 2015
 *
 * 	provides two easy to use functions to use a BMP085
 * 	with an MSP430F5529. Example programm and required
 * 	I2C_UCB0.c/h and sleep.c/h files can be found on
 * 				http://www.pointer-horror.de
 * =========================================================
 */


#include <I2C_G2553.h>
#include <sleep_G2553.h>
#include <BMP085_G2553.h>

unsigned short val[22];
#define 	append(x,y)		((x<<8)+(y & 0xFF))
#define 	NULL			0

void BMP_calibrate(sensor_BMP085 *sensor);


sensor_BMP085* BMP_init()
{	sensor_BMP085 *temp;
	temp = malloc(sizeof(sensor_BMP085));	// reserve memory
	if (temp == NULL) {return NULL;}		// not enough ram available, return

	I2C_setSlaveAdress(0x77);				// set BMP085 hardware I2C adress
	BMP_calibrate(temp);					// fetch calibration date of the sensor

	return temp;	// sensor has been added and calibration data fetched, return
}


void BMP_calibrate(sensor_BMP085 *sensor){
    I2C_sendAndRead(BMP_AC1, 22, (unsigned short *)val);	// read all 22 calibration bytes, starting at adress 0xAA
	sleep(400, TENTHMILLISECONDS);			// give enough time to read

	sensor->oss = 3;
	sensor->adress = 0x77;	// store sensor adress
	sensor->ac1 = append (val[0],  val[1]);	 // store all calibration data
	sensor->ac2 = append (val[2],  val[3]);  // ..
	sensor->ac3 = append (val[4],  val[5]);  // ..
	sensor->ac4 = append (val[6],  val[7]);  // ..
	sensor->ac5 = append (val[8],  val[9]);  // ..
	sensor->ac6 = append (val[10], val[11]); // ..
	sensor->b1  = append (val[12], val[13]); // ..
	sensor->b2  = append (val[14], val[15]); // ..
	sensor->mb  = append (val[16], val[17]); // ..
	sensor->mc  = append (val[18], val[19]); // ..
	sensor->md  = append (val[20], val[21]); // ..
}


void BMP_measure(sensor_BMP085 *sensor) {
	long x1, x2, x3, b3, b5, b6;		// temporary variables for calculation
	unsigned long b4, b7;
	if (sensor->oss > 3) {sensor->oss = 3;} else if (sensor->oss < 0) {sensor->oss = 0;}	// make sure were not out of range

	I2C_setSlaveAdress(sensor->adress);		// make sure correct I2C adress is set
	I2C_writeRegister(BMP_MEAS_REG, BMP_MEAS_TEMP);	// write value for temperatrue measurement into measure mode register on sensor
	sleep(1, TENTHMILLISECONDS);			// wait at least 4.5 ms for sensor to finish conversion
	I2C_sendAndRead(BMP_MEAS_MSB, 2, (unsigned short*)val); // set sensor internal pointer, then read 2 values
	sleep(10, TENTHMILLISECONDS);			// wait for I2C communication
	sensor->uT = append(val[0], val[1]);

	I2C_writeRegister(BMP_MEAS_REG, BMP_MEAS_PRESS+(sensor->oss<<6));	// write value for pressure measurement into measure mode register on sensor
	sleep(100, TENTHMILLISECONDS);			// wait at least 16 ms in oss=3 for sensor to finish conversion
	I2C_sendAndRead(BMP_MEAS_MSB, 3, (unsigned short*)val); // set sensor internal pointer, then read 2 values
	sleep(20, TENTHMILLISECONDS);			// wait for I2C communication to finish
	sensor->uP = (((long)val[0]<<16) + (val[1]<<8) + val[0]) >> (8 - sensor->oss);

	// calculate temperature (datasheet)
	x1 = ((sensor->uT - sensor->ac6) * sensor->ac5) >> 15;
	x2 = ((long) sensor->mc << 11) / (x1 + sensor->md);
	b5 = x1 + x2;
	sensor->T = (b5 + 8) >> 4;

	// calculate pressure (datasheet)
	b6 = b5 - 4000;		// b5 beinhaltet Temperatur
	x1 = (sensor->b2 * (b6 * b6 >> 12)) >> 11;
	x2 = (sensor->ac2 * b6) >> 11;
	x3 = x1 + x2;
//	b3 = (((long) sensor->ac1 * 4 + x3) << sensor->oss + 2 ) >> 2;	// from data sheet: leads to ~1/4 of the real pressure
	b3 = (((long) sensor->ac1 * 4 + x3) << sensor->oss) >> 2;	// corrected line from	http://elchs-kramkiste.de/lapurd/node/148 	09.07.2014
	x1 = sensor->ac3 * b6 >> 13;
	x2 = (sensor->b1 * (b6 * b6) >> 12) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = sensor->ac4 * (unsigned long) (x3 + 32768) >> 15;
	b7 = ((unsigned long) sensor->uP - b3) * (50000 >> sensor->oss);

	if (b7 < 0x8000000) {
		sensor->P = (b7 * 2) / b4;
	} else {
		sensor->P = (b7 / b4) * 2;
	}

	x1 = (sensor->P >> 8) * (sensor->P >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * sensor->P) >> 16;
	sensor->P = sensor->P + ((x1 + x2 + 3791) >> 4);
}
