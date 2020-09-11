/* =========================================================
 *                   I 2 C _ U C B 0 . c
 *
 *      Author: Lars Heinrichs           Date: Dec 2014
 *
 * Library for using the UCB0 module of the MSP430F5529.
 * There is a second library for using the UCB1 module.
 * Defines the interrupt for the UCB module as well.
 * To use the library: call I2C0_Init in your initilization,
 * then
 * =========================================================
 */

#include <msp430.h>
#include <sleep.h>

unsigned short *RXValuesI2C0; 	// Pointer to Receive Values
unsigned int ByteI2C0;			// byte count for I2C communication

/*****************************************************************************
 * I2C0_Init(int IsMaster, int adress)
 * Return: -
 * Param:  	(int)IsMaster	0 sets up I2C in slave mode, !0 sets up master mode
 * 			(int)adress		Sets the own I2C adress in slave mode or the remote
 * 							slaves I2C adress in master mode
 *
 * Initialises the USCI_B0/1 interface for I2C in either slave or master mode,
 * Acitaves Interrupts for TX and RX
 * In Master mode, selects SMCLK clock source, sets the clock rate to ~100 kHz
 *****************************************************************************/
void I2C0_Init(int IsMaster, int adress)
{
	P3REN &= ~(BIT1 + BIT0);
//	P3DIR = BIT1 + BIT0;
//	P3REN = BIT1 + BIT0;

	P3SEL |= BIT1 | BIT0;					// I2C Lanes (P3.0-SDA and P3.1-SCL) (pass pins to peripheral module)
	UCB0CTL1 |= UCSWRST;                 	// set USCI to reset state to allow glitch-free setup
	UCB0CTL0 = UCMODE_3 + UCSYNC;			// I2C + synchronous Mode (non-Master mode)
	if (IsMaster){
		UCB0CTL0 |= UCMST;					// Set to master mode
		UCB0CTL1 = UCSSEL_2 + UCSWRST;      // SMCLK = 1MHz, clock source select, keep UCB0 in reset state
		UCB0BR0 = 15;                  		// divider LSB: fSCL = SMCLK/10 = ~100kHz
		UCB0BR1 = 0;                   		// divider MSB
		UCB0I2CSA = adress;					// set slave I2C adress
	} else {
		UCB0I2COA = adress;					// Set own I2C Adress
	}
	UCB0CTL1 &= ~UCSWRST;         			// Release module from reset status
	UCB0IE = UCRXIE + UCTXIE + UCNACKIE;				// Activates RX and TX interrupts
}

void I2C0_EnablePullups(void){
	P3REN |= (BIT1 + BIT0);
}

/*****************************************************************************
 * I2C0_SetSlaveAdress(short adress)
 * Return: -
 * Param:  (short) adress: Adress to set
 *
 * Sets the I2C slave adress of USCIB to 'adress'
*****************************************************************************/
void I2C0_SetSlaveAdress(short adress)
// sets the 7 bit client adress for the I2C communication
{
	UCB0I2CSA = adress;
}




/*****************************************************************************
 * I2C0_SendCommand(short command)
 * Return: -
 * Param:  	(short)command	command to send to the I2C bus
 *
 * Only in master mode! Generates a start signal, sends the I2C adress of the
 * desired device as set in I2C0_Init() or I2C0_SetSlaveAdress(), sets  the
 * Read/Write bit to write. Writes a single byte and generates a stop condition.
 *****************************************************************************/
void I2C0_SendCommand(short command)
{	// Sends a single command over the I2C bus
	UCB0CTL1 |= UCTXSTT | UCTR;		// generate I2C start signal (sends adress automtically)
	UCB0TXBUF = command;			// send command
	sleep(2);						// wait for transmit to finish
	UCB0CTL1 |= UCTXSTP;			// generate stop condition
}

/*****************************************************************************
 * I2C0_WriteRegister(short command)
 * Return: -
 * Param:  	(short)command	command to send to the I2C bus
 *
 * Only in master mode! Generates a start signal, sends the I2C adress of the
 * desired device as set in I2C0_Init() or I2C0_SetSlaveAdress(), sets  the
 * Read/Write bit to write. Writes two bytes (register/command and value and generates a stop condition.
 *****************************************************************************/
