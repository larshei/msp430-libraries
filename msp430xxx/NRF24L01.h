/* ========================================
 *
 * Copyright Benjamin Heemann, 2014
 * Extended by Lars Heinrichs, 2014
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * ========================================
*/


#ifndef NRF24L01_H_
#define NRF24L01_H_
//********************************************************************************************************************//
// SPI(nRF24L01) registers(addresses)

#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address, may be used to get number of bytes in FIFO for this pipe
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address, may be used to get number of bytes in FIFO for this pipe
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address, may be used to get number of bytes in FIFO for this pipe
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address, may be used to get number of bytes in FIFO for this pipe
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address, may be used to get number of bytes in FIFO for this pipe
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address, may be used to get number of bytes in FIFO for this pipe
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define DYNPD           0x1C  // Enable dynamic payload lengh
#define FEATURE         0x1D  // Feature Register

//********************************************************************************************************************//
// SPI(nRF24L01) commands

#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register


//********************************************************************************************************************//
// SPI(nRF24L01) Functions, added by Lars Heinrichs

void SPI_Setup(void);
void NRFwriteCommand(unsigned short command, short reg, unsigned short *value, short byteCount);
short NRFwriteSingleCommand(short command, short reg, short value);
short NRFsetup(short IsReceiver);
void NRFwriteFIFO(unsigned short *value, short ByteCount);
int NRFreadFIFO(unsigned short *value, short length);
int NRFreadFIFOint();
void NRFsetRXmode(void);
void NRFsetTXmode(void);
int NRFresetMaxRT();
int NRFresetTXDS();
void NRFpowerDown();
void resetStatusByte();
void NRFenterTXmode(void);
void NRFsetStandbyModeI(void);
void NRFsetStandbyModeII(void);
short NRFgetStatus(void);
void NRFreadReg(short reg, short length);
short NRFpayloadAvailable(void);

//********************************************************************************************************************//
// SPI(nRF24L01) register settings, added by Lars Heinrichs, non-complete list of options

// Control register (default: (0)000 1000)
#define MASK_RX_DR		0x40  // Mask Interrupt caused by RX_DR; 1: No Interrupt on IRQ; 0: active low interrupt on IRQ
#define MASK_TX_DS		0x20  // Mask Interrupt caused by TX_DR; 1: No Interrupt on IRQ; 0: active low interrupt on IRQ
#define MASK_MAX_RT		0x10  // Mask Interrupt caused by MAX_RT; 1: No Interrupt on IRQ; 0: active low interrupt on IRQ
#define EN_CRC			0x08  // CNC enable
#define CRCO			0x04  // CRC encoding scheme; 0: 1 Byte; 1: 2 Bytes
#define PWR_UP			0x02  // 1: Power up; 0: Power down
#define PRIM_RX			0x01  // RX/TX control: 1: PRX; 2: PTX

// EN_AA register (default: (00)11 1111)
#define ENAA_P5			0x20  // Enable Auto Acknowelegement on data pipe 5
#define ENAA_P4			0x10  // Enable Auto Acknowelegement on data pipe 4
#define ENAA_P3			0x08  // Enable Auto Acknowelegement on data pipe 3
#define ENAA_P2			0x04  // Enable Auto Acknowelegement on data pipe 2
#define ENAA_P1			0x02  // Enable Auto Acknowelegement on data pipe 1
#define ENAA_P0			0x01  // Enable Auto Acknowelegement on data pipe 0
#define ENAA_DISABLE	0x00  // Alle AutoAck ausschalten

// EN_RXADDR register (default: (00)11 1111)
#define ERX_P5			0x20  // Enable data pipe 5
#define ERX_P4			0x10  // Enable data pipe 4
#define ERX_P3			0x08  // Enable data pipe 3
#define ERX_P2			0x04  // Enable data pipe 2
#define ERX_P1			0x02  // Enable data pipe 1
#define ERX_P0			0x01  // Enable data pipe 0

// SETUP_AW register (default: (0000 00)11)
#define AW_3			0x01  // 3 Bytes RX/TX adress field width
#define AW_4			0x02  // 4 Bytes RX/TX adress field width
#define AW_5			0x03  // 5 Bytes RX/TX adress field width

