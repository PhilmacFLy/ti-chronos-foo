
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
uint8_t mainstate = MAIN_STATE_UNINIT;
uint8_t master = 0;

int main(void)
{
  switch(mainstate)
  {
  // Stop watchdog timer to prevent time out reset
    case MAIN_STATE_UNINIT: 
      WDTCTL = WDTPW | WDTHOLD; // disable WDG
    
      // initialize all modules
      Timer_Init();
      Button_Init();
      Temperature_Init();
      Display_Init();
      Flash_Init();
      Data_Init();
      Scheduler_Init();   // enables interrupts, so should be last one
      mainstate = MAIN_STATE_INIT;
      break;
    case MAIN_STATE_INIT:
      //warte 16 sekunden plus Random(17);
      if Com_Networkexists = 1 then
      {
	master = 0;
	mainstate = MAIN_STATE_INIT_CHILD;
      }
      else
      {
	master = 1;
	mainstate = MAIN_STATE_INIT_MASTER;
      }
      break;
    case MAIN_STATE_INIT_MASTER:
      //TODO Implement
      mainstate = MAIN_STATE_COM_MASTER;
      break;
    case MAIN_STATE_INIT_CHILD:
      //TODO Implement
      mainstate = MAIN_STATE_COM_CHILD;
      break;
    case MAIN_STATE_COM_MASTER:
      //TODO Implement
      break;
    case MAIN_STATE_COM_CHILD;
      //TODO Implement
      break;
  }

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
