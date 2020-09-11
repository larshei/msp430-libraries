/*
 * Si7021.h
 *
 *  Created on: 06.12.2014
 *      Author: DemoniacMilk
 */

#ifndef SI7021_H_
#define SI7021_H_

int Si_GetTempreatureOfLastMeasurement();
unsigned int Si_MeasureHumidity();
void Si_Setup();

#define SI_ADRESS				0x40

// commands
#define SI_MEAS_HUM_HOLD		0xE5
#define SI_MEAS_HUM				0xF5
#define SI_MEAS_TEMP_HOLD		0xE3
#define SI_MEAS_TEMP			0xF3
#define SI_READ_TEMP_PREV		0xE0
#define SI_RESET				0xFE
#define SI_WRITE_USER			0xE6
#define SI_READ_USER			0xE7

// options for user register
	//sets up the bit resolution for relative humidity and temperature measurement
#define SI_12RH_14T				0x00			// wait > 23ms after ordering the measurement
#define SI_08RH_12T				0x01			// wait > 7ms after ordering the measurement
#define SI_10RH_13T				0x80			// wait > 11ms after ordering the measurement
#define SI_11RH_11T				0x81			// wait > 10ms after ordering the measurement
#define SI_HEATENA				0x40			//  enable internal heater
#define SI_HEATDIS				0x00			// disable internal heater


#endif /* SI7021_H_ */
