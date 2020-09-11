/*
 * EEPROM_24xx_G2553.c
 *
 *  Created on: 28.02.2015
 *      Author: DemoniacMilk
 */

#include <I2C_G2553.h>
#include <msp430.h>

/*****************************************************************************
 * EEPROM_writeData(unsigned short *sendData, int count, unsigned short adress, unsigned int memAdress)
 * Return: -
 * Param:  *sendData	pointer to first data byte to send
 * 			count		number of data bytes to send
 * 			adress		selected adress bits of the EEPROM device (0..7) or complete adress (0b1010nnn)
 * 			memAdress	desired memory adress
 *
 * 	writes data to the eeprom
 *****************************************************************************/
void EEPROM_writeData(unsigned short *sendData, int count, unsigned short adress, unsigned int memAdress) {
	int i;
	I2C_setSlaveAdress(0x50+(adress & 0x0F));		// send adress data

	I2C_start();
	I2C_sendByte((memAdress>>8) & 0xFF);
	while (!(IFG2 & UCB0TXIFG));
	I2C_sendByte(memAdress & 0xFF);
	while (!(IFG2 & UCB0TXIFG));

	for (i = 0; i<count ; i++){
		I2C_sendByte(*sendData++);
		while (!(IFG2 & UCB0TXIFG));
	}
	I2C_stop();
	//I2C_sendData(sendData, count);					// send data (interrupts, might need to add a wait time after function!)
}

/*****************************************************************************
 * EEPROM_writeData(unsigned short *sendData, int count, unsigned short adress, unsigned int memAdress)
 * Return: -
 * Param:  *saveTo		pointer to desired storage memory location
 * 			count		number of data bytes to receive
 * 			adress		selected adress bits of the EEPROM device (0..7) or complete adress (0b1010nnn)
 * 			memAdress	desired memory adress
 *
 * 	reads data from the eeprom
 *****************************************************************************/
void EEPROM_readData(unsigned short *saveTo, int count, unsigned short adress, unsigned int memAdress) {
	I2C_setSlaveAdressNC(0x50 + (adress & 0x0F));		// send adress data
//	I2C_writeRegister((memAdress>>8) & 0xFF, memAdress & 0xFF);			// sets the destination adress

	I2C_start();
	I2C_sendByte((memAdress>>8) & 0xFF);
	while (!(IFG2 & UCB0TXIFG));
	I2C_sendByte(memAdress & 0xFF);
	while (!(IFG2 & UCB0TXIFG));

	I2C_recData(saveTo, count);						// receive data (interrupts, might need to add a wait time after function!)
	while (UCB0STAT & UCBBUSY);
}


