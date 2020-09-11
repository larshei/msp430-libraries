/*
 * BMX0855.h
 *
 *  Created on: 17.12.2014
 *      Author: DemoniacMilk
 */

#ifndef BMX0855_H_
#define BMX0855_H_

// current consumptions

typedef struct
{

};

// ****** A C C E L E R O M E T E R *******
#define ACC_REG_X_LSB			0x02
#define ACC_REG_X_MSB			0x03
#define ACC_REG_Y_LSB			0x04
#define ACC_REG_Y_MSB			0x05
#define ACC_REG_Z_LSB			0x06
#define ACC_REG_Z_MSB			0x07
#define ACC_REG_Z_TEMP			0x08
#define ACC_REG_INT_STATUS_0	0x09
#define ACC_REG_INT_STATUS_1	0x0A
#define ACC_REG_INT_STATUS_2	0x0B
#define ACC_REG_INT_STATUS_3	0x0C
//	reserved						0x0D
#define ACC_REG_FIFO_STATUS_0		0x0E
#define ACC_REG_RANGE				0x0F
	#define ACC_RANGE_2G			0x03
	#define ACC_RANGE_4G			0x05
	#define ACC_RANGE_8G			0x08
	#define ACC_RANGE_16G			0x0C
#define ACC_REG_BANDWITH			0x10
	#define ACC_BANDWIDTH_8			0x08
	#define ACC_BANDWIDTH_16		0x09
	#define ACC_BANDWIDTH_32		0x0A
	#define ACC_BANDWIDTH_62		0x0B
	#define ACC_BANDWIDTH_125		0x0C
	#define ACC_BANDWIDTH_250		0x0D
	#define ACC_BANDWIDTH_500		0x0E
	#define ACC_BANDWIDTH_1000		0x0F
#define ACC_LOWPOWER_SELECT			0x11	// PMU_LPW
	#define ACC_NORMAL_POWER		0x00	// all powered
	#define ACC_DEEP_SUSPEND		0x20	// lowest power, only interface section oprational, all data is lost
	#define ACC_LOW_POWER			0x40	// sleep an wake up periodically (..-normal-suspend-..)
	#define ACC_SUSPEND				0x80	// Disable analog section, keep digital and interface section alive
	#define ACC_SLEEP_0_5			0x00	// sleep durations for low power mode
	#define ACC_SLEEP_2				0x06
	#define ACC_SLEEP_6				0x12
	#define ACC_SLEEP_25			0x16
	#define ACC_SLEEP_100			0x1A
	#define ACC_SLEEP_1000			0x1E
#define ACC_REG_LOWPOWER_CONFIG		0x12	// PMU_LOWPOWER
	#define ACC_LPM1				0x00
	#define ACC_LPM2				0x40	// register access still possible
	#define ACC_LPM_TIMER_EVENT		0x00	// event based measuring
	#define ACC_LPM_TIMER_EQUI		0x20	// equidistant measuring
#define ACC_REG_HBW					0x13
	#define ACC_READ_FILTERED		0x00	// data_high_bw
	#define ACC_READ_UNFILTERED		0x80
	#define ACC_LOCK_MSB			0x40	// shadow_dis
	#define ACC_NOLOCK_MSB			0x00
#define ACC_REG_SOFTRESET			0x14
	#define ACC_SOFTRESET			0xB6	// softresets, deletes all user configurations
// reserved								0x15
#define ACC_REG_INT_EN_0			0x16
	#define ACC_FLATIE				0x80
	#define ACC_ORIENTIE			0x40
	#define ACC_STAPIE				0x20
	#define ACC_DTAPIE				0x10
//	reserved
	#define ACC_SLOPZIE				0x04
	#define ACC_SLOPYIE				0x02
	#define ACC_SLOPXIE				0x01
#define ACC_REG_INT_EN_1			0x17
//	reserved
	#define ACC_FWMTIE				0x40	// FIFO Watermark IE
	#define ACC_FULLIE				0x20	// FIFO full
	#define ACC_DATAIE				0x10	// data ready
	#define ACC_LOWGIE				0x08	// Low G interrupt
	#define ACC_HIZGIE				0x04	// high G interrupt, Z axis
	#define ACC_HIYGIE				0x02	// high G interrupt, Y axis
	#define ACC_HIXGIE				0x01	// high G interrupt, X axis
#define ACC_REG_INT_EN_2			0x17
//	4 x reserved
	#define ACC_NOMOIE				0x08	// Low G interrupt
	#define ACC_NOMOZIE				0x04	// high G interrupt, Z axis
	#define ACC_NOMOYIE				0x02	// high G interrupt, Y axis
	#define ACC_NOMOXIE				0x01	// high G interrupt, X axis
