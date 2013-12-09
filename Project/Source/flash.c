
/*
  Filename: flash.c
  Description: This file contains the flash functionality.
*/

#include "includes.h"
#include "scheduler.h"
#include "flash.h"

// write data to flash
void Flash_Write(uint8_t* data, uint8_t len)
{
  if (len <= 0x7F)
  {
    DisableInterrupts();                      // 5xx workaround: disable global
                                              // interrupt
    uint8_t i;
    uint8_t* flashadr = (uint8_t*) FLASH_ADDRESS_SECTOR_C;
    FCTL3 = FWKEY;                            // Clear Lock bit
    FCTL1 = FWKEY+ERASE;                      // Set Erase bit
    *flashadr = 0;                            // Dummy write to erase Flash seg
    FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

    for (i = 0; i < len; i++)
    {
      flashadr[i] = data[i];                  // Write value to flash
    }
    
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
    EnableInterrupts();
  }
}

// read data from flash
void Flash_Read(uint8_t* data, uint8_t len)
{
  if (len <= 0x7F)
  {
    DisableInterrupts();                      // 5xx workaround: disable global
                                              // interrupt
    uint8_t i;
    uint8_t* flashadr = (uint8_t*) FLASH_ADDRESS_SECTOR_C;

    for (i = 0; i < len; i++)
    {
      data[i] = flashadr[i];                  // rear value from flash
    }
    
    EnableInterrupts();
  }
  // todo implement
}

void Flash_Random(uint8_t* data, unit8_t len)
{
    if (len <= 0x7F)
    {
      DisableInterrupts();                      // 5xx workaround: disable global
                                                // interrupt
      uint8_t i;
      uint8_t* flashadr = (uint8_t*) FLASH_ADDRESS_SECTOR_C;

      for (i = 0x80; i < len; i++)
      {
        data[i] = flashadr[i];                  // rear value from flash
      }

      EnableInterrupts();
    }
    // todo implement
}

// initialize flash module
void Flash_Init()
{
  // nothing to do in here
}

