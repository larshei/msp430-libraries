/* =======================================================
 *                  N R F 2 4 L 0 1 . c
 *
 *     Author: Lars Heinrichs           Date: Oct 2014
 *
 *      Contains basic routines to use an MSP430F5529
 *            with an NRF24L01 Transceiver Module
 *    	  uses the UCB1 Module for SPI communication
 *
 * =======================================================
 *
 * Pin Usage:
 * 1 	GND		GND		brown
 * 2 	VCC		3,3V	red
 * 3 	CE		P3.5	yellow
 * 4	CSN		P3.6	orange
 * 5	SCK		P4.3	blue
 * 6	MOSI	P4.1	green
 * 7	MISO	P4.2	grey
 * 8	IRQ 	P7.0	purple
 * Auto ACK ausschalten! 	*/

#include <msp430.h>
#include <NRF24L01.h>
#include <sleep.h>
#include <UARTusb.h>

int short NRFbyteCount = 0, MSPtoNRFbusy = 0;		// Anzahler der zu sendenen Bytes / Status der Übertragung zwischen MSP und NRF
unsigned short *NRFvalueSend, *NRFvalueReceive;
short NRFValues[32], recBytesLeft, recBytesReceived;
short temp;

unsigned short receivedData;
//const short AdressA[5] = {0x11, 0x2C, 0x3D, 0x44, 0x5C};	// PX_0 and TX for Reference
const short AdressB[5] = {0xF0, 0xF0, 0xF0, 0xF0, 0xF0};	// PX_0 and TX for Reference Benni
//const short AdressB[5] = {0x6A, 0x75, 0x82, 0x96, 0xA1};	// PX_0 and TX for non-Reference
const short AdressA[5] = {0x73, 0x65, 0x72, 0x76, 0x31};	// PX_0 and TX for non-Reference Benni
const short AdressLastBytes[5] = {0x0A, 0x2A, 0x4A, 0x6A, 0x8A};	// LSBytes Adressen P2..P6

#define payloadLength	8		// 0..32
#define CEl				P3OUT &= ~BIT5;
#define CEh				P3OUT |= BIT5;
#define CSNl			P3OUT &= ~BIT6;
#define CSNh			P3OUT |= BIT6;


// = = = = = = = =   E L E M E N T A R Y    F U N C T I O N S   = = = = = = = = = = =
// These functions dont call any other functions and dont need any variables

void BlinkLED(void){
	P4OUT ^= BIT7;
	P1OUT ^= BIT0;
}

void SPI_Setup(void)
{	// SPI
	P4OUT &= ~(BIT3 | BIT2 | BIT1);				// Auf LOW setzen
	P4SEL |= BIT3 | BIT2 | BIT1;				// SPI Lanes (P4.1-MOSI und P4.2-MISO) (Auf peripheriegerät stellen)
	P4DIR |= BIT1 | BIT3;						// MOSI, CLK -->out
	P4DIR &= ~BIT2;								// MISO <--in
	// SENSOR
	P3DIR |= BIT5 | BIT6;						// Als Ausgangs
	P3OUT |= BIT5 | BIT6;						// CE, CSN Auf High
	UCB1CTL1 |= UCSWRST;						// Reset Modus gegen unvorhersehbares Verhalten
	UCB1CTL0 = UCMSB + UCSYNC + UCMST + UCCKPH + UCMODE0; // MSB First, Synchron, Master, Clock Phasen, 3Pin SPI
	UCB1CTL1 |= UCSSEL_1 + UCSWRST;				// SMCLK auswählen, Reset status behalten
	UCB1BR0 = 4;								// Kein Clock Teiler
	UCB1BR1 = 0;								// Kein Clock Teiler
	UCB1CTL1 &= ~UCSWRST;        				// USCI nicht mehr im Reset Status festhalten
	UCB1IE |= UCTXIE;                  			// Interrupts für Sendebuffer einschalten
	sleep(1);
}

short NRFgetStatus(void)
{	short returnMe;
	while (MSPtoNRFbusy) {sleep(1);}
	P3OUT &= ~BIT6;
	UCB1IE &= ~ UCRXIE;					// Interrupt ausshcalten um direkt hier zu lesen
	UCB1TXBUF = NOP;					// NOP Befehl senden (nichts machen, NRF sendet immer status als Antwort parallel
	sleep(1);
	while(UCB1STAT & UCBUSY);			// Warten
	returnMe = UCB1RXBUF;				// RX Buffer lesen
	P3OUT |= BIT6;						// CSN auf high schalten -> NRF auf Bus abwählen
	return returnMe;					// Status zurückgeben
}



