/*
 * bq32002.h
 *
 *  Created on: 30.03.2015
 *      Author: DemoniacMilk
 */

#ifndef BQ32002_H_
#define BQ32002_H_

#define BQ_I2C_ADDRESS	0x68

#define BQ_SECONDS	0x00
#define BQ_MINUTES	0x01
#define BQ_CENT_HR	0x02	// century and hour + CENT_EN bit
#define BQ_DAY		0x03	// clock day (1 sunday - 7 saturday)
#define BQ_DATE		0x04
#define BQ_MONTH	0x05
#define BQ_YEARS	0x06
#define BQ_CAL_CFG	0x07
#define BQ_CFG2		0x08

#define BQ_SF_KEY1	0x20	// special function key 1
#define BQ_SF_KEY2	0x21	// special function key 2
#define BQ_SFR		0x23	// special functions


#define BQ_1SEC_M	0x0F
#define BQ_10SEC_M	0x70	// >> 4 to get 10 seconds

unsigned short BQ_readSeconds();
unsigned short BQ_readMinutes();
unsigned short BQ_readHours();
unsigned short BQ_readDays();
unsigned short BQ_readMonths();
unsigned short BQ_readYears();

static char dayName[7][3] = { "sun", "mon", "tue", "wed", "thu", "fri", "sat"};

#endif /* BQ32002_H_ */
