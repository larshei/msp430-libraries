#include <msp430.h> 
#include <sleep_G2553.h>
#include <I2C_G2553.h>
#include <BMP085_G2553.h>
#include <SSD1306.h>

sensor_BMP085 *sensor;

unsigned short var = 0;

int i;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    DCOCTL = 0; 				// Select lowest DCOx and MODx
    BCSCTL1 = CALBC1_1MHZ; 		// Set range
    BCSCTL2 |= DIVS_1;			// set SMCLK divider to 2
    DCOCTL = CALDCO_1MHZ; 		// Set DCO step + modulation

    __delay_cycles(500);
    P1DIR |= BIT0 + BIT1;
    __enable_interrupt();
    sleep_setClock(SMCLK, AUTO);// Set timer clk src to SMCLK, 125KHz

    I2C_init(1, 0x77); // master mode, (adress not needed in MM)
    sensor = BMP_init();

    display_init();
    sleep(1, SECONDS);

    display_clear();
    __delay_cycles(300);
    display_setStartPage(1);
    display_setStartColumn(0);

    display_print("Salos & Flixxxxo\0",0,0,0);
    setStartPage(3);
	while (1) {
		//Testweise aktiviert lassen (Busfunktionalität überprüfen)
		BMP_measure(sensor);
		display_print("T: %0C\0",40,3,1,(int)sensor->T);
		//__delay_cycles(500);
		sleep(3000, TENTHMILLISECONDS);
		__no_operation();
	}
}
