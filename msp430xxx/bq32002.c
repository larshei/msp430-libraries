/*
 * bq32002.c
 *
 *  Created on: 30.03.2015
 *      Author: DemoniacMilk
 */
#include <I2C_G2553.h>
#include <BQ32002.h>



unsigned short BQ_readSeconds(){
	unsigned short a;
	I2C_setSlaveAdressNC(BQ_I2C_ADDRESS);
	a = I2C_readRegister(BQ_SECONDS);
	a = ((a & BQ_10SEC_M) >> 4)*10 + (a & BQ_1SEC_M);
	return a;
}

unsigned short BQ_readMinutes(){
	unsigned short a;
	I2C_setSlaveAdressNC(BQ_I2C_ADDRESS);
	a = I2C_readRegister(BQ_MINUTES);
	a = ((a & BQ_10SEC_M) >> 4)*10 + (a & BQ_1SEC_M);
	return a;
}


unsigned short BQ_readHours(){
	unsigned short a;
	I2C_setSlaveAdressNC(BQ_I2C_ADDRESS);
	a = I2C_readRegister(BQ_CENT_HR);
	a = ((a & 0x30) >> 4)*10 + (a & BQ_1SEC_M);
	return a;
}

unsigned short BQ_readDays(){
	unsigned short a;
	I2C_setSlaveAdressNC(BQ_I2C_ADDRESS);
	a = I2C_readRegister(BQ_DATE);
	a = ((a & 0x30) >> 4)*10 + (a & BQ_1SEC_M);
	return a;
}

unsigned short BQ_readMonths(){
	unsigned short a;
	I2C_setSlaveAdressNC(BQ_I2C_ADDRESS);
	a = I2C_readRegister(BQ_MONTH);
	a = ((a & 0x10) >> 4)*10 + (a & BQ_1SEC_M);
	return a;
}

unsigned short BQ_readYears(){
	unsigned short a;
	I2C_setSlaveAdressNC(BQ_I2C_ADDRESS);
	a = I2C_readRegister(BQ_YEARS);
	a = ((a & 0xF0) >> 4)*10 + (a & 0x0F);
	return a;
}