#define ACC_REG_INT_MAP_0			0x19
#define ACC_REG_INT_MAP_1			0x1A
#define ACC_REG_INT_MAP_2			0x1B
#define ACC_REG_INT_SRC				0x1E	// Use filtered (0) or unfiltered data (1) for interrupt generation
	#define ACC_SRCDATA				0x20	// Source for triggering data ready interrupts
	#define ACC_SRCTAP				0x10	// Source for triggering Taps interrupts
	#define ACC_SRCSLONOMO			0x08	// Source for triggering slow/no motion interrupts
	#define ACC_SRCSLOP				0x04	// Source for triggering slope interrupts
	#define ACC_SRCHI				0x02	// Source for triggering high-g interrupts
	#define ACC_SRCLO				0x01	// Source for triggering low-g interrupts
#define ACC_REG_INT_OUTCTRL			0x20
	#define ACC_INT2_OPENDRAIN		0x08	// Open drain or push-pull
	#define ACC_INT2_HILOINT		0x04	// Level for active interrupt (high low)
	#define ACC_INT1_OPENDRAIN		0x02	// Open drain or push-pull
	#define ACC_INT1_HILOINT		0x01	// Level for active interrupt (high low)
#define ACC_REG_RSTLATCH			0x21	// * contains interrupt reset and interrupt mode *
	#define ACC_INT_RESET			0x80	// Clear any latched interrupts
	#define ACC_INT_LATCHED			0x07
	#define ACC_INT_NLATCHED		0x08
	#define ACC_INT_TMP250M			0x01	// Interrupts being temporary for 250ms
	#define ACC_INT_TMP500M			0x02	// Interrupts being temporary for 500ms
	#define ACC_INT_TMP1S			0x03	// Interrupts being temporary for 1s
	#define ACC_INT_TMP2S			0x04	// Interrupts being temporary for 2s
	#define ACC_INT_TMP4S			0x05	// Interrupts being temporary for 4s
	#define ACC_INT_TMP8S			0x06	// Interrupts being temporary for 8s
	#define ACC_INT_TMP250U			0x09	// Interrupts being temporary for 250µs
	#define ACC_INT_TMP500U			0x0A	// Interrupts being temporary for 500µs
	#define ACC_INT_TMP1M			0x0B	// Interrupts being temporary for 1ms
	#define ACC_INT_TMP12M			0x0C	// Interrupts being temporary for 12,5ms
	#define ACC_INT_TMP25M			0x0D	// Interrupts being temporary for 25ms
	#define ACC_INT_TMP50M			0x0E	// Interrupts being temporary for 50ms
#define ACC_REG_LOGDELAY			0x22	// * defines the delay before a low g interrupt is generated: 2*(value+1) ms
#define ACC_REG_LOGTHR				0x23	// * defines the threshold of a low g interrupt is generated: 7.8*(value) mg
#define ACC_REG_INTHYS				0x24	// * defines the hysteresis of low and high g interrupts, selects single or combined axis mode
	#define ACC_LOGHYS				0x01	// Enable hysteresis for low g interrupt (value always 125 mg)
	#define ACC_HIGHYS				0x80	// Enable hysteresis for hi g intrrupt (value depends on range)
	#define ACC_LOGCOMBINEAXIS		0x04	// combine all axis for low g interrupt generation isntead of going for single axis
#define ACC_REG_HIGDELAY			0x25	// * defines the delay before a hi g interrupt is generated: 2*(value+1) ms
#define ACC_REG_HIGTHR				0x26	// * defines the threshold of a hi g interrupt is generated: 7.8*(value) mg
#define ACC_REG_SLOMOHYSDEL			0x27	// * defines the threshold and durations for a slo/no-mo interrupt to be triggered *
#define ACC_REG_ANYMOTHR			0x28	// * defines the threshold for the any motion interrupt / range*1,95 1/g  mg
#define ACC_REG_SLOMOTHR			0x29	// * defines the threshold for the any motion interrupt / range*1,95 1/g  mg
#define ACC_REG_TAPDEL				0x2A	// * defines the duration for single and double taps
	#define ACC_TAP_QUIETDUR		0x80	// 30ms (0) or 20ms (1)
	#define ACC_TAP_SHOCKDUR		0x40	// shock duration of 50ms (0) or 75ms (1)
	#define ACC_TAP_TIMWIN50M		0x00	// Time window for second tap to appear 50M
	#define ACC_TAP_TIMWIN100M		0x01	// Time window for second tap to appear 100M
	#define ACC_TAP_TIMWIN150M		0x02	// Time window for second tap to appear 150M
	#define ACC_TAP_TIMWIN200M		0x03	// Time window for second tap to appear 200M
	#define ACC_TAP_TIMWIN25M		0x04	// Time window for second tap to appear 250M
	#define ACC_TAP_TIMWIN375M		0x05	// Time window for second tap to appear 375M
	#define ACC_TAP_TIMWIN500M		0x06	// Time window for second tap to appear 500M
	#define ACC_TAP_TIMWIN700M		0x07	// Time window for second tap to appear 700M