void NRFreadReg(short reg, short length)
{	recBytesLeft = (length>32) ? 32 : length;
	NRFvalueReceive = (unsigned short *)NRFValues;
	while (MSPtoNRFbusy) {sleep(1);}
	MSPtoNRFbusy = 1;
	UCB1IE |= UCRXIE;
	P3OUT &= ~BIT6;						// CSN auf low schalten -> NRF auf Bus wählen
	UCB1TXBUF = reg;					// Befehl und register schicken
	sleep(1);							// Warten
	MSPtoNRFbusy = 0;
}

void NRFreadRegNoInterrupt(short reg, short length)
{	recBytesLeft = (length>32) ? 32 : length;
	NRFvalueReceive = (unsigned short *)NRFValues;
	while (MSPtoNRFbusy) {sleep(1);}
	P3OUT &= ~BIT6;						// CSN auf low schalten -> NRF auf Bus wählen
	UCB1TXBUF = reg;					// Befehl und register schicken
	while(UCB1STAT & UCBUSY){sleep(1);}
	*NRFvalueReceive++ = UCB1RXBUF;
	UCB1TXBUF = NOP;

	while(length)
	{	while(UCB1STAT & UCBUSY);
		*NRFvalueReceive++ = UCB1RXBUF;
		UCB1TXBUF = NOP;
		length--;
	}
	P3OUT |= BIT6;								// Warten
}

short NRFreadSingleReg(short reg)
{	short returnMe;
	while(UCB1STAT & UCBUSY);
	UCB1IE &= ~ UCRXIE;
	P3OUT &= ~BIT6;						// CSN auf low schalten -> NRF auf Bus wählen
	while(UCB1STAT & UCBUSY);
	UCB1TXBUF = reg;					// Befehl und register schicken (Lesen = befehl 0x00)
	while(UCB1STAT & UCBUSY);			// Warten
	returnMe = UCB1RXBUF;				// RX Buffer lesen (Beinhaltet Status)
	UCB1TXBUF = NOP;					// weiter senden
	while(UCB1STAT & UCBUSY);			// Warten
	returnMe = UCB1RXBUF;				// RX Buffer lesen (Beinhaltet Antwort)
	while(UCB1STAT & UCBUSY);							// Warten
	P3OUT |= BIT6;						// CSN auf high schalten -> NRF auf Bus abwählen
	UCB1IE |= UCRXIE;
	return returnMe;					// Status zurückgeben
}

// = = = = = = = =   B A S I C   F U N C T I O N S   = = = = = = = = = = =
// These functions dont call any other functions but need global variables

void NRFwriteCommand(unsigned short command, short reg, unsigned short *value, short byteCount)
{	// For sending multiple Bytes, sends n=byteCount Bytes starting at position *value
	// command+reg create the final instruction for the NRF24L01
	// Requires Interrupt routine for UCB1
	// Requires global variables short NRFbyteCount, short MSPtoNRFbusy, short NRFvalueReceived[], short NRFvalueSend[]
	short combined = command + reg;		//
	NRFbyteCount = byteCount;
	if (command == RD_RX_PLOAD) {
		NRFvalueReceive = (unsigned short *)value;
	} else {
		NRFvalueSend = (unsigned short *)value;
	}

	while (MSPtoNRFbusy) { sleep(1); }
	MSPtoNRFbusy = 1;
	P3OUT &= ~BIT6;						// CSN auf low schalten -> NRF auf Bus wählen
	UCB1TXBUF = combined;
	while(UCB1STAT & UCBUSY);
	UCB1IE |= (command == RD_RX_PLOAD) ? UCRXIE : UCTXIE;
}

