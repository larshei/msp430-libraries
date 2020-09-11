/* =========================================================
 *                       O n e W i r e . h
 *
 *      Author: Lars Heinrichs           Date: Oct 2014
 *
 * contains routines for reading and writing bits and bytes
 * over OneWire by using a gpio pin.
 * !1 adjust port and pin (OWPORTDIR, OWPORTIN ..) to your needs
 * !2 might need to adjust wait_us(x) to get the desired
 * !2 delays, recommendation: use and oscilloscope or logic
 * !2 analyzer, use a couple of different __delay_cycle durations
 * !2 and calculate a linear function based on this
 * =========================================================
 */

#include <msp430.h>

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#define 	OWPORTDIR	P1DIR
#define 	OWPORTIN	P1IN
#define 	OWPORTOUT	P1OUT
#define 	OWPORTREN	P1REN
#define  	OW_PIN		PIN3				// defines the pin the DS1820 are connected to

#define 	wait_us(x)		__delay_cycles(((33*x)>>5)-15); // you need to adjust this for your microcontroller!
#define 	OW_LO(OWPIN)	{ OWPORTDIR |= OWPIN ; OWPORTREN &= ~OWPIN ; OWPORTOUT &= ~OWPIN; }
#define 	OW_HI(OWPIN)	{ OWPORTDIR |= OWPIN ; OWPORTREN &= ~OWPIN ; OWPORTOUT |= OWPIN ; }
#define 	OW_RLS(OWPIN) 	{ OWPORTDIR &= ~OWPIN; OWPORTREN |= OWPIN  ; OWPORTOUT |= OWPIN ; }
#define 	OW_IN(OWPIN) 	( OWPORTIN & OWPIN )

short OW_Reset(short pin);
void OW_write_bit(short pin, short bit);
void OW_write(short pin, short byte);	// read a byte
unsigned short OW_read_bit(short pin);
unsigned short OW_read(short pin);		// write a byte

#endif /* ONEWIRE_H_ */
