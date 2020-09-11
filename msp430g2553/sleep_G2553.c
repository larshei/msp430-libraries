/*****************************************************************************
 * Created on 29.01.2015 	 					Lars 'DemoniacMilk' Heinrichs
 *
 * 'Library' for use of a sleep timer on the MSP430G2553
 * External oscillator/crystal recommended, as there is no internal crystal!!
 * not recommended for use in time critical, near zero tolerance scenarios.
 * Uses timer module
 *
 * !! RESETS SETTINGS FOR TIMER A1 ON CALL
 * for periodic / cyclic / continous events, use timer A0. You may still use
 * timer A1 for one shot timers or timers that will not interfere with usage
 * of the sleep function. Make sure to configure the timers to your needs before
 * usage!
 * !! Contains the Timer A1 interrupt vector
 * !! uses the CCR1 interrupt
 *****************************************************************************/

#include <msp430.h>
#include <sleep_G2553.h>
unsigned int _clockSource, _clockFrequency = 0;
unsigned int _cycleCounter, _cycleRest, _cycleLast = 0;

void sleep_setClock(unsigned int clockSource, unsigned long clockFrequencyHZ){
	_clockSource = clockSource;
	if ((clockSource == SMCLK) && (clockFrequencyHZ == AUTO)){
		if 		(BCSCTL1 == CALBC1_1MHZ)  { _clockFrequency = 976   >> ((BCSCTL2 & 0x06 )>>1); }	// 976   = 1MHz >> 10
		else if (BCSCTL1 == CALBC1_8MHZ)  { _clockFrequency = 7812  >> ((BCSCTL2 & 0x06 )>>1); }	// 7812  = 8MHz >> 10
		else if (BCSCTL1 == CALBC1_12MHZ) { _clockFrequency = 11718 >> ((BCSCTL2 & 0x06 )>>1); }	// 11718 = 12MHz >> 10
		else if (BCSCTL1 == CALBC1_16MHZ) { _clockFrequency = 15625 >> ((BCSCTL2 & 0x06 )>>1); }	// 15625 = 16MHz >> 10
	} else {
		_clockFrequency = clockFrequencyHZ >> 10;	// to fit into int data type in all cases (up to 16,000,000)
	}
}

void sleep(unsigned int time, int secOrTenthMillisec){
	int _clockDivider = 0;

	// calculate cycle count
	if (secOrTenthMillisec == SECONDS){
		_clockDivider = ID_3;											// 1<<3 = 8
		_cycleCounter = time * _clockFrequency >> 3;					// clock Count (shiftet by 6: clock is >>10, counts to 0xFFFF --> 1<<16)
		_cycleRest = (_cycleCounter - ((_cycleCounter >> 6) << 6))<<10;	// calculate runtime for last cycle
		_cycleCounter >>= 6;											// correct clock count
	} else {
		_cycleCounter = (long)time * _clockFrequency / 10000;			// clock Count (shiftet by 6: clock is >>10, counts to 0xFFFF --> 1<<16)
		_cycleRest = (_cycleCounter - ((_cycleCounter >> 6) << 6))<<10;	// calculate runtime for last cycle
		_cycleCounter >>= 6;											// correct clock count
		_clockDivider = ID_0;											// set clock divider to 0
	}

	if (_cycleCounter == 0) { TA1R = 0xFFFF - _cycleRest;}		// add 65536-rest, to only count throught the rest
	__enable_interrupt();
	TA1CTL = TACLR;
	TA1CTL = MC_2 + TAIE + _clockDivider + (_clockSource-1)*0x100u;	// cont mode timer (always count to 0xFFFF), enable interrupt
	_bis_SR_register(CPUOFF + GIE);				// go to sleep with activated Interrupts
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer1_A1(void)
{
	  switch( TA1IV )
	  {
	    case  2:  break;				// CCR1
	    case  4:  break;                // CCR2
	    case 10:						// overflow
	      	if(!_cycleCounter){					// no more cycles left
	      		TA1CTL = MC0;					// stop timer
	      		LPM0_EXIT;
	    		} else if (_cycleCounter == 1) {
	    			TA1R = 0xFFFF - _cycleRest;
	    		}
	      	_cycleCounter = _cycleCounter ? _cycleCounter-1 : 0;
	    	break;
	    default:  break;
	  }
}

//Timer_A Interrupt Vector (TA1IV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1_A0(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{

}
