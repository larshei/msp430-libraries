#include <msp430.h>
#include <sleep.h>
#include <I2C_RPi.h>

unsigned short *RXValuesI2C1, *I2C1SendData; 	// Pointer to Receive Values
unsigned int ByteI2C1;			// byte count for I2C communication

/*****************************************************************************
 * I2C1_Init(int IsMaster, int adress)
 * Return: -
 * Param:  	(int)IsMaster	0 sets up I2C in slave mode, !0 sets up master mode
 * 			(int)adress		Sets the own I2C adress in slave mode or the remote
 * 							slaves I2C adress in master mode
 *
 * Initialises the USCI_B0/1 interface for I2C in either slave or master mode,
 * Acitaves Interrupts for TX and RX
 * In Master mode, selects SMCLK clock source, sets the clock rate to ~100 kHz
 *****************************************************************************/

void I2C1_Init(int IsMaster, int adress)
{
	P4REN |= BIT1 + BIT2;
	P4SEL |= BIT1 | BIT2;					// I2C Lanes (P4.1-SDA und P4.2-SCL) (Auf peripheriegerät stellen)
	UCB1CTL1 |= UCSWRST;                 	// USCI im Reset Status festhalten
	UCB1CTL0 = UCMODE_3 + UCSYNC;			// slave + I2C + synchroner Modus
	if (IsMaster){
		UCB1CTL0 |= UCMST;					// master mode
		UCB1CTL1 = UCSSEL_2 + UCSWRST;      // SMCLK = 1MHz, USCI nicht freigeben
		UCB1BR0 = 10;                  		// fSCL = SMCLK/10 = ~100kHz
		UCB1BR1 = 0;                   		// Nichts aktiverien, da kein krummer Teiler
		UCB0I2CSA = adress;					// set slave I2C adress
	} else {
		UCB1I2COA = 0x34;					// Set I2C Slave Adress (0x34 as set with the RPi group) and enable its usage
	}
	UCB1CTL1 &= ~UCSWRST;         			// USCI nicht mehr im Reset Status festhalten
	UCB1IE |= UCRXIE + UCTXIE;	// Interrupts für Empfangsbuffer einschalten
}




/*****************************************************************************
 * I2C1_SetSlaveAdress(short adress)
 * Return: -
 * Param:  (short) adress: Adress to set
 *
 * Sets the I2C slave adress of USCIB to 'adress'
*****************************************************************************/
void I2C1_SetSlaveAdress(short adress)
// sets the 7 bit client adress for the I2C communication
{
	UCB1I2CSA = adress;
}




/*****************************************************************************
 * I2C1_SendCommand(short command)
 * Return: -
 * Param:  	(short)command	command to send to the I2C bus
 *
 * Only in master mode! Generates a start signal, sends the I2C adress of the
 * desired device as set in I2C1_Init() or I2C1_SetSlaveAdress(), sets  the
 * Read/Write bit to write. Writes a single byte and generates a stop condition.
 *****************************************************************************/
void I2C1_SendCommand(short command)
{	// Sends a single command over the I2C bus
	UCB1CTL1 |= UCTXSTT | UCTR;		// generate I2C start signal (sends adress automtically)
	UCB1TXBUF = command;			// send command
	sleep(2);						// wait for transmit to finish
	UCB1CTL1 |= UCTXSTP;			// generate stop condition
}




/*****************************************************************************
 * I2C1_Read(short command)
 * Return: -
 * Param:  	(unsigned char)byteCount	reads the given amount of bytes (max 32)
 * 			(unsigned char*)storageArray	the read function will store the received
 * 										data starting at this pointer location
 *
 * Only in master mode!  Generates a start signal, sends the I2C adress
 * of the desired device as set in I2C1_Init() or I2C1_SetSlaveAdress(), sets
 * the Read/Write bit to read. Reads byteCount bytes from the bus.
 * interrupt based, so global interrupts and UCBI1IE |= UCRCIE need to be set
 * (second is set in I2C1_Init())
 *****************************************************************************/
