/*
 * G2553_Flash.c
 *
 *  Created on: 17.01.2016
 *      Author: Lars 'DemoniacMilk' Heinrichs
 */
#include <msp430.h>
#include <G2553_Flash.h>

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * Sets clock source and divider for Flash Module
 * = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
void Flash_setup(void){
	// sets the flash clock divider
	int divider;
	if (BCSCTL1 == CALBC1_8MHZ)  { divider = 20; }		// 8 MHz / 20 = 400 kHz
	else if (BCSCTL1 == CALBC1_12MHZ) { divider = 30; } //12 MHz / 30 = 400 kHz
	else if (BCSCTL1 == CALBC1_16MHZ) { divider = 40; } //16 MHz / 40 = 400 kHz
	else {divider  =  3;} // not set or set to 1 MHz ; 1MHz/3 = 333 kHz

	FCTL2 = FWKEY + FSSEL0 + divider; // MCLK/divider for Flash Timing Generator
}

/*  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * Reads bytes from flash
 * *char address: 	address of first value you want to read
 * *storeTo: 		address to store the first value in
 * count (short):	number of bytes to be read
 *
 * return: 	OK: the read operation has been performed
 * 			OUTOFBOUNDS: *adress or *(address+count) not within flash boundaries
 *  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
unsigned short Flash_read(int address, char *storeTo, int count){
	// allowed to write here?
	if (((address < FL_INFO_A) || (address > FL_INFO_D + 255) || (address + count > FL_INFO_D + 255))){
		return OUTOFBOUNDS;
	}

	int* pointer = (int *)address;
	while(count--){
		*storeTo++ = *pointer++;
	}
	return OK;
}

/*  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * Flash bits may only be changed from 1 to 0. If a change from 0 to 1 is required, a block erase must be performed first!
 * this function checks, if the changes may be applied without erase.
 * *char address: address you want to write to
 * newValue: Value you want to store
 *
 * return: 	0: you cannot write without performing an erase
 * 			1: you should be able to write the new value without erasing
 * 			2: the chosen address exceeds flash memory
 *  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
unsigned short Flash_writeable(int address, char newValue){
	// adress in flash memory?
	if (((address < FL_INFO_A) || (address >= FL_INFO_D))){
		return OUTOFBOUNDS;
	} else if (address >= FL_INFO_A) {	// in Info block A? Did we protect A? (may contain calibration data)
		return PROTECTED;
	}

	char *p = (char*)address;

	char currentValue, changing;

	currentValue = *p;					// Current cell value
	changing = currentValue ^ newValue;			// what bits are changing?

	// are all the changing bits currently 1?
	if ((changing & currentValue) == changing) {
		return NOTWRITEABLE;
	}
	return WRITEABLE;
}

/*  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * write bytes to flash
 * *char address: 	flash address you want to start writing at
 * *readFrom: 		address to start reading at
 * count (short):	number of bytes to be written
 *
 * return: 	OK: the read operation has been performed
 * 			OUTOFBOUNDS: *adress or *(address+count) not within flash boundaries
 *  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
unsigned short Flash_write(int address, char *readFrom, short count){
	// are we actually in flash memory?
	if (((address < FL_INFO_D) || (address + count > FL_INFO_A))){
		return OUTOFBOUNDS;
	}

	FCTL3 = FWKEY;			// unlock Flash (by not setting LOCK)
	FCTL1 = FWKEY + WRT; 	// set to write mode

	char* p = (char*)address;
	// Perform write operation
	while(count--){
		*p++ = *readFrom++;	// store value
	}

	FCTL1 = FWKEY; 			// exit write mode
	FCTL3 = FWKEY + LOCK;	// lock Flash
	return OK;
}

unsigned short Flash_eraseBlock(int addressWithinBlock){
	if (((addressWithinBlock >= FL_INFO_A) || (addressWithinBlock < FL_INFO_D))){
		return OUTOFBOUNDS;
	}
	int* pointer = (int *)addressWithinBlock;

	FCTL3 = FWKEY;			// unlock Flash (by not setting LOCK)
	FCTL1 = FWKEY + ERASE;		// set to erase mode
	*pointer = 0;	// a dummy write to initialize block erase
	FCTL3 = FWKEY + LOCK;		// lock Flash
	return OK;
}

/*  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * Modifies Values in RAM.
 * (unsigned short) index: 	index of value to modify (0 .. 63)
 * (char *) readFrom:		pointer to starting adress of new values
 * (short) count:			number of bytes to be written (0 .. 64-index)
 *
 * return: 	OK: the read operation has been performed
 * 			OUTOFBOUNDS: *adress or *(address+count) not within flash boundaries
 *
 * 			Recommended usage:
 * - Flash_backup(Block): store block to RAM
 * - Flash_ramWrite() - (this function): alter the values you would like to change
 * - Flash_eraseBlock(Block): erase backed up block (necessary before performing write operation)
 * - flash_ramToBlock(Block, *ramSectionAddress, 64): Write modified data from RAM to Flash block
 *  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
unsigned short Flash_copyToRam(int addressInBlock){
	unsigned short i;
	// determine the block we are in. This ...
	addressInBlock = 0x1000 + ((addressInBlock - 0x1000) / 64) * 64;
	// .. needs to be one of the block starting addresses.
	if  (!( (addressInBlock == FL_INFO_A) || (addressInBlock == FL_INFO_B) || (addressInBlock == FL_INFO_C) || (addressInBlock == FL_INFO_D) )){
		return OUTOFBOUNDS;
	} //If block to be written to is blockA then make sure its not protected.
	if ((addressInBlock == FL_INFO_A) && !INFO_A_PROT){
		return PROTECTED;
	}

	char *ram = (char*)RAM_BCKUP;	// 360h is starting area of custom defined, unused RAM
	char *flash = (char*)addressInBlock;

	for(i=0;i<64;i++){
		*ram++ = *flash++;
	}

	return OK;
}

/*  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * Modifies Values in RAM.
 * (unsigned short) index: 	index of value to modify (0 .. 63)
 * (char *) readFrom:		pointer to starting adress of new values
 * (short) count:			number of bytes to be written (0 .. 64-index)
 *
 * return: 	OK: the read operation has been performed
 * 			OUTOFBOUNDS: *adress or *(address+count) not within flash boundaries
 *
 * 			Recommended usage:
 * - Flash_backup(Block): store block to RAM
 * - Flash_ramWrite() - (this function): alter the values you would like to change
 * - Flash_eraseBlock(Block): erase backed up block (necessary before performing write operation)
 * - flash_ramToBlock(Block, *ramSectionAddress, 64): Write modified data from RAM to Flash block
 *  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
unsigned short Flash_ramWrite(unsigned short index, char *readFrom, short count){
	// index out of bounds?
	if (index+count >= 64) {
		return OUTOFBOUNDS;
	}

	char *p = (char*)(index + RAM_BCKUP);	// 360h is starting area of custom defined, unused RAM

	while(count--){
		*p++ = *readFrom++;
	}

	return OK;
}



/*  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * This function does multiple things:
 * - copies the flash block you want to alter information to RAM (0x360-0x400, linker file needs to be changed!)
 * - adjusts the values while in ram (you cannot write over block boundaries)
 * - erases and rewrites the altered values to Flash
 * (int) block:				What block do you want to overwrite? (FL_INFO_A .. FL_INFO_D)
 * (unsigned short) index: 	index of first value to overwrite (0 .. 63)
 * (char *) readFrom:		pointer to starting adress of new values
 * (short) count:			number of bytes to be written (0 .. 64-index)
 *
 * return: 	OK: the read operation has been performed
 * 			OUTOFBOUNDS: *adress or *(address+count) not within flash boundaries
 *  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
unsigned short Flash_ramToBlock(int blockAddress){
	// determine the block we are in. This ...
	blockAddress = 0x1000 + ((blockAddress - 0x1000) / 64) * 64;
	// .. needs to be one of the block starting addresses.
	if  (!( (blockAddress == FL_INFO_A) || (blockAddress == FL_INFO_B) || (blockAddress == FL_INFO_C) || (blockAddress == FL_INFO_D) )){
		return OUTOFBOUNDS;
	} //If block to be written to is blockA then make sure its not protected.
	if ((blockAddress == FL_INFO_A) && !INFO_A_PROT){
		return PROTECTED;
	}

	unsigned int i;
	char* ram = (char*) RAM_BCKUP;
	char* flash = (char*)blockAddress;

	FCTL3 = FWKEY;			// unlock Flash
	FCTL1 = FWKEY + WRT;		// set to block write mode

	for (i=0;i<64;i++) {
		*flash++ = *ram++;
		while(!(FCTL3 & WAIT));
	}
	FCTL1 = FWKEY;
	while((FCTL3 & BUSY));
	FCTL3 = FWKEY + LOCK;
	return OK;
}

/*  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
 * This function does multiple things:
 * - copies the flash block you want to alter information to RAM (0x360-0x400, linker file needs to be changed!)
 * - adjusts the values while in ram (you cannot write over block boundaries)
 * - erases and rewrites the altered values to Flash
 * (int) block:				What block do you want to overwrite? (FL_INFO_A .. FL_INFO_D)
 * (unsigned short) index: 	index of first value to overwrite (0 .. 63)
 * (char *) readFrom:		pointer to starting adress of new values
 * (short) count:			number of bytes to be written (0 .. 64-index)
 *
 * return: 	OK: the read operation has been performed
 * 			OUTOFBOUNDS: *adress or *(address+count) not within flash boundaries
 *  = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
unsigned short Flash_alterValue(int block, int index, char *readFrom, short count){
	// determine what block we are in
	if ((block < FL_INFO_D) || (block + count > FL_INFO_A + 64)){
		return OUTOFBOUNDS;
	}

	//If block to be written to is blockA then make sure its not protected.
	if ((block > FL_INFO_A) && !INFO_A_PROT){
		return PROTECTED;
	}

	// determine the block we are in
	block = 0x1000 + ((block - 0x1000) / 64) * 64;

	Flash_copyToRam(block);
	Flash_ramWrite(index, readFrom, count);
	Flash_eraseBlock(block);
	Flash_ramToBlock(block);

	__no_operation();
	return OK;
}
