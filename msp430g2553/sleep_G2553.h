/*
 * sleep.h
 *
 *  Created on: 29.01.2015
 *      Author: DemoniacMilk
 */

#ifndef SLEEP_G2553_H_
#define SLEEP_G2553_H_

#define 	TACLK	1
#define 	ACLK	2
#define 	SMCLK	3
#define 	INCLK	4
#define		AUTO	0
#define 	TENTHMILLISECONDS	0
#define		SECONDS				1

void sleep_setClock(unsigned int clockSource, unsigned long clockFrequencyHZ);
void sleep(unsigned int time, int secOrTenthMillisec);

#endif /* SLEEP_H_ */
