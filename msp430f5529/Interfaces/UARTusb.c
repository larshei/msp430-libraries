
/* =======================================================
 *                    U A R T u s b . c
 *
 *     Author: Lars Heinrichs           Date: Oct 2014
 *
 *  Allows sending data to a pc over USB UART from an
 *  MSP430F5529. Data can be fetched through a virtual com
 *  port. Uses UCA1 module and contains an interrupt for it.
 *
 *  call UART_SendDyn("Test: %0 + %1\0", 2, (long)variable1, (long)variable2);
 * =======================================================
 */

#include <msp430.h>
#include <stdarg.h>
#include <sleep.h>

#define UARTasHex			0		// Wahrheitswert: UART zahlen als HEX oder als DEC ausgeben (in ASCII Zeichen!!)


void UART_Setup_PC(int mode);
//void UART_Send(char sendMe[], int length, int varCount, ...);
void UART_Send(char sendMe[], int length, int a0, int a1);
void UART_SendDyn(char sendMe[], int varCount, ...);
int UART_Send_Prepare(char sendMe[], int count, short dynamnic);

unsigned char *UARTnextChar;
int UARTbyteCount_PC, UARTbyteCount_MSP;
int a,b;
char UARTbusy;
char UartString[32];
char UARTfinalString[64];		// Zur Bereitstellung / wird gesendet
long UARTvariablePointer[8];	// Es können bis zu acht zusätzliche parameter and UART_Send übergeben werden
short lengthDy;


/*****************************************************************
 * UART_Setup_PC(int mode)
 * Return: -
 * Param:  (int) mode: baud rate (2400, 4800, 9600)
 *
 * Sets up the UCA1 module for UART comunication.
 *****************************************************************/
void UART_Setup_PC(int mode) // 2400, 4800, 9600
// 2400, 4800 oder 9600 baud, lsb first, 8 Daten-, 0 Paritybits, kein overflow
{
	UCA1CTL1 |= UCSWRST;				// Reset status
	UCA1CTL1 |= UCSSEL_1 + UCSWRST;		// ACLK (größerer timing Fehler als SCLK, aber SCLK im LPM3 ausgeschaltet, UC7BIT nicht gesetzt -> 8 Bit Modus
	UCA1CTL0 = UCMODE0;					// UART Mode

	if (mode == 2400) {
		UCA1BR0 = 13;					// Handbuch S. 905, für 2400 baud mit minimalem Fehler
		UCA1BR1 = 0;					// BR0 und BR1 = erster modulationslevel
		UCA1MCTL = 6 << 1;				// Zweiter modulationslevel, Rest 0
	} else if (mode == 9600) {
		UCA1BR0 = 3;					// Handbuch S. 905, für 9600 baud mit minimalem Fehler
		UCA1BR1 = 0;					// BR0 und BR1 = erster modulationslevel
		UCA1MCTL = 3 << 1;				// Zweiter modulationslevel, Rest 0
	} else { 							// falls nicht 9600 oder 2400 gewählt immer auf 4800 setzen
		UCA1BR0 = 6;					// Handbuch S. 905, für 4800 baud mit minimalem Fehler
		UCA1BR1 = 0;					// BR0 und BR1 = erster modulationslevel
		UCA1MCTL = 7 << 1;				// Zweiter modulationslevel, Rest 0
	}

	P4SEL |= BIT5 | BIT4;				// Auf High setzen (Peripheriemodus)
	UCA1CTL1 &= ~UCSWRST;				// Reset status aufheben
	UCA1IV = 0;
	UCA1IE = UCTXIE;			// Sendebuffer Interrupt einschalten
}

/*****************************************************************
 * UART_Setup_PC(int mode)
 * Return: -
 * Param:  (int) mode: baud rate (2400, 4800, 9600)
 *
 * Sets up the UCA1 module for UART communication.
 *****************************************************************/
