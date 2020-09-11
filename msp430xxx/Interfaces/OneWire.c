/* =========================================================
 *                       O n e W i r e . h
 *
 *      Author: Lars Heinrichs           Date: Oct 2014
 *
 * contains routines for reading and writing bits and bytes
 * over OneWire by using a gpio pin.
 * Set the used port in OneWire.h (OWPORTDIR, OWPORTIN ..)
 * =========================================================
 */

#include <sleep.h>
#include <OneWire.h>

/****************************************************************************
 * Sends a reset pulse to the one wire bus
 * - pin: pin number on port OWPORT (set in header) that is used for OneWire data
 * -- return: (short) returns 0 if presence of a OneWire device has been detected
****************************************************************************/
short OW_Reset(short pin)
// Master Reset Puls
{	OW_LO(pin)					// pull pin low
	wait_us(500);				// wait at least 420µs
	OW_RLS(pin)					// Release pin, give line control to slave
	wait_us(80);				// Slave answers after 15-60 µs
	if (OW_IN(pin)) return 1;	// Slave must pull line to low (before presence pulse)
	wait_us(200);				// wait for presence pulse time slot to finish
	if (!OW_IN(pin)) return 2;	// check for presence pulse
	return 0;
}

/****************************************************************************
 * Writes a single bit to the OneWire bus
 * - pin: pin number on port OWPORT (set in header) that is used for OneWire data
 * - bit: value to send
****************************************************************************/
void OW_write_bit(short pin, short bit)
// Einzelnes Bit setzen
{	OW_LO(pin)					// Pull low to activate slave
	wait_us(20);				// wait time
	if(bit){OW_HI(pin)}			// put valueon line
	wait_us(70);				// let time slot finish
	OW_HI(pin)					// set to defualt high
	wait_us(20);				// wait
}

/****************************************************************************
 * Writes a byte to the OneWire bus
 * - pin:  pin number on port OWPORT (set in header) that is used for OneWire data
 * - byte: data byte to send
****************************************************************************/
void OW_write(short pin, short byte)
// Writes a byte using a for-loop
{	unsigned short i;			// loop variable
	for(i=0 ; i<8 ; i++)		// 8 Bits = 1 Byte
	{
		OW_write_bit(pin, byte&(0x01<<i));	// Set each bit individually,
				// moving 1 bit mask & value to set either N or 0
	}
	wait_us(30);		// slave recovery time
}

/****************************************************************************
 * Reads a single bit from the OneWire bus
 * - pin: pin number on port OWPORT (set in header) that is used for OneWire data
 * -- return: (unsigned short) returns a non-zero value if a bit was read
****************************************************************************/
unsigned short OW_read_bit(short pin)
// Liest ein einzelnes bit vom Bus aus
{	short returnMe;				// Return Value
	OW_LO(pin)					// Pin to low (start)
	OW_HI(pin)					// Immediate pin to high -> sensor knows its read
	OW_RLS(pin)					// Release line to give control to slave
	returnMe = OW_IN(pin);		// Read value of line
	wait_us(120);				// wait for time slot to finish
	OW_HI(pin)					// put line to default high
	return returnMe;			// return bit
}

/****************************************************************************
 * Reads a single byte from the OneWire bus
 * - pin: pin number on port OWPORT (set in header) that is used for OneWire data
 * -- return: (unsigned short) returns the byte that has been read
****************************************************************************/
unsigned short OW_read(short pin)
// Reads a bit from the bus (LSB first)
{	unsigned short i, returnMe;	// Zählvariable, Rückgabewert
	returnMe = 0;				// Rückgabewert auf 0000 0000 setzen, da später nur ODER Verknüpfungen
	for(i=0 ; i<8 ; i++)		// 8 Bits = 1 Byte einlesen
	{
		if (OW_read_bit(pin)){  // in case read_bit returns a non-zero value,
		returnMe |= 1<<i;}		// put 1 at current position in byte
		sleep(1);				// let slave recover
	}
	return returnMe;			// return Byte
}
