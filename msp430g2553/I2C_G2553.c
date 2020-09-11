/*****************************************************************************
 * Created on 29.01.2015 	 					Lars 'DemoniacMilk' Heinrichs
 *
 * Library for using the MSP430G2553 I2C hardware interface.
 *
 * REMOVE JUMPER for LED 1.6 -> Pin is used for I2C
 * Call initialize function first.
 * In master mode, set the slave adress with I2C_setAdressSlave(adress)
 * In slave  mode, set the own   adress with I2C_setAdressOwn(adress)
 * To set TX data in slave mode, use I2C_setTXData(*sendData, count)
 * For reading use I2C_recData(adress, count, *storage)
 * For writing use I2C_sendData(adress, count, *sendData)
 * For writing a single byte (like a command or register) and then ..
 * .. automatically read, use I2C_sendAndRecData(adress, command, count, *storage)
 *
 *****************************************************************************/
#include <MSP430.h>
#include <sleep_G2553.h>
unsigned short *recData, *sendData;
unsigned short *recDataStart, *sendDataStart;
unsigned int recMaxCount, sendMaxCount;
int byteCounter;


/*****************************************************************************
 * I2C_init()
 * Return: -
 * Param:  -
 *****************************************************************************/
void I2C_init(int isMaster, unsigned short adress) {
	short SMCLK_divider, ClockSpeed;
	P1SEL |= BIT6 + BIT7;                     	// Assign I2C pins to USCI_B0
	P1SEL2|= BIT6 + BIT7;                     	// Assign I2C pins to USCI_B0
	UCB0CTL1 |= UCSWRST;                      	// Enable SW reset
	UCB0CTL0 = UCMODE_3 + UCSYNC;				// I2C, synchronous mode
	if (isMaster){ 								// is master mode
		UCB0CTL0 |= UCMST; 						// ? set master bit
		UCB0I2CSA = adress;              		// ? slave Address
		UCB0CTL1 = UCSSEL_2 + UCSWRST;			// ? select SMCLK clock source, keep in Software Reset
		// determine clock prescaler
		SMCLK_divider = (BCSCTL2 & 0x06 )>>1;
		if 		(BCSCTL1 ==  CALBC1_1MHZ) { ClockSpeed =  1; }
		else if (BCSCTL1 ==  CALBC1_8MHZ) { ClockSpeed =  8; }
		else if (BCSCTL1 == CALBC1_12MHZ) { ClockSpeed = 12; }
		else if (BCSCTL1 == CALBC1_16MHZ) { ClockSpeed = 16; }
		// Target: 100 kHz
		// prescaler =
		UCB0BR0 = ClockSpeed * 5 / SMCLK_divider + 1;	// about 100kHz or a bit below
		UCB0BR1 = 0;
	} else {
		UCB0I2COA = adress;              		// : Own Address
	}
	UCB0CTL1 &= ~UCSWRST;                 		// Clear SW reset, resume operation
	UCB0I2CIE = UCSTPIE + UCSTTIE;      		// Enable STT and STP interrupt
//	IE2 = UCB0TXIE + UCB0RXIE;                  // Enable TX interrupt - will be activated when needed
}

/*****************************************************************************
 * I2C_enablePullups()
 * Return: -
 * Param:  -
 * enables internal Pullup Resistors
 *****************************************************************************/
void I2C_enablePullups(void){
	P1REN |= (BIT6 + BIT7);
}

/*****************************************************************************
 * I2C_disablePullups()
 * Return: -
 * Param:  -
 * disables internal Pullup Resistors
 *****************************************************************************/
void I2C_disablePullups(void){
	P1REN &= ~(BIT6 + BIT7);
}

/*****************************************************************************
 * I2C_setAdressSlave()
 * Return: -
 * Param:  (unsigned short) adress
 * sets the I2C slave adress. Will automatically be sent with I2C start signals
 *****************************************************************************/
void I2C_setSlaveAdress(unsigned short adress){
	UCB0I2CSA = adress;
}

/*****************************************************************************
 * I2C_setAdressOwn()
 * Return: -
 * Param:  (unsigned short) adress
 * sets the MCUs I2C adress for slave mode. MCU will react when a start condition
 * followed by this adress is received from the bus.
 *****************************************************************************/
void I2C_setOwnAdress(unsigned short adress){
	UCB0I2COA = adress;
}

/*****************************************************************************
 * I2C_sendCommand(short command)
 * Return: -
 * Param:  	(short)command	command to send to the I2C bus
 *
 * Only in master mode! Generates a start signal, sends the I2C adress of the
 * desired device as set in I2C0_Init() or I2C0_SetSlaveAdress(), sets  the
 * Read/Write bit to write. Writes a single byte and generates a stop condition.
 *****************************************************************************/
void I2C_sendCommand(short command)
{	// Sends a single command over the I2C bus
	UCB0CTL1 |= UCTXSTT | UCTR;		// generate I2C start signal (sends adress automtically)
	UCB0TXBUF = command;			// send command
	sleep(10, TENTHMILLISECONDS);	// wait for transmit to finish
	UCB0CTL1 |= UCTXSTP;			// generate stop condition
}

/*****************************************************************************
 * I2C_writeRegister(short reg, short value)
 * Return: -
 * Param:  	(short)reg		first TX byte, on devices setting a memory adress
 * 			(short)value	second TX byte, value to store in reg
 *
 * Only in master mode! Generates a start signal, sends the I2C adress of the
 * desired device as set in I2C0_Init() or I2C0_SetSlaveAdress(), sets  the
 * Read/Write bit to write. Writes two bytes, then generates a stop condition.
 *****************************************************************************/
