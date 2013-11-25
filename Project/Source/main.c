
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
  Scheduler_Init();   // enables interrupts, so should be last one
  
  // read calibration data from flash (test, working)
  //Flash_Read(&MyID, 1);
  //MyID++;
  //Flash_Write(&MyID, 1);


  
  /*
   start of test code
  */
  uint8_t actLevel = PMMCTL0_L & PMMCOREV_3;
  actLevel = actLevel;
  // flash reading for idiots
  uint8_t* flash_mem_C = (uint8_t*) 0x1800;
  // but is dis workin?
  //*(flash_mem) = 0x00; // OIOIOI
  
  // mostly not done this code here
  // uC Initialisation
  
  
  //x = ((uint16_t)(flash_mem[2]) << 8) + (uint16_t)(flash_mem[3]);
  
  
//*******************************************************************************
// Configure LCD_B
  
  //Display_ShowTemperature(-200);
  //Display_ShowTemperature(400, 24);
  //Display_ShowTemperature(0);
  sint16_t x =Temperature_Get(TEMP_C);
  x++;
  Display_ShowTemperature(123, 45);
  Timer_Start(0);
  
  //LCDM2 = LCD_Map[8];
  //x = Temperature_Get(TEMP_C);
  //x = x;
  
  //Timer_Start();
  
  // here is only testing code!
  
  //Timer_Delay(50);
  //Timer_Start(0);
  //tmp
  //TA0CCTL1 |= CCIE;
  //TA0CCTL2 |= CCIE;
  //TA0CCTL3 |= CCIE;
  //testcall?
  //Timer_CorrectSync(20);
  // never return!
  
  
  /*
   end of test code
  */
  
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