void NRFwriteCommandNoInterrupt(unsigned short command, short reg, unsigned short *value, short byteCount)
{	// For sending multiple Bytes, sends n=byteCount Bytes starting at position *value
	// command+reg create the final instruction for the NRF24L01
	// Requires Interrupt routine for UCB1
	// Requires global variables short NRFbyteCount, short MSPtoNRFbusy, short NRFvalueReceived[], short NRFvalueSend[]
	short combined = command + reg;		//
	NRFbyteCount = byteCount;
	if (command == RD_RX_PLOAD) {
		NRFvalueReceive = (unsigned short *)value;
	} else {
		NRFvalueSend = (unsigned short *)value;
	}

	while (MSPtoNRFbusy) { sleep(1); }
	P3OUT &= ~BIT6;						// CSN auf low schalten -> NRF auf Bus wählen
	UCB1TXBUF = combined;
	while(UCB1STAT & UCBUSY);

	while(byteCount)
	{	UCB1TXBUF = *value++;
		while(UCB1STAT & UCBUSY);
		byteCount--;
	}
	P3OUT |= BIT6;						// CSN auf high schalten -> NRF abwählen
}


short NRFwriteSingleCommand(short command, short reg, short value)
{	// For sending the single Byte "value"
	// command+reg create the final instruction for the NRF24L01
	// Requires Interrupt routine for UCB1
	// Requires global variables short NRFbyteCount, short MSPtoNRFbusy, short NRFvalueReceived[], short NRFvalueSend[]	short combined = command + reg;		//
	unsigned short returnMe, status;

	short combined = command + reg;

	while (MSPtoNRFbusy);
	CSNl							// CSN auf low schalten -> NRF auf Bus wählen
	UCB1TXBUF = combined;				// Befehl und register schicken
	while(UCB1STAT & UCBUSY);
	status = (unsigned short)UCB1RXBUF;
	UCB1TXBUF = value;					// Wert schicken
	while(UCB1STAT & UCBUSY);
	returnMe = (unsigned short)UCB1RXBUF;
	while(UCB1STAT & UCBUSY);
	CSNh							// CSN auf high schalten -> NRF auf Bus abwählen
	sleep(3);
	return returnMe+(status<<8);
}

// = = = = = = = =   E R W E I T E R T E   F U N K T I O N E N   = = = = = = = = = = =
int NRFresetMaxRT()
{	return NRFwriteSingleCommand(WRITE_REG, STATUS, 0x70);
}

int NRFresetTXDS()
{	return NRFwriteSingleCommand(WRITE_REG, STATUS, 0x10);
}


