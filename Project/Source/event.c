
/*
  Filename: event.c
  Description: This file contains the code and functions for the event API.
*/

#include "includes.h"
#include "scheduler.h"
#include "event.h"

static volatile EventMaskType eventmemory = 0;

// gets all events which are set in "eventmask" and returns them
EventMaskType GetEvent(EventMaskType eventmask)
{
#ifdef INTERRUPT_LOCK_EVENT_API
  DisableInterrupts();
#endif
  EventMaskType tmpevents = eventmask & eventmemory;
#ifdef INTERRUPT_LOCK_EVENT_API
  EnableInterrupts();
#endif
  return tmpevents;
}

// clears all events which are set in "eventmask"
void ClearEvent(EventMaskType eventmask)
{
#ifdef INTERRUPT_LOCK_EVENT_API
  DisableInterrupts();
#endif
  eventmemory &= (~eventmask);
#ifdef INTERRUPT_LOCK_EVENT_API
  EnableInterrupts();
#endif
}

// sets all events which are set in "eventmask"
void SetEvent(EventMaskType eventmask)
{
#ifdef INTERRUPT_LOCK_EVENT_API
  DisableInterrupts();
#endif
  eventmemory |= eventmask;
#ifdef INTERRUPT_LOCK_EVENT_API
  EnableInterrupts();
#endif
}