// SETUP_RETR register (default: 0000 0011) Auto retransmit setup
#define ARD_250			0x00  // 250탎 retransmit delay
#define ARD_500			0x01  // 500탎 retransmit delay
#define ARD_750			0x02  // 750탎 retransmit delay
#define ARD_1000		0x03  // 1000탎 retransmit delay
#define ARD_1250		0x04  // 1250탎 retransmit delay
#define ARD_1500		0x05  // 1500탎 retransmit delay
#define ARD_1750		0x06  // 1750탎 retransmit delay
#define ARD_2000		0x07  // 2000탎 retransmit delay
#define ARD_2250		0x08  // 2250탎 retransmit delay
#define ARD_2500		0x09  // 2500탎 retransmit delay
#define ARD_2750		0x10  // 2750탎 retransmit delay
#define ARD_3000		0x11  // 3000탎 retransmit delay
#define ARD_3250		0x12  // 3250탎 retransmit delay
#define ARD_3500		0x13  // 3500탎 retransmit delay
#define ARD_3750		0x14  // 3750탎 retransmit delay
#define ARD_4000		0x15  // 4000탎 retransmit delay
#define ARC_0			0x00  // No automatic retransmit
#define ARC_1			0x01  // Up to 1 auto retransmit on fail of AA
#define ARC_2			0x02  // Up to 2 auto retransmit on fail of AA
#define ARC_3			0x03  // Up to 3 auto retransmit on fail of AA
#define ARC_4			0x04  // Up to 4 auto retransmit on fail of AA
#define ARC_5			0x05  // Up to 5 auto retransmit on fail of AA
#define ARC_6			0x06  // Up to 6 auto retransmit on fail of AA
#define ARC_7			0x07  // Up to 7 auto retransmit on fail of AA
#define ARC_8			0x08  // Up to 8 auto retransmit on fail of AA
#define ARC_9			0x09  // Up to 9 auto retransmit on fail of AA
#define ARC_10			0x10  // Up to 10 auto retransmit on fail of AA
#define ARC_11			0x11  // Up to 11 auto retransmit on fail of AA
#define ARC_12			0x12  // Up to 12 auto retransmit on fail of AA
#define ARC_13			0x13  // Up to 13 auto retransmit on fail of AA
#define ARC_14			0x14  // Up to 14 auto retransmit on fail of AA
#define ARC_15			0x15  // Up to 15 auto retransmit on fail of AA

// RF_CH (default: (0)000 0010): any combination of last 7 bits for selecting the channel
	// nothing to define

// RF_SETUP (default: 0(0)00 111X) RF setup register
#define CONT_WAVE		0x80  // Enable continous carrier transmit
#define RF_DR_LOW		0x20  // Set low RF data rate (250kbps)
#define PLL_LOCK		0x10  // Force PLL lock signal. Only for tests.
#define RF_DR_HIGH		0x08  // Select high data rate mode (dont care if RF_DR_LOW enabled); 0: 1Mbps; 1: 2Mbps
#define RF_PWR_FULL		0x06  //   0dB output power in TX mode
#define RF_PWR_HIGH		0x04  // - 6dB output power in TX mode
#define RF_PWR_MID		0x02  // -12dB output power in TX mode
#define RF_PWR_LOW		0x00  // -18dB output power in TX mode

// STATUS (default: (0)000 1110) STATUS register, sent firstr SPI in each SPI communication
#define RX_DR			0x40  // Data received and ready FIFO Interrupt Flag (write 1 to clear)
#define TX_DS			0x20  // Data sent FIFO Interrupt Flag (write 1 to clear). In AutoAck mode only writte if AutoAck received
#define MAX_RT			0x10  // Maximum number of TX interrupts reached flag
#define RX_P_NO_0		0x00  // Data pipe 0 FIFO - data available for reading
#define RX_P_NO_1		0x02  // Data pipe 1 FIFO - data available for reading
#define RX_P_NO_2		0x04  // Data pipe 2 FIFO - data available for reading
#define RX_P_NO_3		0x06  // Data pipe 3 FIFO - data available for reading
#define RX_P_NO_4		0x08  // Data pipe 4 FIFO - data available for reading
#define RX_P_NO_5		0x0A  // Data pipe 5 FIFO - data available for reading
#define RX_P_NO_WINDOW	0x0E  // & apply to status register, then compare to RX_P_NO_x
//#define TX_FULL			0x01  // TX FIFO Full flag; 1:Full; 0: locations available in FIFO

// FIFO_STATUS (default:(0)001 (00)01) FIFO status register
#define TX_REUSE		0x40
#define TX_FULL			0x20
#define TX_EMPTY		0x10
#define RX_FULL			0x02
#define RX_EMPTY		0x01

// DYNPD (default: (00)00 0000) Dynamic payload length register
#define DPL_P5			0x20  // requires FEATURE:EN_DPL and EN_AA:ENAA_P5; Enable Dynamic Payload on pipe 5
#define DPL_P4			0x10  // requires FEATURE:EN_DPL and EN_AA:ENAA_P4; Enable Dynamic Payload on pipe 4
#define DPL_P3			0x08  // requires FEATURE:EN_DPL and EN_AA:ENAA_P3; Enable Dynamic Payload on pipe 3
#define DPL_P2			0x04  // requires FEATURE:EN_DPL and EN_AA:ENAA_P2; Enable Dynamic Payload on pipe 2
#define DPL_P1			0x02  // requires FEATURE:EN_DPL and EN_AA:ENAA_P1; Enable Dynamic Payload on pipe 1
#define DPL_P0			0x01  // requires FEATURE:EN_DPL and EN_AA:ENAA_P0; Enable Dynamic Payload on pipe 0

// FEATURE register (default: (0000 0)000) Feature register
#define EN_DPL			0x04  // Enables Dynamic Payload Length
#define EN_ACK_PAY		0x02  // Enables Payload with ACK
#define EN_DYN_ACK		0x01  // Enables the W_TX_PAYLOAD_NOACK command


#endif
/* [] END OF FILE */

