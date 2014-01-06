
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
uint16_t cycles = 0;

static uint8_t best_id = 0xFF;

void Com_Handler(EventMaskType ev)
{
  /*
  typedef void (*ComHandlerFktPtrType)(EventMaskType)
  const ComHandlerFktPtrType ComHandlerFktTable[5] = {
    Com_Handler_StartupListen,       // MAIN_STATE_INIT
    Com_Handler_StartupMaster,       // MAIN_STATE_INIT_MASTER
    Com_Handler_StartupChild,        // MAIN_STATE_INIT_CHILD
    Com_Handler_NormalCommunication, // MAIN_STATE_COM
	  Com_Handler_SyncupChild          // MAIN_STATE_COM_SYNCUP
  };
  (ComHandlerFktTable[mainstate])(ev);
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
	  case MAIN_STATE_COM_SYNCUP:
	    Com_Handler_SyncupChild(ev);
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
      uint8_t TxCount = 3;
      TxBuffer[0] = MyID;
      TxBuffer[1] = DistanceToMaster;
      TxBuffer[2] = NumChildren;
      if (MyID != 0) // master doesn't sends data out currently
      {
        uint8_t numvals = 0; // counting variable for number of sent datas
        uint8_t i = 0; // helping var to only search over all values once
        uint8_t idx;
        uint16_t val;
        uint8_t cnt;
        while(i < 64 && numvals < 16)
        {
          idx = LastSentData;
          if ( ((Com_States[idx] & NEWDATABIT_MASK) == NEWDATABIT_MASK) || ((Com_States[idx] & TIMEOUT_MASK) == TIMEOUT_VALUE) )
          {
            val = Data_GetValue(idx);
            cnt = Data_GetCount(idx);
            // mask a little bit to need not so much bytes
            if (val != INVALID_VALUE)
            {
              // zipping of data
              TxBuffer[TxCount++] = (idx << 2) | ((uint8_t) (val >> 8));
              TxBuffer[TxCount++] = (uint8_t) (0xFF & val);
              TxBuffer[TxCount++] = cnt;
              // clear newdatabit mask and timeout counter by ANDing with the mode mask
              Com_States[idx] = Com_States[idx] & COM_MODE_MASK;
              numvals++;
            }
          }
          else
          {
            // increment timeout counter
            // yeah, probably improvable...
            Com_States[idx] = (Com_States[idx] & ~(TIMEOUT_MASK)) | ((Com_States[idx] + 1) & TIMEOUT_MASK);
          }
          
          i++;
          LastSentData = (LastSentData + 1) % 64; // this way, because it is possible to only iterate over <64 nodes (fairness)
        }
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
          uint16_t resyncoffset = (((uint16_t)len + 4) * 52106) / 1000;
          Timer_CorrectSync(MICROTICK_TX_START + 0x30 + resyncoffset); // TODO: recalculate 0x30 (code execution time)
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
              if (Data_GetCount(idx) == cnt) Com_FlagDataForSend(idx); // if new data, mark it for sending
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
    EventMaskType trcvev; // needed below
    uint8_t len;
    
    // clarification: only one new connection is accepted. if multiple nodes
    // want to sync up, the others won't get a ACK packet and have to retry it.
    ClearEvent(EVENT_COM_SLOT_RX_TX_SYNC);
    
    ReceiveOn(); // start RX
    Timer_Delay(30000); // wait 30 ms for the syncup package
    trcvev = GetEvent(EVENT_TRCV_RX_EVENT);
    ClearEvent(EVENT_TRCV_RX_EVENT);
    if (trcvev == EVENT_TRCV_RX_EVENT)
    {
      len = ReadRxData(RxBuffer);
      // TODO check rx data for correctness?
      ReceiveOff();
      
      // answer with sync ack
      // first byte: own id + 0xC0, second byte: partner id
      TxBuffer[0] = MyID |= 0xC0;
      TxBuffer[1] = RxBuffer[0] & 0x3F; // read partner id from rx buffer
      PrepareTransmit(TxBuffer, 2);
      StartTransmit();
      EnterSleep(); // necessary, because transceiver wakes up after transmission
      
      // made this way, so if a client sends multiple times and doesn't receives
      // the acks
      if (Com_States[RxBuffer[0] & 0x3F] == COM_MODE_IGNORE)
      {
        Com_States[RxBuffer[0] & 0x3F] |= COM_MODE_CHILD_RX;
        NumChildren++;
      }
    }
    ReceiveOff(); // probably better with else, but dunno because of more
                  // complicated if case
  }
}

void Com_Handler_StartupListen(EventMaskType ev)
{
  if (MyID == 0)
  {
    SetMainState(MAIN_STATE_INIT_MASTER);
	  return;
  }
  
  ReceiveOn();
  
  if (EVENT_COM_SLOT_START == (ev & EVENT_COM_SLOT_START))
  {
    ClearEvent(EVENT_COM_SLOT_START);
    cycles++;
	  if (cycles == 64)
	  {
	    ReceiveOff();
	    // TODO: wait for X seconds
	  }
  }
  
  if (EVENT_TRCV_RX_EVENT == (ev & EVENT_TRCV_RX_EVENT))
  {
    ClearEvent(EVENT_TRCV_RX_EVENT);
    ReceiveOff();
    SetMainState(MAIN_STATE_INIT_CHILD);
  }
}

void Com_Handler_StartupMaster(EventMaskType ev)
{
  if (EVENT_COM_SLOT_START == (EVENT_COM_SLOT_START & ev))
  {
    ClearEvent(EVENT_COM_SLOT_START);
    CurrentSlot = 0;
    Com_States[MyID] = COM_MODE_TX;
    Timer_Stop();
    Timer_Start(0); // startpoint not relevant because master
    Timer_SetMode(COM_MODE_TX);
    SetMainState(MAIN_STATE_COM);
  }
}

void Com_Handler_StartupChild(EventMaskType ev)
{
  // best_id is global var
  static uint8_t best_distance = 0xFF;
  static uint8_t best_numchilds = 0xFF;
  
  ReceiveOn();
  
  if (EVENT_COM_SLOT_START == (EVENT_COM_SLOT_START & ev))
  {
    ClearEvent(EVENT_COM_SLOT_START);
    cycles++;
	  CurrentSlot = (CurrentSlot + 1) % 64;
  }
  
  if (EVENT_TRCV_RX_EVENT == (ev & EVENT_TRCV_RX_EVENT))
  {
    uint8_t rx_id;
	  uint8_t rx_distance;
	  uint8_t rx_numchilds;
    uint8_t len = ReadRxData(RxBuffer);
    ClearEvent(EVENT_TRCV_RX_EVENT);
    rx_id = RxBuffer[0];
    rx_distance = RxBuffer[1];
    rx_numchilds = RxBuffer[2];
	
	if ((rx_distance < best_distance) && (rx_numchilds < best_numchilds))
	{
	  uint16_t resyncoffset;
	  best_id = rx_id;
    best_distance = rx_distance;
    best_numchilds = rx_numchilds;
		
    DistanceToMaster = best_distance + 1;
    
    // calculation from normal handler
    resyncoffset = (((uint16_t)len + 4) * 52106) / 1000;
    Timer_CorrectSync(MICROTICK_TX_START + 0x30 + resyncoffset); // TODO: recalculate 0x30 (code execution time)
	}
	
	if (CurrentSlot != rx_id) CurrentSlot = rx_id; // to sync the node number / cycle position
  }
  
  if (cycles == 256)
  {
    ReceiveOff();
	  SetMainState(MAIN_STATE_COM_SYNCUP);
  }
}

void Com_Handler_SyncupChild(EventMaskType ev)
{
  if (EVENT_COM_SLOT_START == (EVENT_COM_SLOT_START & ev))
  {
    ClearEvent(EVENT_COM_SLOT_START);
    
    CurrentSlot = (CurrentSlot + 1) % 64;
    if (CurrentSlot == best_id) Timer_SetMode(COM_MODE_MYSYNCSLOT);
    else Timer_SetMode(COM_MODE_IGNORE);
  }
  
  if (EVENT_COM_SLOT_RX_TX_SYNC == (ev & EVENT_COM_SLOT_RX_TX_SYNC))
  {
    EventMaskType trcvev; // needed below
    uint8_t len; // also needed below
    
    ClearEvent(EVENT_COM_SLOT_RX_TX_SYNC);
    
    // send sync message (mynode id |= 0x80) in the proper slot, but wait first a little bit
    // not the best solution, probably change later
    // if receiving ack after that, goto normal communication
    Timer_Delay(10000); // wait about 10 ms
    
    // transmit syncup package (first byte: myid + 0x80, second byte: partner id)
    TxBuffer[0] = MyID | 0x80;
    TxBuffer[1] = best_id;
    PrepareTransmit(TxBuffer, 2);
    StartTransmit();
    EnterSleep(); // wait for TX done
    
    ReceiveOn(); // TX done, start RX
    Timer_Delay(10000);// wait another 10 ms or for rx event
    
    trcvev = GetEvent(EVENT_TRCV_RX_EVENT);
    ClearEvent(EVENT_TRCV_RX_EVENT);
    if (trcvev == EVENT_TRCV_RX_EVENT)
    {
      len = ReadRxData(RxBuffer);
      // ack could be for another node, so check the complete message
      if (len == 2 && RxBuffer[0] == (best_id | 0xC0) && RxBuffer[1] == MyID)
      {
        Com_States[best_id] |= COM_MODE_PARENT_RX;
        SetMainState(MAIN_STATE_COM); // WOOHOO! we got it!
      }
    }
    ReceiveOff(); // in the if case, the SetMainState handles ReceiveOff!
  }
  
  SetMainState(MAIN_STATE_COM);
}

void Com_FlagDataForSend(uint8_t index)
{
  if (index < 64) Com_States[index] |= NEWDATABIT_MASK;
}

void Com_Init()
{
  uint8_t i;
  for(i = 0; i < 64; i++) Com_States[i] = 0;
  Com_States[MyID] |= COM_MODE_TX;
  SetMainState(MAIN_STATE_INIT);
}
