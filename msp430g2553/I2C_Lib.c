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
unsigned short *recData, *sendData;
unsigned short *recDataStart, *sendDataStart;
unsigned int recMaxCount, sendMaxCount;
int byteCounter;


/*****************************************************************************
 * I2C_init()
 * Return: -
 * Param:  -
 *****************************************************************************/
void I2C_init(int isMaster, unsigned short ownAdress) {
	P1SEL |= BIT6 + BIT7;                     	// Assign I2C pins to USCI_B0
	P1SEL2|= BIT6 + BIT7;                     	// Assign I2C pins to USCI_B0
	UCB0CTL1 |= UCSWRST;                      	// Enable SW reset
	UCB0CTL0 = UCMODE_3 + UCSYNC;   			// I2C, synchronous mode
	if (isMaster){ UCB0CTL0 |= UCMST; }			// set master mode
	UCB0I2COA = ownAdress;              		// Own Address
	UCB0CTL1 &= ~UCSWRST;                 		// Clear SW reset, resume operation
	UCB0I2CIE = UCSTPIE + UCSTTIE;      		// Enable STT and STP interrupt
//	IE2 = UCB0TXIE;                         	// Enable TX interrupt - will be activated when needed
}

/*****************************************************************************
 * I2C0_EnablePullups()
 * Return: -
 * Param:  -
 * enables internal Pullup Resistors
 *****************************************************************************/
void I2C0_EnablePullups(void){
	P1REN |= (BIT6 + BIT7);
}

/*****************************************************************************
 * I2C0_DisablePullups()
 * Return: -
 * Param:  -
 * disables internal Pullup Resistors
 *****************************************************************************/
void I2C0_DisablePullups(void){
	P1REN &= ~(BIT6 + BIT7);
}

/*****************************************************************************
 * I2C_setAdressSlave()
 * Return: -
 * Param:  (unsigned short) adress
 * sets the I2C slave adress. Will automatically be sent with I2C start signals
 *****************************************************************************/
void I2C_setAdressSlave(unsigned short adress){
	UCB0I2CSA = adress;
}

/*****************************************************************************
 * I2C_setAdressOwn()
 * Return: -
 * Param:  (unsigned short) adress
 * sets the MCUs I2C adress for slave mode. MCU will react when a start condition
 * followed by this adress is received from the bus.
 *****************************************************************************/
void I2C_setAdressOwn(unsigned short adress){
	UCB0I2COA = adress;
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
		byteCounter = _count;			// store byte count to send
		sendData = _sendData;		// store pointer to first element to send
		IE2 = UCB0TXIE;				// enable TX interrupts
		UCB0I2CIE = UCNACKIE; 		// Enable NACK interrupt

		UCB0CTL1 |= UCTXSTT + UCTR;	// generate I2C start (auto TX's I2C slave adress),
									// .. and set transmit mode7
	}
}

/*****************************************************************************
 * I2C0_()
 * Return: -
 * Param:  -
 *****************************************************************************/
void I2C_sendAndReceiveData(unsigned short *_sendDataStartAdress) {
	sendDataStart = _sendDataStartAdress;
}

/*****************************************************************************
 * I2C0_()
 * Return: -
 * Param:  -
 *****************************************************************************/
void I2C_recData() {

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
{	// ~ ~ ~ ~ M A S T E R ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
	if (UCB0CTL0 && UCMST){
		// T X . . . . . . . . . . . . . . . .
		if (UCB0STAT && UCTR)
		{
			if (byteCounter){
				UCB0TXBUF = *sendData++;	// send next one
				if (byteCounter == 1){		// After next one, generate stop signal
					UCB0CTL1 |= UCTXSTP;
				}
			} else {
				IE2 &= ~UCB0TXIE;
			}
		} else
		// R X . . . . . . . . . . . . . . . .
		{

		}
	} else if

else { 				// RX mode - - - - - - - - -
		if (byteCounter < sendMaxCount){// check if more data is allowed to be sent
			*recData++ = UCB0RXBUF;		// ? send data
		} else {
			UCB0RXBUF = 0;				// : send zero
		}
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
{	// to identify if the device was adressed (only used in slave mode)
	if	((UCB0STAT & UCSTTIFG) && (UCB0STAT & UCTR)) { // Device was adressed, master wants to write
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
