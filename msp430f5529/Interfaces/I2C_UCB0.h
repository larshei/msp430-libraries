/*
 * I2C_library.h
 *
 *  Created on: 06.12.2014
 *      Author: DemoniacMilk
 */

#ifndef I2C_LIBRARY_H_
#define I2C_LIBRARY_H_

void I2C0_Init(int IsMaster, int adress);
void I2C0_EnablePullups(void);
void I2C0_SetSlaveAdress(short adress);
void I2C0_SendCommand(short command);
void I2C0_Read(unsigned char byteCount, unsigned short *storageArray);
void I2C0_SendAndRead(char command, unsigned char byteCount, unsigned short *storageArray);
void I2C0_WriteRegister(short reg, short value);


#endif /* I2C_LIBRARY_H_ */
