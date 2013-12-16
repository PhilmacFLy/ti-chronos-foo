
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
#include "transceiver.h"
#include "scheduler.h"
#include "com.h"

uint8_t Com_States[64];
uint8_t CurrentSlot;
uint8_t DistanceToMaster = 0;
uint8_t NumChildren = 0;
uint8_t LastSentData = 0;

uint8_t TxBuffer[64];
uint8_t TxCount;

uint8_t RxBuffer[64];

uint8_t mainstate = MAIN_STATE_INIT;
uint8_t cycles = 0;

void Com_Handler(EventMaskType ev)
{
  /*
  typedef void (*ComHandlerFktPtrType)(EventMaskType)
  const ComHandlerFktPtrType ComHandlerFktTable[4] = {
    Com_Handler_StartupListen,
    Com_Handler_StartupMaster,
    Com_Handler_StartupChild,
    Com_Handler_NormalCommunication
  };
  ComHandlerFktPtrType ComHandlerFktPtr = ComHandlerFktTable[mainstate];
  if (ComHandlerFktPtr != (ComHandlerFktPtrType)0)
  {
    ComHandlerFktPtr(ev);
  }
  */
  switch(mainstate)
  {
    case MAIN_STATE_INIT:
      Com_Handler_StartupListen(ev);
      break;
    case MAIN_STATE_INIT_MASTER:
      Com_Handler_StartupMaster(ev);
      break;       
    case MAIN_STATE_INIT_CHILD:
      Com_Handler_StartupChild(ev);
      break; 
    case MAIN_STATE_COM:
      Com_Handler_NormalCommunication(ev);
      break;
  }
}

// not yet finished
// handles all communication specific stuff
void Com_Handler_NormalCommunication(EventMaskType ev)
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
      if (MyID == 0) // master doesn't sends data out currently
      {
        for (i = 0; i < 16; i++) // max 16 values
        {
          uint8_t idx = LastSentData + i;
          if ( ((Com_States[idx] & NEWDATABIT_MASK) == NEWDATABIT_MASK)
            || ((Com_States[idx] & TIMEOUT_MASK) == TIMEOUT_VALUE) )
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
          }
        }
        LastSentData = (LastSentData + 16) % 64;
      }
      PrepareTransmit(TxBuffer, TxCount);
    }
  }
  
  if (EVENT_COM_SLOT_RX_START == (ev & EVENT_COM_SLOT_RX_START))
  {
    ClearEvent(EVENT_COM_SLOT_RX_START);
    
    if (currentcomstate == COM_MODE_PARENT_RX || currentcomstate == COM_MODE_CHILD_RX)
    {
      EventMaskType trcvev;
      ReceiveOn();
      Timer_Delay(25000); // TODO: recalculate! currently ~25 ms
      trcvev = GetEvent(EVENT_TRCV_RX_EVENT);
      ClearEvent(EVENT_TRCV_RX_EVENT);
      if (trcvev == EVENT_TRCV_RX_EVENT)
      {
        if (currentcomstate == COM_MODE_PARENT_RX)
        {
          // formula datalen -> transmission time
          // 4 Byte preamble, 2 Byte additional data
          // Anzahl Bits = ((x + 4) * 8)
          // Uebertragungszeit in s = ((x + 4) * 8) / 38383 s
          // 1 microtick ~ 8 us
          // Uebertragungszeit in microticks = ((x + 4) * 8) * 250000 / 38383
          // (x + 4) * 2000000 / 38383 ~ (x + 4) * 52106 / 1000
          uint8_t len = ReadRxData((uint8_t*)0); // no data necessary ATM
          uint16_t resyncoffset = ((len + 4) * 52106) / 1000;
          Timer_CorrectSync(MICROTICK_TX_START + 0x18 + resyncoffset); // TODO: recalculate 0x18 (code execution time)
          ReceiveOff(); // turn off TRCV
        }
        else
        {
          uint8_t len = ReadRxData(RxBuffer);
          uint8_t numdata = (len - 5) / 3; // 3 for control information + 1 for CRC control bit + 1 for RSSI
          
          ReceiveOff(); // turn off TRCV
          if ((RxBuffer[len - 1] & CRC_OK) == CRC_OK) // CRC OK?
          {
            // read data and sort in
            uint8_t i; uint8_t idx; uint16_t val; uint8_t cnt;
            
            for (i = 0; i < numdata; i++)
            {
              // unzip data
              idx = (RxBuffer[i * 3 + 3] >> 2);
              val = ((uint16_t) RxBuffer[i * 3 + 3]) << 8;
              val += ((uint16_t) RxBuffer[i * 3 + 4]);
              cnt = RxBuffer[i * 3 + 5];
              Data_SetValue(idx, val, cnt);       // save this value
              Com_FlagDataForSend(idx); // mark as new data, to send it
            }
          }
        }
      }
      else
      {
        // TODO: Timeout of message?
        ReceiveOff(); // turn off TRCV
      }
    }
  }
    
  if (EVENT_COM_SLOT_TX_START == (ev & EVENT_COM_SLOT_TX_START))
  {
    ClearEvent(EVENT_COM_SLOT_TX_START);
    
    if (currentcomstate == COM_MODE_TX)
    {
      StartTransmit();
      EnterSleep(); // Tx interrupt will wake up, event is not really necessary for this job
      ReceiveOff();
    }
  }
    
  if (EVENT_COM_SLOT_RX_TX_SYNC == (ev & EVENT_COM_SLOT_RX_TX_SYNC))
  {
    ClearEvent(EVENT_COM_SLOT_RX_TX_SYNC);
    
    // TODO: MAKE SYNC STUFF for new nodes
  }
}

void Com_Handler_StartupListen(EventMaskType ev)
{
  if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START))
  {
    ClearEvent(EVENT_COM_SLOT_START);
    cycles++;
  }
  if (MyID == 0)
  {
      mainstate = MAIN_STATE_INIT_CHILD;
  }
  else
  {
      mainstate = MAIN_STATE_INIT_MASTER;
  }
}

void Com_Handler_StartupMaster(EventMaskType ev)
{
  if ((EVENT_COM_SLOT_START & ev) == EVENT_COM_SLOT_START)
  {
    ClearEvent(EVENT_COM_SLOT_START);
    CurrentSlot = 0;
    Com_States[MyID] = COM_MODE_TX;
    Timer_Stop();
    Timer_Start(0); // startpoint not relevant because master
    Timer_SetMode(COM_MODE_TX);
    mainstate = MAIN_STATE_COM;
  }
}

void Com_Handler_StartupChild(EventMaskType ev)
{
  //TODO Implement
  mainstate = MAIN_STATE_COM;
}

// only for own temperature because of main does this job
void Com_FlagDataForSend(uint8_t index)
{
  if (index < 64) Com_States[index] |= NEWDATABIT_MASK;
}

void Com_Init()
{
  uint8_t i;
  for(i = 0; i < 64; i++) Com_States[i] = 0;
  Com_States[MyID] |= COM_MODE_TX;
  mainstate = MAIN_STATE_INIT;
}

void Com_Start(uint8_t slot)
{
  CurrentSlot = slot;
  // start timer?
}
