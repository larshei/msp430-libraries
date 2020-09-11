/* =========================================================
 *                       D S 1 8 2 0 o w n . c
 *
 *      Author: Lars Heinrichs           Date: Nov 2014
 *
 * Functions for using a DS1820 Temperature Sensor. Needs
 * the "OneWire.c - Lars Heinrichs" - or another OneWire
 * library (might need to adjust function names for OneWire
 * functionalities in that case.)
 * =========================================================
 */

#include <DS1820own.h>
#include <msp430.h>
#include <sleep.h>
#include <OneWire.h>

short retMe;
unsigned short OW_recData[9], DS1820_countRemain, DS1820_countPerC;


/****************************************************************************************
 * Reads the temperature value of a OneWire Bus with a single DS1820 connected
 * Does not work for multiple OneWire decives on the same bus, because the
 * device adresses are skipped!
 * - pin: Pin number on port OWPORT (set in DS1820own.h) used for OneWire data transfer
****************************************************************************************/
int DS1820_ReadTemperatureSingle(short pin)
// Rückgabe des Temperaturwertes in 0.5°C
{	int returnMe = 0;							// Return value
	unsigned short i;							// loop variable
	if(!OW_Reset(pin)) 							// Issue Reset command (returns 0 if d detected)
	{	// If an answer (Presence Puls) has been received
	//	P4OUT |= BIT7; P1OUT &= ~BIT0;			// Turn green LED on, red LED off (signal operation)
		OW_write(pin, DS1820_SKIP_ROM);			// ROM Command: skip adressing
		OW_write(pin, DS1820_CONVERT_T);		// FUNCTION command: start temperature conversion
		sleep(50);								// conversion time (obsolete?)
		OW_Reset(pin);							// Reset
		OW_write(pin, DS1820_SKIP_ROM);			// ROM Command: skip adressing
		OW_write(pin, DS1820_READ_SCRATCHPAD);	// FUNCTION command: read scratchpadd
		for (i=0;i<9;i++){					// read 9 bytes (size of scratchpad)
			OW_recData[i] = OW_read(pin);		//call function to read a byte over 1wire
		}
		DS1820_countRemain = OW_recData[6];		// Store value for calculation
		DS1820_countPerC = OW_recData[7];		// Store value for calculation

		// Calculate temperature
		returnMe = (OW_recData[0] + 1 - (((DS1820_countPerC-DS1820_countRemain)/DS1820_countPerC)));
	} else
	{	// No Presence pulse detected, no working sensor connected?
	//	P4OUT &= ~BIT7; P1OUT |= BIT0;			// Green LED off, Red LED on to singal failure
	}
	OW_HI(pin)									// Take bus line control and set line to default high
	return returnMe;							// return temperature
}


/****************************************************************************************
 * Reads the temperature value of DS1820 with known adresses connected to the Bus
 * - pin: 	Pin number on port OWPORT (set in DS1820own.h) used for OneWire data transfer
 * - count:	Number of sensors on the Bus to read
 * - *temperatureArray: Pointer to array to store data in
 * - *adresses: Pointer to array containing adresses (use 64 Bit data type to store at once)
****************************************************************************************/
void DS1820_ReadTemperatureMulti(short pin, unsigned short count, int *temperatureArray, unsigned char *adresses)
// Rückgabe des Temperaturwertes in 0.5°C
{
	unsigned short byte_i, sensor_i;			// loop variables
	if(!OW_Reset(pin)) 							// Reset durchführen, liefert 0 zurück, wenn ein Sensor vorhanden
	{	// Antwort eines Sensors erhalten (Presence Puls)
//		P4OUT |= BIT7; P1OUT &= ~BIT0;			// Turn green LED on, red LED off (signal operation)
		OW_write(pin, DS1820_SKIP_ROM);			// ROM Command: Skip adress
		OW_write(pin, DS1820_CONVERT_T);		// FUNCTION command: start temperature conversion on all sensors
		sleep(50);								// conversion time (obsolete?)
		adresses+=7;							// Jump to LSByte (Adress is sent LSbit first)
		for (sensor_i = 0; sensor_i<count ; sensor_i++){
			OW_Reset(pin);						// Reset
			OW_write(pin, DS1820_MATCHROM);		// ROM Command: Match rom

			for (byte_i=8;byte_i>0;byte_i--){	// write 8 Byte (= 64 bit adress)
				OW_write(pin, *adresses--);		// Call function to write bytes over 1wire, send adress byte, next time, send more significant byte (--)
			}
			adresses+=16;						// we are now one spot before the MSByte of the last adress (position -1). Go to position 15
			OW_write(pin, DS1820_READ_SCRATCHPAD);	// FUNCTION command: read scratchpad
			for (byte_i=0;byte_i<9;byte_i++){		// read 9 bytes (size of scratchpad)
				OW_recData[byte_i] = OW_read(pin);	// Cal function to read bytes over 1wire
			}
			DS1820_countRemain = OW_recData[6];		// Store value for calculation
			DS1820_countPerC = OW_recData[7];		// Store value for calculation

			// Berechnung
			*temperatureArray++ = (OW_recData[0] + 1 - (((DS1820_countPerC-DS1820_countRemain)/DS1820_countPerC)));
			sleep(50);
		}


	} else
	{	// No Presence pulse detected, no working sensor connected?
	//	P4OUT &= ~BIT7; P1OUT |= BIT0;			// Green LED off, Red LED on to singal failure
	}
	OW_HI(pin)									// Take bus line control and set line to default high
}


/****************************************************************************************
 * Reads the adress of a single OneWire device on the Bus
 * - pin: Pin number on port OWPORT (set in DS1820own.h) used for OneWire data transfer
 * -- return: (long long) 64 Bit Adress of OneWire device
****************************************************************************************/
long long DS1820_GetAdress(unsigned short pin){
	long long returnMe = 0;//, returnMeFinal = 0;
	unsigned short i;
	OW_Reset(pin);
	OW_write(pin, DS1820_READROM);			// ROM Command: Read ROM (only works for single devices
	wait_us(100);
	for (i=8;i>0;i--){						// Read 8 times (64 bit adress)
		returnMe += (long long)OW_read(pin)<<(8*(i-1));			// read single Bytes and add them to return value
	}
	return returnMe;						// return result
}
