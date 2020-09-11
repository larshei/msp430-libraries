/*
 * EEPROM_24xx_G2553.h
 *
 *  Created on: 28.02.2015
 *      Author: DemoniacMilk
 */

#ifndef EEPROM_24XX_G2553_H_
#define EEPROM_24XX_G2553_H_

#define ASCII_PAGE_STT		0		// start of ASCII page position
#define ASCII_PAGE_CNT		20		// count of ascii pages
#define PRESS_PAGE_STT		21		// pressure values new start page
#define PRESS_BYTE_CNT		3		// number of bytes for each value
#define TEMP_PAGE_START		33
#define TEMP_BYTE_CNT		2


void EEPROM_writeData(unsigned short *sendData, int count, unsigned short adress, unsigned int memAdress);
void EEPROM_readData (unsigned short *saveTo,   int count, unsigned short adress, unsigned int memAdress);

#endif /* EEPROM_24XX_G2553_H_ */
