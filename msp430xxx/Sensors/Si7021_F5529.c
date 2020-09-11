/*
 * Si7021.c
 *
 *  Created on: 06.12.2014
 *      Author: DemoniacMilk
 */

#include <msp430.h>
#include <sleep.h>
#include <I2C_UCB0.h>
#include <Si7021.h>

unsigned short ReceiveValues[2];
long returnMe;

/*****************************************************************
 * Si_Setup()
 * Return:	-
 * Param: 	-
 *
 * Initializes the Si7021 by writing into a users register
 *****************************************************************/
void Si_Setup(){
	I2C0_SetSlaveAdress(SI_ADRESS);
	I2C0_WriteRegister(SI_WRITE_USER, SI_11RH_11T);
}

/*****************************************************************
 * Si_Setup()
 * Return:	(int) relative humidity Value in %
 * Param: 	-
 *
 * Orders a humidity measurement. Automatically starts a temperature
 * measurement as well. Returns the relative humidity in %.
 * Temperature can be fetched by Si_GetTempreatureOfLastMeasurement()
 *****************************************************************/
unsigned int Si_MeasureHumidity(){
	unsigned int returnMe;

	I2C0_SetSlaveAdress(SI_ADRESS);
	I2C0_SendCommand(SI_MEAS_HUM);
	sleep(80);
	I2C0_Read(2,(unsigned short*)ReceiveValues);
	sleep(10);

	returnMe = ((unsigned int)ReceiveValues[0] << 8) + (ReceiveValues[1]&0xFF) ;
	returnMe = ((125 * (long)returnMe)>>16) - 6;
	if ((returnMe < 105) && (returnMe > 100)) {returnMe = 100;}
	return returnMe;
}

/*****************************************************************
 * Si_GetTempreatureOfLastMeasurement()
 * Return:	(int) Temperature Value in °C
 * Param: 	-
 *
 * Reads the temperature value of the previous measurement
 *****************************************************************/
int Si_GetTempreatureOfLastMeasurement(){
	int returnMeInt;

	I2C0_SetSlaveAdress(SI_ADRESS);
	I2C0_SendAndRead(SI_READ_TEMP_PREV, 2, (unsigned short*)ReceiveValues);
	sleep(10);

	returnMe = (ReceiveValues[0]<<8) + (ReceiveValues[1]&0xFF);
	returnMe = ((17672*returnMe)>>16) - 4685;

	returnMeInt = returnMe & 0xFFFF;
	return returnMeInt;
}

