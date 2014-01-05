
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
#include "transceiver.h"

// -----------TRANSMIT TEST CODE ---------------
//// LCD Segments
//#define LCD_A    BIT4
//#define LCD_B    BIT5
//#define LCD_C    BIT6
//#define LCD_D    BIT7
//#define LCD_F    BIT0
//#define LCD_G    BIT1
//#define LCD_E    BIT2
//#define LCD_H    BIT3
//
//#define  PACKET_LEN         (0x05)	    // PACKET_LEN <= 61
//
//const unsigned char myTxBuffer[6]= {PACKET_LEN, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
//unsigned int k = 0;
//
//
//const unsigned char  LCD_Char_Map[] =
//{
//  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F,        // '0' or 'O'
//  LCD_B+LCD_C,                                // '1' or 'I'
//  LCD_A+LCD_B+LCD_D+LCD_E+LCD_G,              // '2' or 'Z'
//  LCD_A+LCD_B+LCD_C+LCD_D+LCD_G,              // '3'
//  LCD_B+LCD_C+LCD_F+LCD_G,                    // '4' or 'y'
//  LCD_A+LCD_C+LCD_D+LCD_F+LCD_G,              // '5' or 'S'
//  LCD_A+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,        // '6' or 'b'
//  LCD_A+LCD_B+LCD_C,                          // '7'
//  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,  // '8' or 'B'
//  LCD_A+LCD_B+LCD_C+LCD_D+LCD_F+LCD_G,        // '9' or 'g'
//};
// ----------- END TRANSMIT TEST CODE ---------------

uint8_t MyID = 0xFF;

uint8_t temperaturecounter = 0;
uint8_t MyDataCount = 0;

// as function so the variables will be destroyed after
void InitID()
{
  volatile uint8_t OKpressed = 0;
  EventMaskType ev;
  
  Flash_Read(&MyID, 1);
  Display_ShowSetID(MyID);
  
  while(OKpressed == 0)
  {
    EnterSleep(); // wait for a timed event, exact timing not really necessary here
    ev = GetAllEvents();
    ClearEvent(~0); // to clear all events at once, however not perfect code :/
    if (EVENT_BUTTON_NUM == (ev & EVENT_BUTTON_NUM)) OKpressed = 1; // dispatch here
    if (EVENT_DISPLAY_TICK == (ev & EVENT_DISPLAY_TICK)) Display_Handler(ev);
  }
  
  Flash_Write(&MyID, 1);
}

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
  Radio_Init();
  Scheduler_Init();   // enables interrupts, so should be last one
  
  Timer_Start(0); // start the timer to get a halfway proper timebase
  
  InitID(); // asking for ID
  
// -----------TRANSMIT TEST CODE ---------------
//
//  //LCD
//  P5SEL |= (BIT5 | BIT6 | BIT7);
//  P5DIR |= (BIT5 | BIT6 | BIT7);
//  LCDBCTL0 =  (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3 + LCDDIV4)| LCDPRE0 | LCD4MUX | LCDON | LCDSON;
//  LCDBVCTL = LCDCPEN | VLCD_3_44;
//  LCDBCTL0 |= LCDON + LCDSON;
//  REFCTL0 &= ~REFMSTR;
//  LCDBPCTL0 = 0x0030;
//  LCDBPCTL1 = 0x0000;  
//  
//  k = 0;
//  
//  while (1)
//  { 
//      
//      LCDM3 = LCD_Char_Map[k];          // Display Character
//      __delay_cycles(1000000);           // Delay ~1sec
//
//      if (++k == 10)
//        k = 0;
//      
//      P1IFG = 0; 
//     
//      ReceiveOff();
//
//      PrepareTransmit( (unsigned char*)myTxBuffer, sizeof myTxBuffer);
//      StartTransmit();
//
//      __delay_cycles(1000000);           // Delay ~1sec       
//  }
// ----------- END TRANSMIT TEST CODE ---------------
  
  
  
  while(1)
  {
    // go to sleep, wakeup through EVENT_COM_SLOT_START
    EnterSleep();
    
    // get all events and dispatch in the switch construct
    EventMaskType ev;
    ev = GetAllEvents();
    
    if ( (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START))
      || (EVENT_COM_SLOT_RX_START == (ev & EVENT_COM_SLOT_RX_START))
      || (EVENT_COM_SLOT_TX_START == (ev & EVENT_COM_SLOT_TX_START))
      || (EVENT_COM_SLOT_RX_TX_SYNC == (ev & EVENT_COM_SLOT_RX_TX_SYNC))
      || (EVENT_TRCV_RX_EVENT == (ev & EVENT_TRCV_RX_EVENT)) )
    {
      // no clearing in here, will be done in subhandlers
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