void I2C_writeRegister(short reg, short value)
{	// Sends a register adress and then a single value over the I2C bus
	UCB0CTL1 |= UCTXSTT | UCTR;		// generate I2C start signal (sends adress automtically)
	UCB0TXBUF = reg;				// send reg
	__delay_cycles(500);
	UCB0TXBUF = value;				// send value
	__delay_cycles(500);
	UCB0CTL1 |= UCTXSTP;			// generate stop condition
}

/*****************************************************************************
 * I2C_sendData()
 * Return: -
 * Param:  (unsigned short) *sendData
 * 		   (unsignd int)	 count
 * Creates a start signal incuding the currently set slave adress (use
 * I2C_SetAdressSlave() ), sends data starting at *sendData. Sends 'count'
 * bytes of data, then generates a stop signal.
 *****************************************************************************/
void I2C_sendData(unsigned short *_sendData, int _count) {
	if (_count > 0){
		byteCounter = _count;		// store byte count to send
		sendData = _sendData;		// store pointer to first element to send
		IE2 = UCB0TXIE;				// enable TX interrupts
		UCB0I2CIE = UCNACKIE; 		// Enable NACK interrupt

		UCB0CTL1 |= UCTXSTT + UCTR;	// generate I2C start (auto TX's I2C slave adress),
									// .. and set transmit mode
	}
}

/*****************************************************************************
 * I2C0_sendAndRead(unsigned short _sendByte, int _byteCount, unsigned short *_recData)
 * Return: -
 * Param:  -
 *****************************************************************************/
void I2C_sendAndRead(unsigned short _sendByte, int _byteCount, unsigned short *_recData) {
	if (_byteCount > 0){
		IE2 = 0;
		recData = _recData;				// store pointer to starting adress
		byteCounter = _byteCount;		// store byte count to receive
		UCB0CTL1 |= UCTXSTT + UCTR;		// generate I2C start (auto TX's I2C slave adress),
		__delay_cycles(500);
		UCB0TXBUF = _sendByte;
		__delay_cycles(500);

		// Switch to read-mode
		UCB0CTL1 &= ~UCTR;		// Set mode to Not-Transmitter (Receiver mode)
		UCB0CTL1 |= UCTXSTT;	// generate I2C start signal (sends adress automtically)
		IFG2 = 0;
		IE2 = UCB0RXIE;			// enable RX interrupts
	}
}

/*****************************************************************************
 * I2C_recData(unsigned short *storageLocation, int _byteCount)
 * Return: -
 * Param:  -
 * Stores _byteCount number of bytes, storing them at adresses starting at
 * *storageLocation
 *****************************************************************************/
void I2C_recData(unsigned short *storageLocation, int _byteCount) {
	recData = storageLocation;
	byteCounter = _byteCount;
	UCB0CTL1 &= ~UCTR;		// Set to not Tramsmitter (=receiver mode)
	UCB0CTL1 |= UCTXSTT;	// generate I2C start signal (sends adress automatically)
	IE2 = UCB0RXIE;
	// Reading is done in interrupt routine (bottom of file)
}

/*****************************************************************************
 * I2C0_()
 * Return: -
 * Param:  -
 *****************************************************************************/
void I2C_setTXData(unsigned short *sendDataStartAdress, unsigned int maxCount) {
	sendDataStart = sendDataStartAdress;
	sendMaxCount = maxCount;
}

/*****************************************************************************
 * I2C0_()
 * Return: -
 * Param:  -
 *****************************************************************************/
void I2C_setRXData(unsigned short *recDataStartAdress, unsigned int maxCount) {
	recDataStart = recDataStartAdress;
	recMaxCount = maxCount;
}

/*****************************************************************************
 * USCIAB0TX_ISR()
 * Return: -
 * Param:  -
 * Interrupt service routine for I2C TX and RX
 *****************************************************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
	if (IFG2 & UCB0TXIFG) {				// ~ ~ TX ~ ~
		if (byteCounter){				// check if bytes are left
			UCB0TXBUF = *sendData++;	// ? send next byte
			byteCounter--;				// ? reduce count of bytes left
			if (byteCounter==1){UCB0CTL1 |= UCTXSTP;} // ? ? NACK after next recieve to end com
		} else {
			IE2 &= ~UCB0TXIE;			// : disable TXBUF interrupts
		} // end master tx
	} else {							// ~ ~ RX ~ ~
		if (byteCounter){				// check if bytes are left
			byteCounter--;				// ? reduce count of bytes left
			if (byteCounter == 1){UCB0CTL1 |= UCTXSTP;}
			*recData++ = UCB0RXBUF;		// ? send next byte
		} else {

			IE2 &= ~UCB0RXIE;			// : disable TXBUF interrupts
		} // end master Rx
	}
}

/*****************************************************************************
 * USCIAB0RX_ISR()
 * Return: -
 * Param:  -
 * Interrupt service routine for I2C status registers
 *****************************************************************************/
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR (void)
#else
#error Compiler not supported!
#endif
// TODO add slave emode
{	// to identify if the device was adressed (only used in slave mode)
if (UCB0CTL0 & UCMST)
	if	((UCB0STAT & UCSTTIFG) && (UCB0CTL1 & UCTR)) { // Device was adressed, master wants to write
		recData = recDataStart;			// reset start
		byteCounter = sendMaxCount;
	} else if (UCB0STAT & UCSTTIFG){
		sendData = sendDataStart;
		byteCounter = recMaxCount;
	}
	else if (UCB0STAT & UCSTPIFG) {

	}
	UCB0STAT &= ~(UCSTPIFG + UCSTTIFG);
}

//UCB0TXBUF = *sendData++;
