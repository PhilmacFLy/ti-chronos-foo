
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

uint8_t temperaturecounter = 0;
uint8_t MyDataCount = 0;

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
      // no clear in here, dispatching in the different handlers
      Com_Handler(ev);
    }
    
    // dispatch display event if incoming
    if (EVENT_DISPLAY_TICK == (ev & EVENT_DISPLAY_TICK))
    {
      ClearEvent(EVENT_DISPLAY_TICK);
      
      if (temperaturecounter == 0)
      {
        // every second: measure temperature (each 8th display update)
        uint16_t newtemp = Temperature_Get();
        if (newtemp != Data_GetValue(MyID))
        {
          Data_SetValue(MyID, newtemp, ++MyDataCount);
          Com_FlagDataForSend(MyID);
        }
      }
      temperaturecounter = (temperaturecounter + 1) % 8;
      
      Display_Handler(ev);
    }
    
  }
}
