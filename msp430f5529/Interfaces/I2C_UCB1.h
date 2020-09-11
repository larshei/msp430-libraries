/*
 * I2C_library.h
 *
 *  Created on: 06.12.2014
 *      Author: DemoniacMilk
 */

#ifndef I2C_LIBRARY_H_
#define I2C_LIBRARY_H_

void I2C1_Init(int IsMaster, int adress);
void I2C1_SetSlaveAdress(short adress);
void I2C1_SendCommand(short command);
void I2C1_Read(unsigned char byteCount, unsigned short *storageArray);
void I2C1_SendAndRead(char command, unsigned char byteCount, unsigned short *storageArray);
void I2C0_WriteRegister(short command, short reg);


#endif /* I2C_LIBRARY_H_ */
