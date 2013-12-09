
/*
  Filename: com.c
  Description: This file contains basically the communication related code.
*/

#include "includes.h"
#include "event.h"
#include "main.h"
#include "timer.h"
#include "data.h"
#include "temperature.h"
#include "com.h"

uint8_t Com_States[64];
uint8_t CurrentSlot;
uint8_t DistanceToMaster = 0;
uint8_t NumChildren = 0;
uint8_t LastSentData = 0;
uint8_t TxBuffer[64];
uint8_t TxCount;

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
      // Prepare TX Data for Send and give it to driver
      uint8_t i = 0;
      uint8_t TxCount = 3;
      TxBuffer[0] = MyID;
      TxBuffer[1] = DistanceToMaster;
      TxBuffer[2] = NumChildren;
      for (i = 0; i < 16; i++) // max 16 values
      {
        uint8_t idx = LastSentData + i;
        if ( ((Com_States[idx] & NEWDATABIT_MASK) == NEWDATABIT_MASK)
          || ((Com_States[idx] & TIMEOUT_MASK) == TIMEOUT_MASK) )
        {
          uint16_t val = Data_GetValue(LastSentData + i);
          uint8_t cnt = Data_GetCount(LastSentData + i);
          // mask a little bit to need not so much bytes
          if (val != INVALID_VALUE)
          {
            TxBuffer[TxCount++] = (idx << 2) | ((uint8_t) (val >> 8));
            TxBuffer[TxCount++] = (uint8_t) (0xFF & val);
            TxBuffer[TxCount++] = cnt;
            // clear newdatabit mask and timeout counter
            Com_States[idx] = Com_States[idx] & COM_MODE_MASK;
          }
        }
        else
        {
          // increment timeout counter
          // yeah, probably improvable...
          Com_States[idx] = (Com_States[idx] & ~(TIMEOUT_MASK)) | ((Com_States[idx] + 1) & TIMEOUT_MASK);
          // TODO: implement timeout functionality for this
        }
      }
      LastSentData = (LastSentData + 16) % 64;
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
