
/*
  Filename: com.c
  Description: This file contains basically the communication related code.
*/

#include "includes.h"
#include "event.h"
#include "main.h"
#include "timer.h"
#include "com.h"

uint8_t Com_States[64];
uint8_t CurrentSlot;

// TODO implement
void Com_Handler(EventMaskType ev)
{
  // next cycle
  if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START)) CurrentSlot = (CurrentSlot + 1) % 64;
  
  uint8_t currentcomstate = (Com_States[CurrentSlot] & COM_MODE_MASK);
  
  if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START))
  {
    ClearEvent(EVENT_COM_SLOT_START);
    
    Timer_SetMode(currentcomstate); // enable/disable my necessary interrupts
    
    if (currentcomstate == COM_MODE_TX)
    {
      // TODO: Prepare TX Data
    }
  }
  
  if (EVENT_COM_SLOT_RX_START == (ev & EVENT_COM_SLOT_RX_START))
  {
    ClearEvent(EVENT_COM_SLOT_RX_START);
    
    // TODO: Activate RX
    Timer_Delay(20000); // TODO recalculate!
    // TODO: Dispatch RX Event and react
  }
    
  if (EVENT_COM_SLOT_TX_START == (ev & EVENT_COM_SLOT_TX_START))
  {
    ClearEvent(EVENT_COM_SLOT_TX_START);
    
    // TODO: Activate Transmission
    // TODO: Wait for end of transmission? is this necessary?
  }
    
  if (EVENT_COM_SLOT_RX_TX_SYNC == (ev & EVENT_COM_SLOT_RX_TX_SYNC))
  {
    ClearEvent(EVENT_COM_SLOT_RX_TX_SYNC);
    
    // MAKE SYNC SHIT
  }
}

uint8_t Com_NetworkExists()
{
  // TODO: Implement;
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
