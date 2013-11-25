
/*
  Filename: timer.h
  Description: This file contains the declarations for the timer interrupt and
               it's initialization routine. Additionally, it provides a time
               base for  the complete software and a function to resynchronize
               to the node.
*/

#ifndef __TIMER_H__
# define __TIMER_H__

// clocking: 32768 Hz divisor: 1
// timebase: 30,517578125 us
# define NUM_MICROTICKS 0x7FFFu // 0x8000 == 32768

// scheduling defines
# define MICROTICK_RX_START       0x1FD6u // a bit more than 1,2 ms before TX START
# define MICROTICK_TX_START       0x2000u
# define MICROTICK_RX_TX_SYNC     0x6000u

# define MICROTICK_DSP_START      0x0A00u
# define MICROTICK_DSP_CYCLETIME  0x1000u

/*
precalculated scheduling

jeweils 1s

assumptions:
- no job needs more than ~62500 cycles (at 1 MHz)
- if not, higher cpu frequency necessary

                                    |             Open RX Sync Slot
            |                                     Communication
    |     |     |     |     |     |     |     |   Display Update
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
0    1000  2000  3000  4000  5000  6000  7000  8000

assumptions:
- no job needs more than ~62500 cycles (at 1 MHz)
- if not, higher cpu frequency necessary


alternative:

      |           |           |           |       Open RX Sync Slot
|           |           |           |             Communication
    |     |     |     |     |     |     |     |   Display Update
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
0    1000  2000  3000  4000  5000  6000  7000  8000



*/

void Timer_Init();
void Timer_Start(uint16_t position);
void Timer_Stop(); // needed?
void Timer_SetMode(uint8_t mode);
void Timer_CorrectSync(uint16_t expectedmicrotick);

void Timer_Delay(uint16_t ticks);

#endif /* __TIMER_H__ */
