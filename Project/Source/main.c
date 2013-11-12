
/*
  Filename: main.c
  Description: This file contains the main.c (application)
*/

#include "includes.h"
#include "timer.h"
#include "scheduler.h"
#include "button.h"
#include "temperature.h"
#include "main.h"

int main(void)
{
  uint16_t x;
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW | WDTHOLD;
  
  // mostly not done this code here
  // uC Initialisation
  Timer_Init();
  Button_Init();
  Temperature_Init();
  Scheduler_Init();
  
  
  
  x = Temperature_Get(TEMP_C);
  x = x;
  
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
