
/*
  Filename: button.h
  Description: This file contains the declarations for the button functionality
*/

#ifndef __BUTTON_H__
# define __BUTTON_H__

#define BUTTON_DOWN         (BIT0)
#define BUTTON_NUM          (BIT1)
#define BUTTON_STAR         (BIT2)
#define BUTTON_BACKLIGHT    (BIT3)
#define BUTTON_UP           (BIT4)
#define ALL_BUTTONS         (BUTTON_STAR + BUTTON_NUM + BUTTON_UP + BUTTON_DOWN + BUTTON_BACKLIGHT)

void Button_Init(void);

#endif /* __BUTTON_H__ */
