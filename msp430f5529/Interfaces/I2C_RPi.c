/* =========================================================
 *                    I 2 C _ R P i . c
 *
 *      Author: Lars Heinrichs           Date: Dec 2014
 *
 *   Set up of the UCB1 interface module for slave use
 *			 used for communicating with an RPi
 *
 *						MSP430F5529
 * =========================================================
 */

#include <msp430.h>
#include <I2C_RPi.h>

unsigned short *sendData;
unsigned short *sendDataArrayFullStart, *sendDataArrayReducedStart, *sendDatArrayFull0;

void I2C_Setup_RPi(short IsMaster, short adress){
	P4OUT = BIT1 + BIT2;
	P4REN |= BIT1 + BIT2;
	P4SEL |= BIT1 | BIT2;					// I2C Lanes (P4.1-SDA und P4.2-SCL) (Auf peripheriegerät stellen)
	UCB1CTL1 |= UCSWRST;                 	// USCI im Reset Status festhalten
	UCB1CTL0 = UCMODE_3 + UCSYNC;			// slave + I2C + synchroner Modus
	if (IsMaster){
		UCB1CTL0 |= UCMST;					// master mode
		UCB1CTL1 = UCSSEL_2 + UCSWRST;      // SMCLK = 1MHz, USCI nicht freigeben
		UCB1BR0 = 10;                  		// fSCL = SMCLK/10 = ~100kHz
		UCB1BR1 = 0;                   		// Nichts aktiverien, da kein krummer Teiler	} else {
		UCB1I2CSA = adress;					// Set I2C Slave Adress (0x34 as set with the RPi group) and enable its usage

	} else {
		UCB1I2COA = adress;						// Set I2C Slave Adress (0x34 as set with the RPi group) and enable its usage
	}
	UCB1CTL1 &= ~UCSWRST;         			// USCI nicht mehr im Reset Status festhalten
	UCB1IE = UCRXIE + UCTXIE;	// Interrupts für Empfangsbuffer einschalten
}

void RPi_SetSendData(unsigned short *sendMeFull, unsigned short *sendMeReduced)
{
	sendDatArrayFull0 = sendMeFull;
	sendDataArrayFullStart = sendMeFull;
	sendDataArrayReducedStart = sendMeReduced;
}

// Interrupt für den Empfang
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B1_VECTOR))) USCI_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
	unsigned short in;
	switch(__even_in_range(UCB1IV,12))
	{
		case 0: break;  // Vector  0: No interrupts
		case 2: break;	// Vector  2: ALIFG
		case 4: 		// Vector  4: NACKIFG
			break;
		case 6: break;	// Vector  6: STTIFG
		case 8: 		// Vector  8: STPIFG
			//UCB1IE &= ~ UCTXIE;
			UCB1IFG &= ~UCSTPIFG;
			sendDataArrayFullStart = sendDatArrayFull0;
			break;
		case 10:		// Vector 10: RXIFG
			in = UCB1RXBUF;
			switch(in){
			case RPI_MEAS_REQUEST:			// A measurement is requested, exit sleep state and run while(1) loop in main
							// Toggle green LED
				 __bic_SR_register_on_exit(LPM0_bits);	// exit sleep on return from interrupt
				break;
			case RPI_DATA_REQUEST2:
				//RPi_SetSendData((unsigned short *)RPi_SendData);
				sendData = sendDataArrayReducedStart;
				UCB1IE |= UCTXIE;	// Enable TX interrupts
				break;
			case RPI_DATA_REQUEST:
				P1OUT ^= BIT0;
				//RPi_SetSendData((unsigned short *)RPi_SendData);
				sendData = sendDataArrayFullStart;
				UCB1IE |= UCTXIE;	// Enable TX interrupts
				break;
			default:
				break;
			}
		break;
		case 12:		// Vector 12: TXIFG
//			P1OUT ^= BIT0;
//			if (*sendData == 0xFF) { *sendData = 0xAA;}
			UCB1TXBUF = *sendData++;
//			P1OUT ^= BIT0;
			break;
		default: break;
	}
}