void I2C0_WriteRegister(short reg, short value)
{	// Sends a single command over the I2C bus
	UCB0CTL1 |= UCTXSTT | UCTR;		// generate I2C start signal (sends adress automtically)
	UCB0TXBUF = reg;			// send register
	sleep(2);						// wait for transmit to finish
	UCB0TXBUF = value;			// send value
	sleep(2);						// wait for transmit to finish
	UCB0CTL1 |= UCTXSTP;		// generate stop condition
}


/*****************************************************************************
 * I2C0_Read(unsigned char byteCount, unsigned short *storageArray)
 * Return: -
 * Param:  	(unsigned char)byteCount	reads the given amount of bytes (max 32)
 * 			(unsigned char*)storageArray	the read function will store the received
 * 										data starting at this pointer location
 *
 * Only in master mode!  Generates a start signal, sends the I2C adress
 * of the desired device as set in I2C0_Init() or I2C0_SetSlaveAdress(), sets
 * the Read/Write bit to read. Reads byteCount bytes from the bus.
 * interrupt based, so global interrupts and UCBI1IE |= UCRCIE need to be set
 * (second is set in I2C0_Init())
 *****************************************************************************/
void I2C0_Read(unsigned char byteCount, unsigned short *storageArray)
{	RXValuesI2C0 = storageArray;// setup pointer for usage in RX interupts
	ByteI2C0 = byteCount;
	UCB0CTL1 &= ~UCTR;		// Set to not Tramsmitter (=receiver mode)
	UCB0CTL1 |= UCTXSTT;	// generate I2C start signal (sends adress automatically)
	// Reading is done in interrupt routine (bottom of file)
}



/*****************************************************************************
 * I2C0_SendAndRead(char command, unsigned char byteCount, unsigned short *storageArray)
 * Return: -
 * Param:  	(char)command				command to send to the I2C bus
 * 			(unsigned char)byteCount	reads the given amount of bytes
 * 			(unsigned char*)storageArray	the read function will store the received
 * 										data starting at this pointer location
 *
 * Only in master mode!  Generates a start signal, sends the I2C adress
 * of the desired device as set in I2C0_Init() or I2C0_SetSlaveAdress(), sets
 * the Read/Write bit to read. Reads byteCount bytes from the bus.
 * interrupt based, so global interrupts and UCBI1IE |= UCRCIE need to be set
 * (second is set in I2C0_Init())
 *****************************************************************************/
void I2C0_SendAndRead(char command, unsigned char byteCount, unsigned short *storageArray)
{	ByteI2C0 = byteCount;			// Set the number of bytes that are to be read
	RXValuesI2C0 = storageArray;	// setup pointer for usage in RX interupts
	UCB0CTL1 |= UCTXSTT | UCTR;				// generate I2C start signal (sends adress automtically)
	sleep(2);								// wait for transmit to finish
	UCB0TXBUF = command;					// send command
	sleep(2);								// wait for transmit to finish

	// Switch to read-mode
	UCB0CTL1 &= ~UCTR;		// Set mode to Not-Transmitter (Receiver mode)
	UCB0CTL1 |= UCTXSTT;	// generate I2C start signal (sends adress automtically)
	// Reading is done in interrupt routine (bottom of file)
}

/******************************************************************
 ******************************************************************/
// Interrupt für den Empfang I2C1

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(UCB0IV,12))
	{
		case 0: break;  // Vector  0: No interrupts
		case 2: break;	// Vector  2: ALIFG
		case 4: 		// Vector  4: NACKIFG
			P1OUT = BIT0;
			UCB0CTL1 |= UCTXSTP;	// Generate stop condition on receiving a NACK condition
			break;
		case 6: break;	// Vector  6: STTIFG
		case 8: break;	// Vector  8: STPIFG
		case 10:		// Vector 10: RXIFG
		__no_operation();// Für Debug
		if (ByteI2C0) {
			*RXValuesI2C0++ = UCB0RXBUF; 	// RX daten auf Pointerposition (nächster Platz im Array) schreiben
			ByteI2C0--;
			if (ByteI2C0 == 1) {
				UCB0CTL1 |= UCTXSTP;      	// Stop generieren
			}
		}
		else {
			*RXValuesI2C0 = UCB0RXBUF; 		// RX daten auf erhöhte Pointerposition (nächster Platz im Array) schreiben
			__no_operation();
		}
		break;
		case 12: break;                     // Vector 12: TXIFG
		default: break;
	}
}

