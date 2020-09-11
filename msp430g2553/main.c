#include <msp430.h> 
#include <sleep.h>
/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    DCOCTL = 0; // Select lowest DCOx and MODx
    BCSCTL1 = CALBC1_12MHZ; // Set range
    BCSCTL2 |= DIVS_0;	// set SMCLK divider to 8
    DCOCTL = CALDCO_12MHZ; // Set DCO step + modulation

    P1DIR |= BIT0 + BIT6;
    sleep_setClock(SMCLK, 12000000);	// Set timer clk src to SMCLK, 125KHz

	while (1) {
		sleep(50000, TENTHMILLISECONDS);
		__no_operation();
		P1OUT ^= BIT0;
	}
}