#define ACC_REG_TAPCTL				0x2B	// * controls sample count and threshold for the tap interrupt *
	#define ACC_TAPSAMCNT2			0x00	// 2 samples to be checked
	#define ACC_TAPSAMCNT4			0x40	// 4 samples to be checked
	#define ACC_TAPSAMCNT8			0x80	// 8 samples to be checked
	#define ACC_TAPSAMCNT16			0xC0	//16 samples to be checked
	#define ACC_TAPTHR				0x01	// correct?!
#define ACC_REG_ORCTL				0x2C	// * control register for orientation interrupts (hysteresis, blocking and mode)
	#define	ACC_ORHYS				0x10	// ACC_ORHYS * n (0..7) -> n*62.5 mg
	#define	ACC_ORHYS				0x04	// ACC_ORHYS * n (0..7) -> n*62.5 mg
	#define	ACC_ORHYS				0x10	// ACC_ORHYS * n (0..7) -> n*62.5 mg
	#define	ACC_ORHYS				0x10	// ACC_ORHYS * n (0..7) -> n*62.5 mg
#define ACC_REG_SELFTEST			0x32		// Set to 8g range!
	#define ACC_SELFTEST_X			0x01
	#define ACC_SELFTEST_Y			0x02
	#define ACC_SELFTEST_Z			0x03

#define ACC_REG_OFFCTL				0x36
	#define ACC_OFFRST				0x80
	#define ACC_OFFTRGX				0x20	// trigger offset calibration for X-Axis
	#define ACC_OFFTRGY				0x40	// trigger offset calibration for Y-Axis
	#define ACC_OFFTRGZ				0x60	// trigger offset calibration for Z-Axis
	#define ACC_OFFREADY			0x10	// Hi if device is ready for calibration
	#define ACC_OFFSLOX				0x04	// enable (1) or disable (0) slow offset compensation for X-Axis
	#define ACC_OFFSLOY				0x02	// enable (1) or disable (0) slow offset compensation for Y-Axis
	#define ACC_OFFSLOZ				0x01	// enable (1) or disable (0) slow offset compensation for Z-Axis

#define ACC_REG_OFFSETRESET			0x36		// Write 1 to offset_reset reset all offsets
#define ACC_REG_OFFSET_X			0x38
#define ACC_REG_OFFSET_Y			0x39
#define ACC_REG_OFFSET_Z			0x3A

#define ACC_REG_OFFTRGT				0x37	// * sets targets for offset compensation
	#define ACC_OFFTRGT_Z_0G		0x00	// Offset target for Z-Axis:  0g
	#define ACC_OFFTRGT_Z_1G		0x20	// Offset target for Z-Axis:  1g
	#define ACC_OFFTRGT_Z_M1G		0x40	// Offset target for Z-Axis: -1g
	#define ACC_OFFTRGT_Y_0G		0x00	// Offset target for Z-Axis:  0g
	#define ACC_OFFTRGT_Y_1G		0x08	// Offset target for Z-Axis:  1g
	#define ACC_OFFTRGT_Y_M1G		0x10	// Offset target for Z-Axis: -1g
	#define ACC_OFFTRGT_X_0G		0x00	// Offset target for Z-Axis:  0g
	#define ACC_OFFTRGT_X_1G		0x02	// Offset target for Z-Axis:  1g
	#define ACC_OFFTRGT_X_M1G		0x04	// Offset target for Z-Axis: -1g
	#define ACC_OFFCT				0x01	// Offset calibration low pass cutoff enable


#define ACC_REG_FIFOCTL				0x3E
	#define ACC_FIFO_BYPASS			0x00	// only store neweset value
	#define ACC_FIFO_FIFO			0x40	// Store 32 valeues then stop
	#define ACC_FIFO_STREAM			0x80	// Store 32 newest values
	#define ACC_FIFO_X				0x01	// only store X values
	#define ACC_FIFO_Y				0x02	// only store Y values
	#define ACC_FIFO_Z				0x03	// only store Z values
	#define ACC_FIFO_XYZ			0x00	// store X+Y+Z values

// MAGNETOMETER



#endif /* BMX0855_H_ */
