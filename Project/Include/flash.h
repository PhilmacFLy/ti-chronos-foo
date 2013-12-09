
/*
  Filename: flash.h
  Description: This file contains the declarations and defines for the flash.
*/

#ifndef __FLASH_H__
# define __FLASH_H__

# define FLASH_ADDRESS_SECTOR_C  (0x1880);

void Flash_Write(uint8_t* data, uint8_t len);
void Flash_Read(uint8_t* data, uint8_t len);
void Flash_Random(uint8_t* data, unit8_t len);

void Flash_Init();

#endif /* __FLASH_H__ */
