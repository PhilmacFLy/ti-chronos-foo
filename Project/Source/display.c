
/*
  Filename: display.c
  Description: This file contains the code and functions for the display
               functionality.
*/

#include "includes.h"
#include "scheduler.h"
#include "event.h"
#include "data.h"
#include "main.h"
#include "display.h"

// probably change so the mainfunction handles only a mode

static volatile uint8_t displaycounter = 40; // 5 seconds
static volatile uint8_t displaystate = DSP_STATE_SYNC; // start value
uint8_t currentshownvalue = 0;
  
const uint8_t LCD_Map[] = {
  BOTTOM | TOP | LEFT_BOTTOM | LEFT_TOP | RIGHT_BOTTOM | RIGHT_TOP,         // 0
  RIGHT_BOTTOM | RIGHT_TOP,                                                 // 1
  TOP | RIGHT_TOP | MIDDLE | LEFT_BOTTOM | BOTTOM,                          // 2
  TOP | MIDDLE | BOTTOM | RIGHT_TOP | RIGHT_BOTTOM,                         // 3
  MIDDLE | LEFT_TOP | RIGHT_TOP | RIGHT_BOTTOM,                             // 4
  TOP | LEFT_TOP | MIDDLE | RIGHT_BOTTOM | BOTTOM,                          // 5
  TOP | MIDDLE | BOTTOM | LEFT_TOP | LEFT_BOTTOM | RIGHT_BOTTOM,            // 6
  TOP | RIGHT_TOP | RIGHT_BOTTOM,                                           // 7
  TOP | MIDDLE | BOTTOM | RIGHT_TOP | RIGHT_BOTTOM | LEFT_TOP | LEFT_BOTTOM,// 8
  TOP | MIDDLE | BOTTOM | RIGHT_TOP | RIGHT_BOTTOM | LEFT_TOP               // 9
};

void Display_ShowStringSync()
{
  LCDM1 &= (~0x40); // hide dot
  
  // show "SYNC" string in upper line
  DSP_SHOW_TOPLINE_CHAR(1, S);
  DSP_SHOW_TOPLINE_CHAR(2, Y);
  DSP_SHOW_TOPLINE_CHAR(3, N);
  DSP_SHOW_TOPLINE_CHAR(4, C);
  
  // clear lower line
  DSP_SHOW_BOTTOMLINE_CHAR(1, SPACE);
  DSP_SHOW_BOTTOMLINE_CHAR(2, SPACE);
  DSP_SHOW_BOTTOMLINE_CHAR(3, SPACE);
  DSP_SHOW_BOTTOMLINE_CHAR(4, SPACE);
  DSP_SHOW_BOTTOMLINE_CHAR(5, SPACE);
  
  displaystate = DSP_STATE_SYNC;
  DSP_ACTIVATE();
}

void Display_ShowSetID(uint8_t index)
{
  LCDM1 &= (~0x40); // hide dot
  
  // show current id in top line
  DSP_SHOW_TOPLINE_CHAR(1, SPACE);
  DSP_SHOW_TOPLINE_CHAR(2, SPACE);
  DSP_SHOW_TOPLINE_NUM(3, (index / 10));
  DSP_SHOW_TOPLINE_NUM(4, (index % 10));
  
  // show "SETID" string in bottom line
  DSP_SHOW_BOTTOMLINE_CHAR(1, S);
  DSP_SHOW_BOTTOMLINE_CHAR(2, E);
  DSP_SHOW_BOTTOMLINE_CHAR(3, T);
  DSP_SHOW_BOTTOMLINE_CHAR(4, I);
  DSP_SHOW_BOTTOMLINE_CHAR(5, D);
  
  displaystate = DSP_STATE_SETID;
  DSP_ACTIVATE();
}

void Display_SetShowTemperature()
{
  Display_ShowTemperature(Data_GetValue(currentshownvalue), currentshownvalue);
  displaystate = DSP_STATE_SINGLE_TEMP;
  DSP_ACTIVATE();
}

// todo: replace some defines
void Display_ShowTemperature(uint16_t value, uint8_t index)
{
  LCDM1 |= 0x40; // show dot
  
  if (value > 999)
  {
    DSP_SHOW_TOPLINE_NUM(1, (value / 100)); // 10^2 digit
    DSP_SHOW_TOPLINE_NUM(2, ((value / 10) % 10)); // 10^1 digit
    DSP_SHOW_TOPLINE_NUM(3, (value % 10)); // 10^0 digit
  }
  else
  {
    DSP_SHOW_TOPLINE_CHAR(1, MINUS);
    DSP_SHOW_TOPLINE_CHAR(2, MINUS);
    DSP_SHOW_TOPLINE_CHAR(3, MINUS);
  }
  DSP_SHOW_TOPLINE_CHAR(4, C);
  
  DSP_SHOW_BOTTOMLINE_CHAR(1, I);
  DSP_SHOW_BOTTOMLINE_CHAR(2, D);
  DSP_SHOW_BOTTOMLINE_CHAR(3, SPACE);
  DSP_SHOW_BOTTOMLINE_NUM(4, (index / 10));
  DSP_SHOW_BOTTOMLINE_NUM(5, (index % 10));
}

