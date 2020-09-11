 /*****************************************************************************
* Created on 29.01.2015 	 					Lars 'DemoniacMilk' Heinrichs
 *
 * Library for using the MSP430G2553 I2C hardware interface.
 *
 * REMOVE JUMPER for LED 1.6 -> Pin is used for I2C
 * Call initialize function I2C_init() first.
 * In master mode, set the slave adress with I2C_setSlaveAdress(adress)
 * In slave  mode, set the own   adress with I2C_setOwnAdress(adress)
 * To set TX data in slave mode, use I2C_setTXData(*sendData, count)
 * For reading use I2C_recData(adress, count, *storage)
 * For writing use I2C_sendData(adress, count, *sendData)
 * For writing a single byte (like a command or register) and then ..
 * .. automatically read, use I2C_sendAndRead(command/register, count, *storage)
 *
 * code example and more documentation can be found on
 *		 			http://www.pointer-horror.de
***************************************************************************** */
#ifndef I2C_G2553_H_
#define I2C_G2553_H_

void I2C_init(int isMaster, unsigned short ownAdress);
void I2C_enablePullups(void);
void I2C_disablePullups(void);
void I2C_setSlaveAdress(unsigned short adress);
void I2C_setOwnAdress(unsigned short adress);
void I2C_sendCommand(short command);
void I2C_writeRegister(short reg, short value);
void I2C_sendByte(unsigned short *_sendData, int _count);
void I2C_sendAndRead(unsigned short _sendByte, int _byteCount, unsigned short *_recData);
void I2C_recData(unsigned short *storageLocation, int _byteCount);
void I2C_setTXData(unsigned short *sendDataStartAdress, unsigned int maxCount);
void I2C_setRXData(unsigned short *recDataStartAdress, unsigned int maxCount);

#endif /* I2C_LIB_H_ */
