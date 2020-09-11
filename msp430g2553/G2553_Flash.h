/*
 * G2553_Flash.h
 *
 *  Created on: 17.01.2016
 *      Author: DemoniacMilk
 */

#ifndef G2553_FLASH_H_
#define G2553_FLASH_H_

#define FL_INFO_A	0x10C0	// contains configuration data (for clock speeds and ADC)
#define FL_INFO_B	0x1080
#define FL_INFO_C	0x1040
#define FL_INFO_D	0x1000

#define RAM_BCKUP	0x0360	// a reserved area in RAM -> used to save and modify FLASH data (as flash content might need a block erase before being written)
#define INFO_A_PROT	1		// kepp info block A protected? (contains calibration data!)

// same as in sleep_G2553.h !
#define 	TACLK	1
#define 	ACLK	2
#define 	SMCLK	3
#define 	INCLK	4
#define		AUTO	0

// possible return values of Flash_writeable()
#define WRITEABLE		0
#define OK				0
#define NOTWRITEABLE	1
#define OUTOFBOUNDS		2
#define	PROTECTED		3

unsigned short Flash_writeable(int address, char newValue);
void Flash_setup(void);
unsigned short Flash_read(int address, char *storeTo, int count);
unsigned short Flash_writeable(int address, char newValue);
unsigned short Flash_eraseBlock(int addressWithinBlock);
unsigned short Flash_write(int address, char *readFrom, short count);
unsigned short Flash_ramToBlock(int blockStartAddress);
unsigned short Flash_alterValue(int block, int index, char *readFrom, short count);
unsigned short Flash_copyToRam(int addressInBlock);
#endif /* G2553_FLASH_H_ */
