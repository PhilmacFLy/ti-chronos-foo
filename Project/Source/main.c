
/*
  Filename: main.c
  Description: This file contains the main.c (application). This function is
               basically the scheduler.
*/

#include "includes.h"
#include "timer.h"
#include "scheduler.h"
#include "button.h"
#include "temperature.h"
#include "display.h"
#include "event.h"
#include "com.h"
#include "flash.h"
#include "data.h"
#include "main.h"

uint8_t MyID = 0xFF;

int main(void)
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW | WDTHOLD; // disable WDG
  
  // initialize all modules
  Timer_Init();
  Button_Init();
  Temperature_Init();
  Display_Init();
  Flash_Init();
  Data_Init();
  Com_Init();
  Scheduler_Init();   // enables interrupts, so should be last one
  
  // Timer_Start(0);
  
  // read calibration data from flash (test, working)
  //Flash_Read(&MyID, 1);
  //MyID++;
  //Flash_Write(&MyID, 1);

  while(1)
  {
    // go to sleep
    EnterSleep();
    // get all events and dispatch
    EventMaskType ev = GetAllEvents();
    
    if (EVENT_DISPLAY_TICK == (ev & EVENT_DISPLAY_TICK))
    {
      ClearEvent(EVENT_DISPLAY_TICK);
      Display_Handler(ev);
    }
    
    if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START) ||
        EVENT_COM_SLOT_RX_START == (ev & EVENT_COM_SLOT_RX_START) ||
        EVENT_COM_SLOT_TX_START == (ev & EVENT_COM_SLOT_TX_START) ||
        EVENT_COM_SLOT_RX_TX_SYNC == (ev & EVENT_COM_SLOT_RX_TX_SYNC))
    {
      // no clear in here, dispatching in Com_Handler
      Com_Handler(ev);
    }
  }
}
