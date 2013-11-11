
/*
  Filename: timer.c
  Description: This file contains the code and functions for the scheduler.
               Additionally, the functions to resync are also there.
*/

#include "includes.h"
#include "scheduler.h"

static EventMaskType eventmemory = 0;

// gets all events which are set in "eventmask"
EventMaskType GetEvent(EventMaskType eventmask)
{
  return eventmask & eventmemory;
}

// clears all events which are set in "eventmask"
void ClearEvent(EventMaskType eventmask)
{
  eventmemory &= (~eventmask);
}

// sets all events which are set in "eventmask"
void SetEvent(EventMaskType eventmask)
{
  eventmemory |= eventmask;
}
