 
 /*
  Filename: temperature.c
  Description: This file contains the code for the ADC initzialitaion
	       and Conversion of a ADC Valure into a temperature Value.
*/

#include "includes.h"
#include "com.h"
#include "scheduler.h"
#include "temperature.h"

volatile long temp;

void Temperature_Init()
{
    //adc_result = adc12_single_conversion(REFVSEL_0, ADC12SHT0_8, ADC12INCH_10);
  /* Initialize the shared reference module */ 
  REFCTL0 |= REFMSTR + REFVSEL_0 + REFON;    // Enable internal 1.5V reference
  
  /* Initialize ADC12_A */ 
  ADC12CTL0 = ADC12SHT0_8 + ADC12ON;        // Set sample time 
  ADC12CTL1 = ADC12SHP;                     // Enable sample timer
  ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10;  // ADC input ch A10 => temp sense 
  ADC12IE = 0x001;                          // ADC_IFG upon conv result-ADCMEMO
  
  __delay_cycles(35);                       // 35us delay to allow Ref to settle
                                            // based on default DCO frequency.
                                            // See Datasheet for typical settle
                                            // time. In our case probably not
                                            // necessary because of "long"
                                            // startup phase?
  ADC12CTL0 |= ADC12ENC;
}

// Start Conversion
uint16_t Temperature_Get(uint8_t TEMP_ART)
{
    sint16_t temp2;
    ADC12CTL0 |= ADC12SC;                   // Sampling and conversion start
    
    // LPM3 with interrupts enabled
    EnableInterrupts(); // not necessary
    EnterSleep();
    
    // messwert1: 680 mV = 0 grad
    // messwert2: 850 mV = 70 grad
    // y = grad celsius
    // x = voltage                                      (A10/4096*1500mV) - 680mV
    // y = mx+t
    // m = (y1-y2)/(x1-x2)
    // m = (70 - 0) / (855 - 680) = 70/175 =                    1/2.25 mV
    // t = 0
    // => temperature = ((A10 / 4096 * 1500mV) - 680mV) * (1/2.25mV)
    // = (A10 - 1855) * (667 / 4096)
    temp2 = (((uint32_t) ((uint32_t) temp - 1855)) * 667 * 10) / 4096;
    temp2 = temp2 + 1;
    
    /*
    if (TEMP_ART == TEMP_C) 
    {
    // Temperature in Celsius
    // ((A10/4096*1500mV) - 894mV)*(1/3.66mV) = (A10/4096*410) - 244
    // = (A10 - 2438) * (410 / 4096)
    temp = ((temp - 2438) * 410) / 4096;
    }
    else
    {
    // Temperature in Fahrenheit
    // ((A10/4096*1500mV) - 829mV)*(1/2.033mV) = (A10/4096*738) - 408
    // = (A10 - 2264) * (738 / 4096)
    temp = ((temp - 2264) * 738) / 4096;
    }
    __no_operation();                       // SET BREAKPOINT HERE
    */
    
    return (uint16_t) temp;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  // better code
  
  volatile uint8_t value = ADC12IV;
  if (6 == value) // ADC12IFG0
  {
    temp = ADC12MEM0;
    LeaveSleep();
  }
  
  /*
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
    temp = ADC12MEM0;                       // Move results, IFG is cleared
    LeaveSleep();   // Exit active CPU
    break;
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
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
  */
}