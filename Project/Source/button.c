
/*
  Filename: button.c
  Description: This file contains the code for the button functionality (mostly
               only interrupt code)
*/

#include "includes.h"
#include "scheduler.h"
#include "event.h"
#include "button.h"

const EventMaskType ButtonEvents[] = {
  EVENT_BUTTON_DOWN,         // Port 2.0
  EVENT_BUTTON_NUM,          // Port 2.1
  EVENT_BUTTON_STAR,         // Port 2.2
  EVENT_BUTTON_BACKLIGHT,    // Port 2.3
  EVENT_BUTTON_UP            // Port 2.4
};

// initialize button code
void Button_Init(void)
{
  // set direction to IN (0)
  P2DIR &= ALL_BUTTONS;
  
  // enable external pull down
  P2OUT &= ~ALL_BUTTONS;
  P2REN |= ALL_BUTTONS;
  
  // IRQ triggers on rising edge
  P2IES &= ~ALL_BUTTONS;

  // Reset IRQ flags
  P2IFG &= ~ALL_BUTTONS;

  // Enable button interrupts
  P2IE |= ALL_BUTTONS;
}

// interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
  // Priority: lowest value to highest
  // Port 2.0 = 0x02
  // Port 2.1 = 0x04
  // Port 2.2 = 0x06
  // Port 2.3 = 0x08
  // Port 2.4 = 0x0A
  // mapped to 0x00 - 0x04
  SetEvent( ButtonEvents[(P2IV >> 1) - 1] );
}