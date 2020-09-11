/* =========================================================
 *                  I N I T I A L I Z E . c
 *
 *      Author: Lars Heinrichs           Date: Dec 2014
 *
 * Sets all port to low output to save energy
 * =========================================================
 */
#include <msp430.h>

void basic_init()
{	// set all ports to output with low value to save energy
	P1DIR = 0xFF;
	P1OUT = 0x00;
	P2DIR = 0xFF;
	P2OUT = 0x00;
	P3DIR = 0xFF;
	P3OUT = 0x00;
	P4DIR = 0xFF;
	P4OUT = 0x00;
	P5DIR = 0xFF;
	P5OUT = 0x00;
	P6DIR = 0xFF;
	P6OUT = 0x00;
	P7DIR = 0xFF;
	P7OUT = 0x00;
	P8DIR = 0xFF;
	P8OUT = 0x00;
}
