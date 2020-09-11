/*
 * BMP085.h
 *
 *  Created on: 10.02.2015
 *      Author: DemoniacMilk
 */


#ifndef BMP085_H_
#define BMP085_H_

#include <stdlib.h>

typedef struct sensor_BMP085 sensor_BMP085;

struct sensor_BMP085
{	short adress, oss;						// I2C Sensor Adress
	long uP, uT, P, T;						// measurement and calculation results
	int ac1, ac2, ac3, b1, b2, mb, mc, md;	// Calibration BMP085
	unsigned int ac4, ac5, ac6;				// Calibration BMP085

};

#define BMP_ADRESS_7		0x77	// 7-Bit sensor adress (+R/W bit = 1 Byte on UCSTT)

// Messmodi und -Ergebnisse
#define BMP_MEAS_MODE		0xF4	// Adresse, auf der der Messmodus ausgewählt wird
#define BMP_MEAS_PRESS		0x34	// Wert für Druckmessung
#define BMP_MEAS_TEMP		0x2E	// Wert fur Tempmessung
#define BMP_MEAS_MSB		0xF6	// Adresse zum Auslesen des Ergebnisses der Messung
#define BMP_MEAS_LSB		0xF7	// Adresse zum Auslesen des Ergebnisses der Messung

// Kalibrierungsdaten
#define BMP_AC1				0xAA
#define BMP_AC2				0xAC
#define BMP_AC3				0xAE
#define BMP_AC4				0xB0
#define BMP_AC5				0xB2
#define BMP_AC6				0xB4
#define BMP_B1				0xB6
#define BMP_B2				0xB8
#define BMP_MB				0xBA
#define BMP_MC				0xBC
#define BMP_MD				0xBE

sensor_BMP085* BMP_init();
void BMP_measure(sensor_BMP085 *sensor);


#endif /* BMP085_H_ */


/*
#include <msp430.h>
#include <sleep.h>
#include <BMP085.h>
#include <I2C_UCB0.h>

sensor_BMP085 *BMP085;	// pointer to a struct of type sensor_BMP085

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P1DIR |= BIT0;				// for blinking LED
    sleep_Setup();				// Setup sleep timer
    I2C0_Init(1, 0x77);			// initialize I2C (P3.0 SDA and P3.1 SCL)
    BMP085 = BMP_init();		// initialize a BMP085 including calibration

    while(1){
    	BMP_measure(BMP085);	// measure pressure and temperature, stores values in BMP085->T and BMP085->P
    	P1OUT ^= BIT0;			// toggle LED
    	sleep(400);				// delay in Low Power Mode (100 ms)
    }
}

 *
 * */
