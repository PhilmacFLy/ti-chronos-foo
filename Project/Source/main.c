
/*
  Filename: main.c
  Description: This file contains the main.c (application)
*/

#include "includes.h"
#include "timer.h"
#include "scheduler.h"
#include "main.h"

int main(void)
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW | WDTHOLD;
  
  // mostly not done this code here
  // uC Initialisation
  Timer_Init();
  //Timer_Start();
  
  // enable interrupts
  //__bis_SR_register(GIE);
  EnableInterrupts();
  
  // here is only testing code!
  
  Timer_Delay(50);
  Timer_Start(0);
  //tmp
  //TA0CCTL1 |= CCIE;
  //TA0CCTL2 |= CCIE;
  //TA0CCTL3 |= CCIE;
  //testcall?
  Timer_CorrectSync(20);
  // never return!
  while(1)
  {
    EnterSleep();
  }
}