short NRFsetup(short IsReceiver)
{	// Uses NRFwriteSingleCommand() and NRFwriteCommand() to set up the NRF via SPI
	short send, i;
	if(IsReceiver){
		P1DIR &= ~BIT6;									// IRQ Pin als Eingang
		P1IE = BIT6;									// Interrupts einschalten
		P1IES = BIT6;									// High to Low für Interrupt
		P1IFG &= ~BIT6;									// Interrupt Flag reset (vorher undefiniert)
	}

//	NRFwriteSingleCommand(WRITE_REG, STATUS, 0x70);		// Reset der Interrupt Pins
//	sleep(4);

	send = EN_CRC + CRCO + PWR_UP;
	NRFwriteSingleCommand(WRITE_REG, CONFIG, send);		// CRC einschalten, keine Interrupt masks, 2 Byte CRC Check
	temp = NRFwriteSingleCommand(READ_REG, CONFIG, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = 0x3F;										// AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
	NRFwriteSingleCommand(WRITE_REG, EN_AA, send);		// AutoAck einschalten
	temp = NRFwriteSingleCommand(READ_REG, EN_AA, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = 0x03;
	NRFwriteSingleCommand(WRITE_REG, EN_RXADDR, send);	// Data Pipes einschalten
	temp = NRFwriteSingleCommand(READ_REG, EN_RXADDR, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = AW_5;
	NRFwriteSingleCommand(WRITE_REG, SETUP_AW, send);		// Adresslänge auf 5 Byte
	temp = NRFwriteSingleCommand(READ_REG, SETUP_AW, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = 0x4C;
	NRFwriteSingleCommand(WRITE_REG, RF_CH, send);			// Kanal 00 auswählen
	temp = NRFwriteSingleCommand(READ_REG, RF_CH, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = RF_PWR_FULL + RF_DR_HIGH + 1;//+ CONT_WAVE;
	NRFwriteSingleCommand(WRITE_REG, RF_SETUP, send);		// Volle Sendeleistung, Mittlere Datenrate (1Mbps)
	temp = NRFwriteSingleCommand(READ_REG, RF_SETUP, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = ARC_8 + ARD_1000;
	NRFwriteSingleCommand(WRITE_REG, SETUP_RETR, send);		// Retransmit einstellen
	temp = NRFwriteSingleCommand(READ_REG, SETUP_RETR, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = 0x3F;
	NRFwriteSingleCommand(WRITE_REG, DYNPD, send);			// Dynamic Payload auf allen Pipes einstellen
	temp = NRFwriteSingleCommand(READ_REG, DYNPD, NOP);
	if (!((0xFF & temp)==send)) {return -1;}

	send = EN_DPL;
	NRFwriteSingleCommand(WRITE_REG, FEATURE, send);		// Dynamic Payload length aktivieren
	temp = NRFwriteSingleCommand(READ_REG, FEATURE, NOP);
	if (!((0xFF & temp)==send)) {return -1;}


	// Adressen setzen.
	// Empfänder RX_P0 muss dabei Transmitter TX Adresse entsprechen!
	NRFwriteCommandNoInterrupt(WRITE_REG, RX_ADDR_P0, ((IsReceiver) ? (unsigned short *)AdressA : (unsigned short *)AdressB), 5);
	sleep(1);
	NRFreadRegNoInterrupt(RX_ADDR_P0, 5);
	sleep(10);
	for (i=5;i>0;i--)
	{	if ( !(NRFValues[i] == ( (IsReceiver) ? AdressA[i - 1] : AdressB[i - 1]) ) )
		{	__no_operation();
			return -3;
		}
	}

	NRFwriteCommandNoInterrupt(WRITE_REG, TX_ADDR, ((IsReceiver) ? (unsigned short *)AdressA : (unsigned short *)AdressB), 5);
	sleep(1); NRFreadRegNoInterrupt(TX_ADDR, 5); sleep(10);
	for (i=5;i>0;i--)
	{	if (!(NRFValues[i] == ((IsReceiver) ? AdressA[i-1] : AdressB[i-1])))
		{	return -4;
		}
	}

	NRFwriteCommandNoInterrupt(WRITE_REG, RX_ADDR_P1, ((!IsReceiver) ? (unsigned short *)AdressA : (unsigned short *)AdressB), 5);
	sleep(1); NRFreadRegNoInterrupt(RX_ADDR_P1, 5); sleep(10);
	for (i=5;i>0;i--)
	{	if (!(NRFValues[i] == ((!IsReceiver) ? AdressA[i-1] : AdressB[i-1])))
		{	return -5;
		}
	}


	NRFwriteSingleCommand(WRITE_REG, RX_ADDR_P2, AdressLastBytes[1]);
	sleep(1);
	NRFwriteSingleCommand(WRITE_REG, RX_ADDR_P3, AdressLastBytes[2]);
	sleep(1);
	NRFwriteSingleCommand(WRITE_REG, RX_ADDR_P4, AdressLastBytes[3]);
	sleep(1);
	NRFwriteSingleCommand(WRITE_REG, RX_ADDR_P5, AdressLastBytes[4]);

	NRFwriteSingleCommand(WRITE_REG, RX_PW_P1, payloadLength);
	sleep(1);
	NRFwriteSingleCommand(WRITE_REG, RX_PW_P2, payloadLength);
	sleep(1);
	NRFwriteSingleCommand(WRITE_REG, RX_PW_P3, payloadLength);
	sleep(1);
	NRFwriteSingleCommand(WRITE_REG, RX_PW_P4, payloadLength);
	sleep(1);
	NRFwriteSingleCommand(WRITE_REG, RX_PW_P5, payloadLength);

	if(IsReceiver){NRFsetRXmode();}

	return 0;
}

short NRFpayloadAvailable(void)
{	short NRFstatus = NRFgetStatus();
	short result = NRFstatus & RX_DR;

	NRFwriteSingleCommand(WRITE_REG, STATUS, RX_DR);

	 if ( NRFstatus & TX_DS )
	{
		 NRFwriteSingleCommand(WRITE_REG, STATUS, TX_DS);
	}

	 if (!((NRFstatus && 0x0E) == 0x0E)){result = 1;}

	 return result;
}

void NRFwriteFIFO(unsigned short *value, short ByteCount)
{	// Überträgt die Daten über SPI

	while (MSPtoNRFbusy) { sleep(1); }
	P3OUT &= ~BIT5;
	P3OUT &= ~BIT6;						// CSN auf low schalten -> NRF auf Bus wählen
	UCB1TXBUF = WR_TX_PLOAD;
	while(UCB1STAT & UCBUSY);
	while(ByteCount)
	{	UCB1TXBUF = *value++;
		ByteCount--;
		while(UCB1STAT & UCBUSY);
	}
	P3OUT |= BIT6;						// AAAAAA hinzugefügt
	P3OUT |= BIT5;
}

/*int NRFreadFIFOint(unsigned short *value)
{	int returnMe;
	while (MSPtoNRFbusy);
	UCB1TXBUF = RD_RX_PLOAD;
	while(UCB1STAT & UCBUSY);
	UCB1TXBUF = NOP;
	while(UCB1STAT & UCBUSY);
	returnMe = UCB1RXBUF<<8;
	UCB1TXBUF = NOP;
	while(UCB1STAT & UCBUSY);
	returnMe = UCB1RXBUF;
	return returnMe;
}*/

int NRFreadFIFO(unsigned short *value, short length)
{	int i;
	short NRFstatus;
	CSNl
/*
	length = NRFgetStatus();			// Status abfragen
	if (!((length && 0x0E)==0x0E))		// Bits 1 - 3 abfragen (<>111 wenn daten da)
	{	length = (length && 0x0E)>>1;	// Datenpipe erfragen
		length = (NRFwriteSingleCommand(READ_REG, RX_PW_P0, NOP) && 0xFF);
	} else {return 1;}
*/
	while (MSPtoNRFbusy);
	UCB1TXBUF = RD_RX_PLOAD;
	while(UCB1STAT & UCBUSY);
	NRFstatus = UCB1RXBUF;

	for (i=length;i>0;i--)
	{ 	UCB1TXBUF = NOP;
		while(UCB1STAT & UCBUSY){sleep(1);}
		*value++ = UCB1RXBUF;
	}
	sleep(4);
	UCB1TXBUF = FLUSH_RX;
	sleep(4);
	CSNh
	NRFsetRXmode();
	sleep(4);
	return 0;
}

void NRFsetRXmode(void)
{	NRFwriteSingleCommand(WRITE_REG, CONFIG, EN_CRC + CRCO + PWR_UP + PRIM_RX);
	CEh
}

void NRFsetTXmode(void)
{	NRFwriteSingleCommand(WRITE_REG, CONFIG, EN_CRC+PWR_UP+CRCO);
	P3OUT |= BIT5;
}

void NRFenterTXmode(void)
{	NRFwriteSingleCommand(WRITE_REG, CONFIG, EN_CRC+PWR_UP+CRCO);
	sleep(1);
	P3OUT |= BIT5;
	__delay_cycles(5000);
	P3OUT &= ~BIT5;
}

void NRFsetStandyModeI(void)
{	NRFwriteSingleCommand(WRITE_REG, CONFIG, EN_CRC+PWR_UP+CRCO);
	P3OUT &= ~BIT5;
}

void NRFpowerDown()
{	NRFwriteSingleCommand(WRITE_REG, CONFIG, EN_CRC+CRCO);
	P3OUT &= ~BIT5;
}

// --------------------------------------------------------------------------------------

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B1_VECTOR
__interrupt void USCI_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B1_VECTOR))) USCI_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch(__even_in_range(UCB1IV,4))
	{
		case 0:break;                           // Vector 0 - no interrupt
		case 2:									// Vector 2 - RXIFG
			*NRFvalueReceive++ = UCB1RXBUF;
			recBytesReceived++;
			if(recBytesReceived <  NRFbyteCount)
			{	UCB1TXBUF = NOP;
			}
			else {
				MSPtoNRFbusy = 0;
				P3OUT |= BIT6;						// CSN auf high schalten -> NRF auf Bus abwählen
				UCB1IE &= ~ UCRXIE;
				LPM1_EXIT;
				P4OUT ^= BIT7;
			}

			break;
		case 4:									// Vector 4 - TXIFG
		// strings, auf die der Pointer UARTnextChar zeigt, senden
			if(NRFbyteCount) {						// noch Bytes vorhanden?
				UCB1TXBUF = *NRFvalueSend++;		// Nächstes byte senden
				NRFbyteCount--;						// Übrige Anzahl verringern
			}
			else {
				UCB1IE &= ~UCTXIE;					// Interrupts ausschalten
				while(UCB1STAT & UCBUSY);
				P3OUT |= BIT6;						// CSN auf high schalten -> NRF auf Bus abwählen
				MSPtoNRFbusy = 0;					// Wieder freigeben (Softwareseitig!)
			}
			break;
		default: break;
	}
}