void UART_Send(char sendMe[], int length, int a0, int a1)
{	// ersetzt %a und %b durch a0 und a1

	a = a0;
	b = a1;
	while (UARTbusy) {			// Chekcen, ob UART Sendebereit
		sleep(1);				// 1/4ms warten
	}
	UARTbusy = 1;				// Wieder auf beschäftigt setzen, um den nächsten Sendevorgange ggf zu verzögern
	UARTbyteCount_PC = UART_Send_Prepare(sendMe, length, 0);	// String bearbeiten (ersetze %T mit temp und %P mit Druckmessergebnissen)
	UARTnextChar = (unsigned char*) UARTfinalString;	// Für Interrupt, pointer auf Ausgabestring (gefüllt in UART_Send_Prepare)
	UCA1IE |= UCTXIE;			// Interrupts einschalten
	P4OUT ^= BIT7;				// LED rot togglen für optische Rückmeldung
}

/*****************************************************************
 * UART_Setup_PC(int mode)
 * Return: 	-
 * Param:  	(char) sendMe[]: string to send
 * 			(int)  varCount: number of variables to send
 * 			(long) ...	: list of variables to put in
 *
 * String needs to end with \0 symbol!
 * Sends a string over UART. Before sending, the string is modified
 * and the values of the variables replace the expressions %0..%7.
 * The values are put in as ASCII chars, not as numeric values.
 *****************************************************************/
void UART_SendDyn(char sendMe[], int varCount, ...)
{	// Parameter: String, Stringlänge, Anzahl der nachfolgenden Parameter (0-8), Liste der Parameter (nur int!!)

	// String Länge bestimmen
	char *checkChar = sendMe;
	int varCountTotal = varCount;
	lengthDy = -1;
	while((!(checkChar == "\0")) && (lengthDy<32))
	{	lengthDy++;
		checkChar++;
	}

	va_list zusatzVariablen;
	if (varCount>8)
	{	varCount=8;
	} else if (varCount<0)
	{	varCount=0;
	}

	if (varCount)
	{	va_start(zusatzVariablen, varCount);
		while(varCount--)
		{	UARTvariablePointer[varCountTotal - varCount - 1] = va_arg(zusatzVariablen, long);
		}
		va_end(zusatzVariablen);
	}

	while (UARTbusy) {			// Chekcen, ob UART Sendebereit
		sleep(1);				// 1/4ms warten
	}
	UARTbusy = 1;				// Wieder auf beschäftigt setzen, um den nächsten Sendevorgange ggf zu verzögern
	UARTbyteCount_PC = UART_Send_Prepare(sendMe, lengthDy, 1);	// String bearbeiten (ersetze %T mit temp und %P mit Druckmessergebnissen)
	UARTnextChar = (unsigned char*) UARTfinalString;	// Für Interrupt, pointer auf Ausgabestring (gefüllt in UART_Send_Prepare)
	UCA1IE |= UCTXIE;			// Interrupts einschalten
//	P4OUT ^= BIT7;				// LED rot togglen für optische Rückmeldung
}

/*****************************************************************
 * UART_Send_Prepare(char sendMe[], int count, short dynamic)
 * Return: 	-
 * Param:  	(char) sendMe[]: string to send
 * 			(int)  count: number of variables to send
 * 			(short) dynamic	: used dynamic? (always yes)
 *
 * Modifies a string: replaces %0..%7 by variable
 * valus (as ASCII symbols). Stores the new string in a global
 * array that is then send over uart.
 *****************************************************************/
