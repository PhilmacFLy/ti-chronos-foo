
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

// sets all events which are set in "eventmask"
void SetEvent(EventMaskType eventmask)
{
  DisableInterrupts();
  eventmemory |= eventmask;
  EnableInterrupts();
}
