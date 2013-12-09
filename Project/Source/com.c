
/*
  Filename: com.c
  Description: This file contains basically the communication related code.
*/

#include "includes.h"
#include "event.h"
#include "main.h"
#include "timer.h"
#include "temperature.h"
#include "com.h"

uint8_t Com_States[64];
uint8_t CurrentSlot;

// not yet finished
// handles all communication specific stuff
void Com_Handler(EventMaskType ev)
{
  // next cycle, so dispatch this here
  if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START)) CurrentSlot = (CurrentSlot + 1) % 64;
  
  uint8_t currentcomstate = (Com_States[CurrentSlot] & COM_MODE_MASK);
  
  if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START))
  {
    ClearEvent(EVENT_COM_SLOT_START);
    
    Timer_SetMode(currentcomstate); // enable/disable my necessary interrupts
    
    if (currentcomstate == COM_MODE_TX)
    {
      // TODO: Prepare TX Data for Send and give it to driver
      // TODO: or increment timeout counter
    }
  }
  
  if (EVENT_COM_SLOT_RX_START == (ev & EVENT_COM_SLOT_RX_START))
  {
    ClearEvent(EVENT_COM_SLOT_RX_START);
    
    if (currentcomstate == COM_MODE_PARENT_RX || currentcomstate == COM_MODE_CHILD_RX)
    {
      // TODO: Activate RX
      Timer_Delay(20000); // TODO: recalculate! currently ~20 ms
      // TODO: wait for RX event / timeout and disable RX mode
      if (currentcomstate == COM_MODE_PARENT_RX)
      {
        // TODO: Resynchronisation, because parent sent us data
      }
      // TODO: read data and sort in
      // else TODO: dispatch RX timeout event (timer returns normally)
    }
  }
    
  if (EVENT_COM_SLOT_TX_START == (ev & EVENT_COM_SLOT_TX_START))
  {
    ClearEvent(EVENT_COM_SLOT_TX_START);
    
    if (currentcomstate == COM_MODE_TX)
    {
      // TODO: Activate Transmission
      // TODO: Wait for end of transmission? is this necessary?
    }
  }
    
  if (EVENT_COM_SLOT_RX_TX_SYNC == (ev & EVENT_COM_SLOT_RX_TX_SYNC))
  {
    ClearEvent(EVENT_COM_SLOT_RX_TX_SYNC);
    
    // MAKE SYNC STUFF
  }
}

// only for own temperature because of main does this job
void Com_FlagDataForSend(uint8_t index)
{
  if (index < 64) Com_States[index] |= NEWDATABIT_MASK;
}

uint8_t Com_NetworkExists()
{
  // TODO: Implement;
  return 0;
}

uint8_t Com_IsInitialized()
{
  // TODO: Implement
  return 0;
}

void Com_Init()
{
  uint8_t i;
  for(i = 0; i < 64; i++) Com_States[i] = 0;
  Com_States[MyID] |= COM_MODE_TX;
}

void Com_Start(uint8_t slot)
{
  CurrentSlot = slot;
  // start timer?
}