int UART_Send_Prepare(char sendMe[], int count, short dynamic)
{
	int j, h, intBufCtr;		// Zählvariablen
	int add = 0;				// Offset zur Berücksichtigung veränderter Stringlängen bei Ersetzen
	char nextChar;				// nächstes zu setzendes Zeichen
	char intBuf[8];				// Buffer für int zur späteren Umkehrung, da diese beim zerlegen Rückwärts abgearbeitet werden

	long tempP;					// werden zur Bearbeitung von Druck und Temperaturwerten genutzt

	for (j=63; j>=0; j--)		// Leer machen
	{	UARTfinalString[j] = 0;
	}

	for (j = 0; j < count; j++) //Läuft alle Zeichen des Strings ab
	{	if (((sendMe[j - 1] == '%'))) // P gefunden? war es ein %P?
		{	intBufCtr = 0;		// Ziffernzahl zurücksetzen
			if (!dynamic)
			{	switch (sendMe[j])
				{	case 'a':
						tempP = a;
						break;
					case 'b':
						tempP = b;
						break;
					default:
						tempP = -1;
						break;
				}
			} else // Dynamic
			{	switch (sendMe[j])
				{	case '0':
						tempP = UARTvariablePointer[0];
						break;
					case '1':
						tempP = UARTvariablePointer[1];
						break;
					case '2':
						tempP = UARTvariablePointer[2];
						break;
					case '3':
						tempP = UARTvariablePointer[3];
						break;
					case '4':
						tempP = UARTvariablePointer[4];
						break;
					case '5':
						tempP = UARTvariablePointer[5];
						break;
					case '6':
						tempP = UARTvariablePointer[6];
						break;
					case '7':
						tempP = UARTvariablePointer[7];
						break;
					default:
						tempP = -1;
						break;
				}
			}
			add--;				// Im Ausgabestring eine Stelle zurück (% überschreiben)
			if (tempP < 0) { UARTfinalString[j + add] = '-'; add++; tempP *= -1; }
			if (tempP == 0) { intBuf[intBufCtr++] = 48;; add++;}
			while (tempP) 		// Hier wird der Druckwert in die einzelnen Ziffern gesplittet
			{	intBuf[intBufCtr++] = UARTasHex ? (tempP & 0x0F) + 48 : tempP % 10 + 48;// 48 ist 0 im ASCII code
				intBuf[intBufCtr-1] += (intBuf[intBufCtr-1]>57) ? 7 : 0 ;
				tempP = UARTasHex ? tempP >>= 4 : tempP / 10;
			}

			for (h = 1; h <= intBufCtr; h++) // Ziffernfolge in korrekter Reihenfolge in den String schreiben
			{	nextChar = intBuf[intBufCtr - h];
				UARTfinalString[j + add] = nextChar;
				add++;			// Offset für Ausgabestring einen erhöhen pro gesetztem Zeichen
			}
			add--;				// da am Ende das letzte nextChar nochmal geschrieben wird, dieses Überschreiben (nicht doppelt einbauen)
			__no_operation();
		}
		else									// Ist kein %P oder %T aufgetreteten, Zeichen direkt verwenden
		{	nextChar = sendMe[j];
		}

		UARTfinalString[j + add] = nextChar;	// Zeichen in Ausgabestring schreiben an nächster Stelle
	}
	return j + add;								// Rückgabe der neuen Gesamtlänge, Info braucht der Interrupt
}

// ---------------------------------------------------------------
// ------------------ I N T E R R U P T S ------------------------
// ---------------------------------------------------------------

// Interrupt für Senden und Empfangen über UART an den PC
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(UCA1IV,4))
	{
		case 0:break;                             // Vector 0 - no interrupt
		case 2:										// Vector 2 - RXIFG
			// Echo erhaltene Befehle
	//		UCA1TXBUF = UCA1RXBUF;
	//		UCA1IFG = 0;
			break;
		case 4:									// Vector 4 - TXIFG
		// strings, auf die der Pointer UARTnextChar zeigt, senden
		if(UARTbyteCount_PC) {					// noch Bytes vorhanden?
			UCA1TXBUF = *UARTnextChar++;		// Nächstes byte senden
			UARTbyteCount_PC--;					// Übrige Anzahl verringern
		}
		else {
			UCA1TXBUF = *UARTnextChar;			// Nur noch ein Byte da? Senden
			UCA1IE &= ~UCTXIE;					// Interrupts ausschalten
//			P1OUT ^= BIT0;						// LED togglen (debug)
			UARTbusy = 0;						// UART wieder freigeben (Softwareseitig!)
		}
		break;
		default: break;
	}
}
