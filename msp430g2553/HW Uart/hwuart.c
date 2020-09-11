/* MSP420G2553 HW UART Verbindung
 */

#include "msp430g2553.h"

void UARTSendArray(unsigned char *_sendArray, unsigned int _sendLength)
{
	while(_sendLength--)
	{
		while(!(IFG2 & UCA0TXIFG)); 			// Wait for TX buffer to be ready for new data
		UCA0TXBUF = *_sendArray; 			// Write the character at the location specified py the pointer
		_sendArray++; 					// Increment the TxString pointer to point to the next character
	}
}

unsigned char data;

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;				// Stop watchdog timer (10.3.1)

	/* Configure Digital Clock */
	BCSCTL1 = CALBC1_1MHZ; 					// DCO Calibraton Data for 1MHz (24.2.1)
	DCOCTL = CALDCO_1MHZ; 					// DCO Calibraton Data for 1MHz (24.2.1)

	/* Configure HW UART */
	P1SEL = BIT1 + BIT2 ; 					// Secondary periphal modul function selected P1.1 = RXD, P1.2=TXD (8.2.5)
	P1SEL2 = BIT1 + BIT2 ; 					// Secondary periphal modul function selected P1.1 = RXD, P1.2=TXD (8.2.5)
	UCA0CTL1 |= UCSSEL_2; 					// UCA Control, use SMCLK Clock Source (15.4.2)
	UCA0BR0 = 104; 						// 9600 Baud with 1MHz clock (15.3.13)
	UCA0CTL1 &= ~UCSWRST; 					// Software Reset, Initialize USCI state machine (15.4.2)
	IE2 |= UCA0RXIE; 					// Enable USCI_A0 RX interrupt (15.3.15.2)

	UARTSendArray("< MSP430 HW UART Console: \n\r", 28);
	__bis_SR_register(LPM0_bits + GIE); 			// Enter LPM0, interrupts enabled
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	data = UCA0RXBUF;					// RX Data
	UARTSendArray("> ", 2);
	UARTSendArray(&data, 1);				// Write back RX data
}
