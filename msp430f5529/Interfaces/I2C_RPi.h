/* =========================================================
 *                       I 2 C _ R P i . h
 *
 *      Author: Lars Heinrichs           Date: Oct 2014
 *
 * contains routines for reading and writing bits and bytes
 * over OneWire by using a gpio pin.
 *
 * =========================================================
 */

#ifndef I2C_RPI_
#define I2C_RPI_

#define RPI_DATA_REQUEST2	0xD5
#define RPI_DATA_REQUEST	0xC5
#define RPI_MEAS_REQUEST	0xB5
#define RPI_CTL				0xA8
#define RPI_END				0xBB

void I2C_Setup_RPi(short IsMaster, short adress);
void RPi_SetSendData(unsigned short *sendMeFull, unsigned short *sendMeReduced);

#endif /* I2C_RPI_ */
