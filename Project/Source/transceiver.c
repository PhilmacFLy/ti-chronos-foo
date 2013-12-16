/******************************************************************************
* CC430 RF Code Example - TX and RX (variable packet length =< FIFO size)
* 
* Each device will transmit a small packet every 2 sec.  
* 
* The RF packet engine settings specify variable-length-mode with CRC check 
* enabled. The RX packet also appends 2 status bytes regarding CRC check, RSSI 
* and LQI info. For specific register settings please refer to the comments for 
* each register in RfRegSettings.c, the CC430x613x User's Guide, and/or 
* SmartRF Studio.
* 
* * M. Morales/D. Dang
* Texas Instruments Inc.
* June 2010
* Built with IAR v4.21 and CCS v4.1
******************************************************************************/

#include "includes.h"
#include "com.h"
#include "transceiver.h"

#define  PACKET_LEN         (0x05)	    // PACKET_LEN <= 61
#define  RSSI_IDX           (PACKET_LEN+1)  // Index of appended RSSI 
#define  CRC_LQI_IDX        (PACKET_LEN+2)  // Index of appended LQI, checksum
#define  PATABLE_VAL        (0x51)          // 0 dBm output 

extern RF_SETTINGS rfSettings;

unsigned char packetReceived;
unsigned char packetTransmit; 

//unsigned char RxBuffer[64];
//unsigned char RxBufferLength = 0;
//const unsigned char TxBuffer[6]= {PACKET_LEN, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
unsigned char buttonPressed = 0;
unsigned int i = 0; 

uint8_t trcv_state;

void Radio_Init( void )
{  
  // Increase PMMCOREV level to 2 for proper radio operation
  SetVCore(2);                            
  
  ResetRadioCore(); 
  
  // Set the High-Power Mode Request Enable bit so LPM3 can be entered
  // with active radio enabled 
  PMMCTL0_H = 0xA5;
  PMMCTL0_L |= PMMHPMRE_L; 
  PMMCTL0_H = 0x00; 
  
  WriteRfSettings(&rfSettings);
  
  WriteSinglePATable(PATABLE_VAL);
  
  ReceiveOff(); // default to OFF
}


void PrepareTransmit(unsigned char *buffer, unsigned char length)
{
  ReceiveOff(); // just in case
  RF1AIES |= BIT9;                          
  RF1AIFG &= ~BIT9;                         // Clear pending interrupts
  RF1AIE |= BIT9;                           // Enable TX end-of-packet interrupt
  
  WriteBurstReg(RF_TXFIFOWR, buffer, length);     
}

void StartTransmit(void)
{
  Strobe( RF_STX );                         // Strobe STX

  trcv_state = TRCV_STATE_TX;  
}


void ReceiveOn(void)
{  
  RF1AIES |= BIT9;                          // Falling edge of RFIFG9
  RF1AIFG &= ~BIT9;                         // Clear a pending interrupt
  RF1AIE  |= BIT9;                          // Enable the interrupt 
  
  // Radio is in IDLE following a TX, so strobe SRX to enter Receive Mode
  Strobe( RF_SRX );
  
  trcv_state = TRCV_STATE_RX;
}

void ReceiveOff(void)
{
  RF1AIE &= ~BIT9;                          // Disable RX interrupts
  RF1AIFG &= ~BIT9;                         // Clear pending IFG

  // It is possible that ReceiveOff is called while radio is receiving a packet.
  // Therefore, it is necessary to flush the RX FIFO after issuing IDLE strobe 
  // such that the RXFIFO is empty prior to receiving a packet.
  Strobe( RF_SIDLE );
  Strobe( RF_SFRX  );
  
  trcv_state = TRCV_STATE_OFF;
}

// returns length, writes data to uint8_t * data
uint8_t ReadRxData(uint8_t* buffer)
{   
  uint8_t length = ReadSingleReg(RXBYTES);               
  if (buffer != (uint8_t*)0) ReadBurstReg(RF_RXFIFORD, buffer, length);
  return length;
}

#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR(void)
{
  volatile uint8_t val = RF1AIV;
  if (20 == val) // RFIFG9
  {
    // brackets necessary because of macro implementation of Com functions
    if (trcv_state == TRCV_STATE_RX)
    {
      Com_RxIndication();
    }
    else if (trcv_state == TRCV_STATE_TX)
    {
      Com_TxConfirmation();
    }
  }
  /*
  switch(__even_in_range(RF1AIV,32))        // Prioritizing Radio Core Interrupt 
  {
    case  0: break;                         // No RF core interrupt pending                                            
    case  2: break;                         // RFIFG0 
    case  4: break;                         // RFIFG1
    case  6: break;                         // RFIFG2
    case  8: break;                         // RFIFG3
    case 10: break;                         // RFIFG4
    case 12: break;                         // RFIFG5
    case 14: break;                         // RFIFG6          
    case 16: break;                         // RFIFG7
    case 18: break;                         // RFIFG8
    case 20:                                // RFIFG9
      // needs some rework
      if(trcv_state == TRCV_STATE_RX)			    // RX end of packet
      {
        // Read the length byte from the FIFO       
        //RxBufferLength = ReadSingleReg( RXBYTES );               
        //ReadBurstReg(RF_RXFIFORD, RxBuffer, RxBufferLength); 
        
        // Stop here to see contents of RxBuffer
        __no_operation(); 		   
        
        // Check the CRC results
        //if(RxBuffer[CRC_LQI_IDX] & CRC_OK)  
          //P1OUT ^= BIT0;                    // Toggle LED1      
      }
      else if(trcv_state == TRCV_STATE_TX)		    // TX end of packet
      {
        RF1AIE &= ~BIT9;                    // Disable TX end-of-packet interrupt
        P3OUT &= ~BIT6;                     // Turn off LED after Transmit 
        trcv_state = TRCV_STATE_OFF; // call receive off?
      }
      else while(1); 			    // trap 
      break;
    case 22: break;                         // RFIFG10
    case 24: break;                         // RFIFG11
    case 26: break;                         // RFIFG12
    case 28: break;                         // RFIFG13
    case 30: break;                         // RFIFG14
    case 32: break;                         // RFIFG15
  }  
  __bic_SR_register_on_exit(LPM3_bits);
  */
}
