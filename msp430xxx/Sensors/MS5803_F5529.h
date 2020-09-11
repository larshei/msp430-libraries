/*
 * MS5803clean.h
 *
 *  Created on: 03.12.2014
 *      Author: DemoniacMilk
 */

#ifndef MS5803CLEAN_H_
#define MS5803CLEAN_H_

#include <sleep.h>
#include <stdlib.h>
#include <msp430.h>
#include <UARTusb.h>

#define		MS5803_ADRESS_A		0x77
#define		MS5803_ADRESS_B		0x76

#define 	MS5803_PROM_1		0xA2
#define 	MS5803_PROM_2		0xA4
#define 	MS5803_PROM_3		0xA6
#define 	MS5803_PROM_4		0xA8
#define 	MS5803_PROM_5		0xAA
#define 	MS5803_PROM_6		0xAB
#define 	MS5803_RESET		0x1E
#define 	MS5803_ADC_READ		0x00
#define 	MS5803_CON_P_256	0x40
#define 	MS5803_CON_P_512	0x42
#define 	MS5803_CON_P_1024	0x44
#define 	MS5803_CON_P_2048	0x46
#define 	MS5803_CON_P_4096	0x48
#define 	MS5803_CON_T_256	0x50
#define 	MS5803_CON_T_512	0x52
#define 	MS5803_CON_T_1024	0x54
#define 	MS5803_CON_T_2048	0x56
#define 	MS5803_CON_T_4096	0x58

typedef struct sensor_properties sensor_properties;
struct sensor_properties
// stores the properties of a sensor
{	short SensorType;				// sensor type (0 BMP, 1 MS5803)
	short SensorAdress;				// I2C Sensor Adress
	long uP, uT, P, T, dT;			// measurement and calculation results
	unsigned int CalValues[8];		// data for calibration MS5803
};

// - - - -  F U N C T I O N S  - - - - - -
// hardware related functions
void I2C_Setup_Sensor();
void MS_Calibration(sensor_properties *sensor);
void MS_Measure(sensor_properties *sensor);
void MS_ReadTemperature(sensor_properties *sensor);
void MS_ReadPressure(sensor_properties *sensor);
long MS_MultiMeasure(int MeasureCount, sensor_properties *sensor, long *saveTo);
void SetI2C_Adress(short adress);
sensor_properties *newPressureSensor(short SensorType, short SensorAdress, short calibrate);


// calculations
void MS_calcPressure(sensor_properties *sensor);
void MS_calcTemperature(sensor_properties *sensor);

#endif /* MS5803CLEAN_H_ */
