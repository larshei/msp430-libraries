/*
 * UARTusb.h
 *
 *  Created on: 03.12.2014
 *      Author: DemoniacMilk
 */

#ifndef UARTUSB_H_
#define UARTUSB_H_


void UART_Setup_PC(int mode);
//void UART_Send(char sendMe[], int length, int varCount, ...);
void UART_Send(char sendMe[], int length);
void UART_SendDyn(char sendMe[], int varCount, ...);
int UART_Send_Prepare(char sendMe[], int count);


#endif /* UARTUSB_H_ */
