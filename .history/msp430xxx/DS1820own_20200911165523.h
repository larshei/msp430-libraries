/* =========================================================
 *                       D S 1 8 2 0 o w n . h
 *
 *      Author: Lars Heinrichs           Date: Nov 2014
 *
 * Functions for using a DS1820 Temperature Sensor. Needs
 * the "OneWire.c - Lars Heinrichs" - or another OneWire
 * library (might need to adjust function names for OneWire
 * functionalities in that case.)
 * =========================================================
 */


#ifndef DS1820OWN_H_
#define DS1820OWN_H_

// Kurzschreibweisen für das Setzen auf High/Low, Release der Leitung, Pinwert bestimmen
// OWPIN wird über Funktionsaufrufe gesetzt und muss in der .c als Variable deklariert und nach hier übergeben werden!!!


#define FALSE 0
#define TRUE  1
#define DS1820_WRITE_SCRATCHPAD 	0x4E
#define DS1820_READ_SCRATCHPAD      0xBE
#define DS1820_COPY_SCRATCHPAD 		0x48
#define DS1820_READ_EEPROM 			0xB8
#define DS1820_READ_PWRSUPPLY 		0xB4
#define DS1820_SEARCHROM 			0xF0
#define DS1820_SKIP_ROM             0xCC
#define DS1820_READROM 				0x33
#define DS1820_MATCHROM 			0x55
#define DS1820_ALARMSEARCH 			0xEC
#define DS1820_CONVERT_T            0x44

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * DS1820 Scratchpad:
 * Byte0	Temperature LSB
 * Byte1	Temperature MSB
 * Byte2	TH User Byte
 * Byte3	TL User Byte
 * Byte4	Reserved
 * Byte5	Reserved
 * Byte6	Count Remain
 * Byte7	Count Per °C
 * Byte8	CRC
 */

// Variables - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/************************************************************
* Adresses of each sensor need to be stored in an array     *
* Use function DS1820_GetAdress() while a single OneWire    *
* device is connecteed to the bus to get the device adress  *
* Put adress into the array below						    *
*************************************************************

// Stores the 64 Bit Adress of the DS1820 / adresses have been determined manually
//static unsigned long long OW_Adresses[0] = {};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Functions
short OW_Reset(short pin);
void  OW_write_bit(short pin, short bit);
void  OW_write(short pin, short byte);
unsigned short OW_read_bit(short pin);
unsigned short OW_read(short pin);
int DS1820_ReadTemperatureSingle(short pin);
long long DS1820_GetAdress(unsigned short pin);
void DS1820_ReadTemperatureMulti(short pin, unsigned short count, int *temperatureArray, unsigned char *adresses);

#endif /* DS1820OWN_H_ */
