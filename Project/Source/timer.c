
/*
  Filename: timer.c
  Description: This file contains the code for the timer interrupt and it's
               initialization routine. Additionally, it provides a time base for
               the complete software and a function to resynchronize to the
               node.
*/

#include "includes.h"
#include "com.h"
#include "scheduler.h"
#include "event.h"
#include "timer.h"

uint16_t nextnummicroticks = NUM_MICROTICKS;

const EventMaskType TimerEvents[4] = {
  EVENT_COM_SLOT_RX_START,     // Timer 0 CCR1
  EVENT_COM_SLOT_TX_START,     // Timer 0 CCR2
  EVENT_COM_SLOT_OPEN_SYNC_RX, // Timer 0 CCR3
  EVENT_DISPLAY_TICK,          // Timer 0 CCR4
};

// function to initialize the timer
// DOESN'T start the timer!
void Timer_Init()
{
  // Timer 0 used for high resolution timer
  // load zero to count register
  TA0R = 0;
  
  // enable interrupt for CCR0
  // others will be enabled with Timer_SetMode API
  TA0CCTL0 |= CCIE;
  
  // enable interrupt for CCR4 (Capture Compare Register 4)
  // handles the display timings
  TA0CCTL4 |= CCIE;
  
  // ACLK as clock source, with input divider /1
  TA0CTL = TASSEL__ACLK | ID__1;
  
  // load CCR0 with number of microticks
  // and other CCR with corresponding values
  // values will/can be adjusted later
  TA0CCR0 = NUM_MICROTICKS;
  TA0CCR1 = MICROTICK_RX_START;
  TA0CCR2 = MICROTICK_TX_START;
  TA0CCR3 = MICROTICK_OPEN_SYNC_RX;
  
  // load CCR4 with value for display handling
  TA0CCR4 = MICROTICK_DSP_START;
  
  // Timer 1 used for "wait" API
  // load zero to count register
  TA1R = 0;
  
  // enable interrupt for CCR0 (Capture Compare Register 0)
  TA1CCTL0 = CCIE;
  
  // TA0 configured with SMCLK as clock source and clear TAR
  // SMCLK default: ~1045 KHz
  TA1CTL = TASSEL__SMCLK | TACLR;
}

// start timer A0
void Timer_Start(uint16_t position)
{
  // load position
  TA0R = position;
  
  // set timer mode to up mode
  TA0CTL |= MC__UP;
}

// stop timer (and set to zero) A1
void Timer_Stop()
{
  // set timer mode to stopped (leave up mode)
  TA0CTL &= (~MC__UP);
  
  // set count register to zero
  TA0R = 0;
  
  // set ccr register to NUM_MICROTICKS
  TA0CCR0 = NUM_MICROTICKS;
}

void Timer_SetMode(uint8_t mode)
{
  TA0CCTL1 &= (~CCIE);
  TA0CCTL2 &= (~CCIE);
  TA0CCTL3 &= (~CCIE);
  
  if (COM_MODE_PARENT_RX == mode || COM_MODE_CHILD_RX == mode)
  {
    TA0CCTL1 |= CCIE; // enable RX interrupt
  }
  
  if (COM_MODE_TX == mode)
  {
    TA0CCTL2 |= CCIE; // enable TX interrupt
    TA0CCTL3 |= CCIE; // enable RX interrupt for sync slots
  }
}

// function to check the correctness of our clock
// called once every cycle in slot of parent node
void Timer_CorrectSync(uint16_t expectedmicrotick)
{
  // faster version
  nextnummicroticks += (TA0R - expectedmicrotick);
  
  /*
  // get current tick
  uint16_t currentmicrotick = TA0R;
  // own clock to slow, run faster
  if (expectedmicrotick > currentmicrotick)
  {
    nextnummicroticks -= (expectedmicrotick - currentmicrotick);
  }
  
  // own clock to fast, run slower
  if (expectedmicrotick < currentmicrotick)
  {
    nextnummicroticks += (currentmicrotick - expectedmicrotick);
  }
  */
}

// wait x ticks ~ 1 us with timer 1
// not very exactly
void Timer_Delay(uint16_t ticks)
{
  // start timer
  TA1R = ticks;
  TA1CTL |= MC__UP;
  
  // enter LPM3
  EnterSleep();
  
  // LPM3 with ISR left, disable timer 1
  // reason: timer interrupt or other interrupt, application has to react
  // with event
  TA1CTL &= (~MC__UP);
}

// Timer 0 interrupt service routine for CCR0
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
  // reload CCR0
  TA0CCR0 = nextnummicroticks;
  nextnummicroticks = NUM_MICROTICKS;
  
  // reload CCR4
  TA0CCR4 = MICROTICK_DSP_START;
  
  // here we start the next communication slot
  SetEvent(EVENT_COM_SLOT_START);
  
  // leave sleep mode
  LeaveSleep();
}

// Timer 0 interrupt service routine for CCR1-CCR3
// will be disabled at beginning of com slot, if not necessary
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_5_ISR(void)
{
  // Priority: lowest value to highest
  // Timer 0 CCR1 = 0x02
  // Timer 0 CCR2 = 0x04
  // Timer 0 CCR3 = 0x06
  // Timer 0 CCR4 = 0x08
  // Timer 0 CCR5 = 0x0A
  // Timer 0 CCR6 = 0x0C
  // Timer 0 Overflow = 0x0E
  // mapped to 0x00 - 0x06
  SetEvent( TimerEvents[(TA0IV >> 1) - 1] );
  if (0x08 == TA0IV)
  {
    // reload CCR4
    TA0CCR4 += MICROTICK_DSP_CYCLETIME;
  }
  LeaveSleep();
}

// Timer 1 interrupt service routine for CCR0
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  // leave sleep mode
  LeaveSleep();
}