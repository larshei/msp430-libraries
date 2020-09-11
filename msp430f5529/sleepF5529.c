/* =======================================================
 *                  s l e e p . c
 *
 *     Author: Lars Heinrichs           Date: Oct 2014
 *
 *  Contains a sleep function for MSP430
 *  sleep sends MSP to LPM3 mode with active Interrupts
 *  uses the timer module TIMER0_A0 for wake up
 *	Contains an interrupt routine for TIMER0_A0.
 *	Call sleep_Setup() before first usage of sleep()
 * =======================================================
 */

#include <msp430.h>

void sleep_Setup(void);
void sleep(int time);

/*****************************************************************
 * sleep_Setup()
 * Return: -
 * Param:  -
 *
 * Initializes Timer_A, enables interrupts
 *****************************************************************/
void sleep_Setup(void) {
	TA0CCTL0 = CCIE; 		// enable CCR0 interrupt (IF on reaching TA0CCR0)
	TA0CCR0 = 32000; 		// Default timer CCR0, will be overwritten when calling sleep()
	TA0EX0 = 7;				// Set divider to 4 -> 32000 / 8 = 4000 1/s = 1/4 ms
}

/*****************************************************************
 * sleep(int time)
 * Return: -
 * Param:  (int) time
 *
 * sends the MSP to sleep mode (LPM3) disabling most oscillators and
 * the CPU to save energy. Wakes up the device after 'time' 1/4th ms.
 * Maximum wait time: ~8 seconds
 *****************************************************************/
void sleep(int time)
{	time = (time > 32767) ? 32767 : time;
	TA0CTL |= TASSEL_1 + TACLR | MC_1; 		// reset timer, set to up mode
	TA0CCR0 = time; 						// set CCR0
	__bis_SR_register(LPM3_bits + GIE);     // LPM3, Interrupts enable
}

// ---------------------------------------------------------------
// ------------------ I N T E R R U P T S ------------------------
// ---------------------------------------------------------------

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	TA0CTL = MC_0;	// Stop timer
	__bic_SR_register_on_exit(LPM3_bits); 		// Exit low power mode
}
