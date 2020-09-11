/* =========================================================
 *                    	A D C . c
 *
 *      Author: Lars Heinrichs           Date: Oct 2014
 *
 * Can set up a pin for ADC12, reads a value on ADC12_Read()
 *
 * =========================================================
 */


#include<msp430.h>
#include <sleep.h>

unsigned short i;
unsigned int result;
volatile unsigned int *storageArray;
volatile unsigned int results[4];

void ADC12_Setup()
// Sets up the ADC, 2,5V reference
{	P6SEL |= BIT0 + BIT1 + BIT2 + BIT3;         // A/D Kanal A0 auf P6.0
	REFCTL0 &= ~REFMSTR;                      	// Übergebe REFMSTR Optionen an ADC12_A ref Register

	// ADC12 enable, set sampling time, activate refrence generator, use 2.5V reference voltage
	ADC12CTL0 = ADC12MSC + ADC12ON + ADC12SHT0_2 + ADC12REFON + ADC12REF2_5V;

	// single sequence mode / ACLK clock source / Divide clock by 4 / Sample and hold source: ADC12SC bit
	ADC12CTL1 = ADC12CONSEQ_3 + ADC12DIV_3 + ADC12SHP;

	// RES_2: 12 bit / ADC12SR: Reduced sampling rate (50kbps) for reduced energy consumption
//	ADC12CTL2 = ADC12RES_2 + ADC12SR;

	// Set up reference voltages and inputs for each channel
	ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_0;   	// Reference voltages Vr+=Vref+ and Vr-=AVss, channel A0 (P6.0)
	ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_1;   	// Reference voltages Vr+=Vref+ and Vr-=AVss, channel A0 (P6.1)
	ADC12MCTL2 = ADC12SREF_1 + ADC12INCH_2;   	// Reference voltages Vr+=Vref+ and Vr-=AVss, channel A0 (P6.2)
	ADC12MCTL3 = ADC12SREF_1 + ADC12INCH_3 + ADC12EOS;   // Reference voltages Vr+=Vref+ and Vr-=AVss, channel A0 (P6.3), End of sequence

	ADC12IE = 0x08; // Interrupts for A3 (signals end of sequence)
}

void ADC12_ReadFirstFour(unsigned int *storage)
{	storageArray = storage;
	        // enable conversion
	ADC12CTL0 |= ADC12ENC;
	ADC12CTL0 |= ADC12SC;						// start conversion

	while(ADC12CTL1 & ADC12BUSY){
		sleep(1);								// Wait for sequence to be finished
	}
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
    break;
  case  8: 									// Vector  8:  ADC12IFG1
    break;
  case 10: 									// Vector 10:  ADC12IFG2
	  break;
  case 12:                 					// Vector 12:  ADC12IFG3
	  // sequence measuring: Channels 0..3, if this interrupt fires,
	  // the sequence has been finished.


	  *storageArray++ = ADC12MEM0;			// store results in array
	  *storageArray++ = ADC12MEM1;			// pointer is pointing to first
	  *storageArray++ = ADC12MEM2;			// element in destination array
	  *storageArray++ = ADC12MEM3;             // disable conversion
	  storageArray -= 4;

	  ADC12CTL0 &= ~ADC12SC;			// Clear 'Start Conversion'
	  ADC12CTL0 &= ~ADC12ENC;			// disable Conversion
	  break;
  case 14: break;                   		// Vector 14:  ADC12IFG4
  case 16: break;                   		// Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
