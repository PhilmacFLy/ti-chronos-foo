
/*
  Filename: button.c
  Description: This file contains the code for the button functionality (mostly
               only interrupt code)
*/

#include "includes.h"
#include "scheduler.h"
#include "event.h"
#include "button.h"

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
  // save P2IFG flags, only enabled bits
  uint8_t intflags = P2IFG;
  intflags &= P2IE;
  
  // dispatch buttons
  if (IS_BUTTON_PRESSED(intflags, BUTTON_DOWN)) SetEvent(EVENT_BUTTON_DOWN);
  if (IS_BUTTON_PRESSED(intflags, BUTTON_NUM)) SetEvent(EVENT_BUTTON_NUM);
  if (IS_BUTTON_PRESSED(intflags, BUTTON_STAR)) SetEvent(EVENT_BUTTON_STAR);
  if (IS_BUTTON_PRESSED(intflags, BUTTON_BACKLIGHT)) SetEvent(EVENT_BUTTON_BACKLIGHT);
  if (IS_BUTTON_PRESSED(intflags, BUTTON_UP)) SetEvent(EVENT_BUTTON_UP);
  
  // reset P2IFG (interrupt flags)
  DisableInterrupts();
  P2IFG &= ~(intflags);
  EnableInterrupts();
}