void I2C1_Read(unsigned char byteCount, unsigned short *storageArray)
{	RXValuesI2C1 = storageArray;// setup pointer for usage in RX interupts
	ByteI2C1 = byteCount;
	UCB1CTL1 &= ~UCTR;		// Set to not Tramsmitter (=receiver mode)
	UCB1CTL1 |= UCTXSTT;	// generate I2C start signal (sends adress automatically)
	// Reading is done in interrupt routine (bottom of file)
}



/*****************************************************************************
 * I2C1_Read(short command)
 * Return: -
 * Param:  	(char)command				command to send to the I2C bus
 * 			(unsigned char)byteCount	reads the given amount of bytes
 * 			(unsigned char*)storageArray	the read function will store the received
 * 										data starting at this pointer location
 *
 * Only in master mode!  Generates a start signal, sends the I2C adress
 * of the desired device as set in I2C1_Init() or I2C1_SetSlaveAdress(), sets
 * the Read/Write bit to read. Reads byteCount bytes from the bus.
 * interrupt based, so global interrupts and UCBI1IE |= UCRCIE need to be set
 * (second is set in I2C1_Init())
 *****************************************************************************/
void I2C1_SendAndRead(char command, unsigned char byteCount, unsigned short *storageArray)
{	ByteI2C1 = byteCount;			// Set the number of bytes that are to be read
	RXValuesI2C1 = storageArray;	// setup pointer for usage in RX interupts
	UCB1CTL1 |= UCTXSTT | UCTR;	// generate I2C start signal (sends adress automtically)
	sleep(2);					// wait for transmit to finish
	UCB1TXBUF = command;		// send command
	sleep(2);					// wait for transmit to finish

	// Switch to read-mode
	UCB1CTL1 &= ~UCTR;		// Set mode to Not-Transmitter (Receiver mode)
	UCB1CTL1 |= UCTXSTT;	// generate I2C start signal (sends adress automtically)
	// Reading is done in interrupt routine (bottom of file)
}

/*****************************************************************************
 * I2C1_SetSendData(unsigned short *sendMe)
 * Return: -
 * Param:  	(unsigned short*)sendMe		pointer to storage array of inconing bytes
 *****************************************************************************/
void I2C1_SetSendData(unsigned short *sendMe)
{
	I2C1SendData = sendMe;
}



/******************************************************************
 ******************************************************************/
// Interrupt für den Empfang I2C1

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B1_VECTOR))) USCI_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{	unsigned short in;
	switch(__even_in_range(UCB1IV,12))
	{
		case 0: break;  // Vector  0: No interrupts
		case 2: break;	// Vector  2: ALIFG
		case 4: 		// Vector  4: NACKIFG
			break;
		case 6: break;	// Vector  6: STTIFG
		case 8: break;	// Vector  8: STPIFG
		case 10:		// Vector 10: RXIFG

			if (UCB1CTL0 & UCMST)		// AM I I2C MASTER?
			{	// If im the master, i have to continue generating clock signals until all bytes are received
				if (ByteI2C1) {
					*RXValuesI2C1++ = UCB1RXBUF; 	// Store RX at pointer destination (set in the read function)
					ByteI2C1--;
					if (ByteI2C1 == 1) {			// generate a stop condition after next receive has finished
						UCB1CTL1 |= UCTXSTP;
					}
				}
				else {
					*RXValuesI2C1++ = UCB1RXBUF; 	// Store RX at pointer destination (set in the read function)
					__no_operation();
				}
			} else						// AM I I2C SLAVE?
			{	// Store incoming byte
				in = UCB1RXBUF;
				// check if incoming byte is a command
				if (in == RPI_MEAS_REQUEST)
				{
					P1OUT ^= BIT0;
					__bic_SR_register_on_exit(LPM0_bits);
				}
				if (in == RPI_DATA_REQUEST)			// get a send request?
				{
					UCB1IE |= UCTXIE;	// Enable TX interrupts
				}
			}

		break;
		case 12:		// Vector 12: TXIFG
			UCB1TXBUF = *I2C1SendData++;
			break;
		default: break;
	}
}
