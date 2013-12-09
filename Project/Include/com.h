
/*
  Filename: com.h
  Description: This file contains the declarations for the communication.
*/

#ifndef __COM_H__
# define __COM_H__

#include "event.h"

/*
  8 Bit:
  0x01 : Timeout Bit 1
  0x02 : Timeout Bit 2
  0x04 : Timeout Bit 3
  0x08 : Timeout Bit 4
  0x10 : Mode Bit 1
  0x20 : Mode Bit 2
  0x40 : Mode Bit 3
  0x80 : New Data Bit
*/

# define COM_MODE_MASK        (0x70u)

# define COM_MODE_IGNORE        (0x10u) // this slot has no matter for me
# define COM_MODE_TX            (0x20u) // i am sending in this slot
# define COM_MODE_PARENT_RX     (0x30u) // my parent is sending in this slot
# define COM_MODE_CHILD_RX      (0x40u) // one of my childs is sending in this slot
# define COM_MODE_MYSYNCSLOT    (0x50u) // used for marking of the slot i want to sync to
# define COM_MODE_PARENT_RESYNC (0x60u) // used if synchronisation to parent is lost

# define TIMEOUT_MASK           (0x0Fu)

# define NEWDATABIT_MASK        (0x80u) // this bit is used to indicate the next-to-send stuff

void Com_Handler(EventMaskType ev);
void Com_Init();
void Com_FlagDataForSend(uint8_t index);
uint8_t Com_IsInitialized();
uint8_t Com_NetworkExists();

#endif /* __COM_H__ */
