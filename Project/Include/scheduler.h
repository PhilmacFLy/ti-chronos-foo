
/*
  Filename: scheduler.h
  Description: This file contains the declarations for the scheduler
               functionality. It's basically a one-task-OS
*/

#ifndef __SCHEDULER_H__
# define __SCHEDULER_H__

// defines for sleep/wakeup
// __no_operation necessary?
# define EnterSleep() __bis_SR_register(LPM3_bits); \
                      __no_operation()
# define LeaveSleep() __bic_SR_register_on_exit(LPM3_bits); \
                      __no_operation()

// defines for enabling/disabling interrupts
// __no_operation necessary?
# define EnableInterrupts() __bis_SR_register(GIE); \
                            __no_operation()
# define DisableInterrupts() __bic_SR_register(GIE); \
                             __no_operation()

void Scheduler_Init();

#endif /* __SCHEDULER_H__ */
