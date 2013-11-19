
/*
  Filename: data.h
  Description: This file contains the declarations for the data management unit.
*/

#ifndef __DATA_H__
# define __DATA_H__

# define INVALID_VALUE (0xFFFFu)

void Data_Init();
void Data_SetValue(uint8_t index, uint16_t value);
uint16_t Data_GetValue(uint8_t index);

#endif /* __DATA_H__ */
