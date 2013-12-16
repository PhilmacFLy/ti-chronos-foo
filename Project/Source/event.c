
/*
  Filename: event.c
  Description: This file contains the code and functions for the event API.
*/

#include "includes.h"
#include "scheduler.h"
#include "event.h"

#ifdef OPTIMIZE_EVENTS

volatile EventMaskType eventmemory = 0;

#else

static volatile EventMaskType eventmemory = 0;

#endif /* OPTIMIZE_EVENTS */

// gets all events which are set in "eventmask" and returns them
EventMaskType GetEvent(EventMaskType eventmask)
{
  DisableInterrupts();
  EventMaskType tmpevents = eventmask & eventmemory;
  EnableInterrupts();
  return tmpevents;
}

// gets all events
EventMaskType GetAllEvents()
{
  DisableInterrupts();
  EventMaskType tmpevents = eventmemory;
  EnableInterrupts();
  return tmpevents;
}

// clears all events which are set in "eventmask"
void ClearEvent(EventMaskType eventmask)
{
  DisableInterrupts();
  eventmemory &= (~eventmask);
  EnableInterrupts();
}

#ifndef OPTIMIZE_EVENTS

// sets all events which are set in "eventmask"
void SetEvent(EventMaskType eventmask)
{
  DisableInterrupts();
  eventmemory |= eventmask;
  EnableInterrupts();
}

#endif /* OPTMIZE_EVENTS */

