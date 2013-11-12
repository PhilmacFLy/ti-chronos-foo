
/*
  Filename: timer.h
  Description: This file contains the declarations for the timer interrupt and
               it's initialization routine. Additionally, it provides a time
               base for  the complete software and a function to resynchronize
               to the node.
*/

#ifndef __TIMER_H__
# define __TIMER_H__

// clocking: 32768 Hz divisor: 2
//# define NUM_MICROTICKS 0x7FFFu
# define NUM_MICROTICKS 0x3FFFu // 0x4000 == 32768 / 2

# define MICROTICK_RX_START       0x0FE8u // 24 ticks before TX START
# define MICROTICK_TX_START       0x1000
# define MICROTICK_OPEN_SYNC_RX   0x3000u

# define MICROTICK_DSP_START      0x0800u
# define MICROTICK_DSP_CYCLETIME  0x1000u

void Timer_Init();
void Timer_Start(uint16_t position);
void Timer_Stop(); // needed?
void Timer_SetMode(uint8_t mode);
void Timer_CorrectSync(uint16_t expectedmicrotick);

void Timer_Delay(uint16_t ticks);

#endif /* __TIMER_H__ */