// display initialization
void Display_Init()
{
  uint8_t i;
  
  P5SEL |= (BIT5 | BIT6 | BIT7);
  P5DIR |= (BIT5 | BIT6 | BIT7);

  // clear LCD memory
  for (i = 0; i < 24; i++)
  {
    *((uint8_t*)(0x0A20 + i)) = 0x00;
  }
  
  // LCD_FREQ = ACLK/32/4, LCD Mux 4, turn on LCD
  LCDBCTL0 =  (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3 + LCDDIV4)| LCDPRE0 | LCD4MUX | LCDSON;// | LCDON;
  LCDBVCTL = LCDCPEN | VLCD_3_44;
  REFCTL0 &= ~REFMSTR;

  // Charge pump generated internally at 3.44V, external bias (V2-V4) generation
  // Internal reference for charge pump
   
  // select needed LCD segments
  LCDBPCTL0 = 0xCCFF;
  LCDBPCTL1 = 0x00FF;
}

// called cyclically, first time after full initialization
// consumes button events
void Display_MainFunction()
{
  if (displaycounter > 0)
  {
    displaycounter--;
    if (displaycounter == 0)
    {
      // lcd timeout
      DSP_DEACTIVATE();
    }
    else
    {
      // update shown data
      if (displaystate == DSP_STATE_SINGLE_TEMP)
      {
        Display_ShowTemperature(Data_GetValue(currentshownvalue), currentshownvalue);
      }
    }
  }
}

void Display_Button_Up_Pressed()
{
  switch (displaystate) 
  {
    case DSP_STATE_SYNC:
      break;
    case DSP_STATE_SETID:
      MyID = (MyID + 1) % 64;
      Display_ShowSetID(MyID);
      break;
    case DSP_STATE_SINGLE_TEMP:
      // next value
      currentshownvalue = (currentshownvalue + 1) % 64;
      Display_ShowTemperature(Data_GetValue(currentshownvalue), currentshownvalue);
      break;
  }
}

void Display_Button_Down_Pressed()
{
  switch (displaystate) 
  {
    case DSP_STATE_SYNC:
      break;
    case DSP_STATE_SETID:
      MyID = (MyID - 1) % 64;
      Display_ShowSetID(MyID);
      break;
    case DSP_STATE_SINGLE_TEMP:
      // previous value
      currentshownvalue = (currentshownvalue - 1) % 64;
      Display_ShowTemperature(Data_GetValue(currentshownvalue), currentshownvalue);
      break;
  }
}

void Display_Button_Star_Pressed()
{
  switch (displaystate) 
  {
    case DSP_STATE_SYNC:
      break;
    case DSP_STATE_SETID:
      break;
    case DSP_STATE_SINGLE_TEMP:
      break;
  }
}

void Display_Button_Num_Pressed()
{
  switch (displaystate) 
  {
    case DSP_STATE_SYNC:
      break;
    case DSP_STATE_SETID:
      break;
    case DSP_STATE_SINGLE_TEMP:
      currentshownvalue = MyID;
      Display_ShowTemperature(Data_GetValue(currentshownvalue), currentshownvalue);
      break;
  }
}

void Display_Handler(EventMaskType ev)
{
  // dispatch events
  if (EVENT_BUTTON_UP == (ev & EVENT_BUTTON_UP))
  {
    ClearEvent(EVENT_BUTTON_UP);
    Display_Button_Up_Pressed();
    DSP_ACTIVATE();
  }

  if (EVENT_BUTTON_DOWN == (ev & EVENT_BUTTON_DOWN))
  {
    ClearEvent(EVENT_BUTTON_DOWN);
    Display_Button_Down_Pressed();
    DSP_ACTIVATE();
  }

  if (EVENT_BUTTON_NUM == (ev & EVENT_BUTTON_NUM))
  {
    ClearEvent(EVENT_BUTTON_NUM);
    Display_Button_Num_Pressed();
    DSP_ACTIVATE();
  }

  if (EVENT_BUTTON_STAR == (ev & EVENT_BUTTON_STAR))
  {
    ClearEvent(EVENT_BUTTON_STAR);
    Display_Button_Star_Pressed();
    DSP_ACTIVATE();
  }

  if (EVENT_BUTTON_BACKLIGHT == (ev & EVENT_BUTTON_BACKLIGHT))
  {
    ClearEvent(EVENT_BUTTON_BACKLIGHT);
    DSP_ACTIVATE();
  }

  Display_MainFunction();
}  
