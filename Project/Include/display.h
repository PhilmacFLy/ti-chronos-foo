
/*
  Filename: display.h
  Description: This file contains the declarations and defines for the display
               functionality.
*/

#ifndef __DISPLAY_H__
# define __DISPLAY_H__

#include "event.h"

// code for display
# define BOTTOM         (BIT7)
# define MIDDLE         (BIT1)
# define TOP            (BIT4)
# define LEFT_TOP       (BIT0)
# define LEFT_BOTTOM    (BIT2)
# define RIGHT_TOP      (BIT5)
# define RIGHT_BOTTOM   (BIT6)

# define UNUSED_BIT     (BIT3)

// necessary for lower line
# define FOURBITSWAP(x) (((x >> 4) & 0x0F) | ((x << 4) & 0xF0))

# define TOP_LINE_1      (LCDM2)
# define TOP_LINE_2      (LCDM3)
# define TOP_LINE_3      (LCDM4)
# define TOP_LINE_4      (LCDM6)

# define BOTTOM_LINE_1   (LCDM12)
# define BOTTOM_LINE_2   (LCDM11)
# define BOTTOM_LINE_3   (LCDM10)
# define BOTTOM_LINE_4   (LCDM9)
# define BOTTOM_LINE_5   (LCDM8)

# define DSP_CHAR_S         (TOP | LEFT_TOP | MIDDLE | RIGHT_BOTTOM | BOTTOM)
# define DSP_CHAR_Y         (LEFT_TOP | MIDDLE | RIGHT_TOP | RIGHT_BOTTOM)
# define DSP_CHAR_N         (LEFT_TOP | LEFT_BOTTOM | RIGHT_TOP | RIGHT_BOTTOM)
# define DSP_CHAR_C         (TOP | LEFT_TOP | LEFT_BOTTOM | BOTTOM)
# define DSP_CHAR_I         (RIGHT_TOP | RIGHT_BOTTOM)
# define DSP_CHAR_D         (RIGHT_TOP | MIDDLE | LEFT_BOTTOM | RIGHT_BOTTOM | BOTTOM)
# define DSP_CHAR_E         (LEFT_TOP | LEFT_BOTTOM | TOP | BOTTOM | MIDDLE)
# define DSP_CHAR_L         (LEFT_TOP | LEFT_BOTTOM | BOTTOM)
# define DSP_CHAR_T         (LEFT_BOTTOM | LEFT_TOP | TOP)

# define DSP_CHAR_SPACE     ((uint8_t) 0)
# define DSP_CHAR_MINUS     (MIDDLE)

# define DSP_NUM(x)         (LCD_Map[x])

# define DSP_SHOW_TOPLINE_CHAR(pos, character) TOP_LINE_##pos &= UNUSED_BIT; \
                                               TOP_LINE_##pos |= DSP_CHAR_##character

# define DSP_SHOW_TOPLINE_NUM(pos, num) TOP_LINE_##pos &= UNUSED_BIT; \
                                        TOP_LINE_##pos |= DSP_NUM(num)

# define DSP_SHOW_BOTTOMLINE_CHAR(pos, character) BOTTOM_LINE_##pos &= FOURBITSWAP( UNUSED_BIT ); \
                                                  BOTTOM_LINE_##pos |= FOURBITSWAP( DSP_CHAR_##character )

# define DSP_SHOW_BOTTOMLINE_NUM(pos, num) BOTTOM_LINE_##pos &= FOURBITSWAP( UNUSED_BIT ); \
                                           BOTTOM_LINE_##pos |= FOURBITSWAP( DSP_NUM(num) )

# define DSP_ACTIVATE()     LCDBCTL0 |= LCDON; \
                            displaycounter = 40
# define DSP_DEACTIVATE()   LCDBCTL0 &= (~LCDON)

//Display States
# define DSP_STATE_SYNC (0)
# define DSP_STATE_SETID (1)
# define DSP_STATE_SINGLE_TEMP (2)

void Display_ShowStringSync();
void Display_ShowSetID(uint8_t index);
void Display_SetShowTemperature();
void Display_ShowTemperature(uint16_t value, uint8_t index);

void Display_Init();
void Display_Activate();
void Display_MainFunction();
void Display_Handler(EventMaskType ev);

//Single Button functions
void Display_Button_Up_Pressed();
void Display_Button_Down_Pressed();
void Display_Button_Num_Pressed();
void Display_Button_Star_Pressed();

#endif /* __DISPLAY_H__ */
