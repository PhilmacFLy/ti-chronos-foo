
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
uint8_t cycles;

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
  mainstate = MAIN_STATE_INIT;
  
  Timer_Start(0); // start the timer to get a halfway proper timebase
  
  while(1)
  {
    // go to sleep, wakeup through EVENT_COM_SLOT_START
    EnterSleep();
    
    // get all events and dispatch in the switch construct
    EventMaskType ev;
    ev = GetAllEvents();
    
    if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START) ||
        EVENT_COM_SLOT_RX_START == (ev & EVENT_COM_SLOT_RX_START) ||
        EVENT_COM_SLOT_TX_START == (ev & EVENT_COM_SLOT_TX_START) ||
        EVENT_COM_SLOT_RX_TX_SYNC == (ev & EVENT_COM_SLOT_RX_TX_SYNC))
    {
      switch(mainstate)
      {
        case MAIN_STATE_INIT:
          cylces++;
          if (cycles > 16*4) { //Should wait + Random(17) but dunno how
              while(Com_IsInitialized == 0){};
              if (Com_NetworkExists() == 1)
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
          }
        case MAIN_STATE_INIT_MASTER:
          //TODO Implement
          mainstate = MAIN_STATE_COM;
          break;
          
        case MAIN_STATE_INIT_CHILD:
          //TODO Implement
          mainstate = MAIN_STATE_COM;
          break;
          
        case MAIN_STATE_COM:
          {
            // no clear in here, dispatching in Com_Handler
            Com_Handler(ev);
          }
          break;
      }
    }
    
    // dispatch display event if incoming
    if (EVENT_DISPLAY_TICK == (ev & EVENT_DISPLAY_TICK))
    {
      ClearEvent(EVENT_DISPLAY_TICK);
      Display_Handler(ev);
    }
    
  }
}
