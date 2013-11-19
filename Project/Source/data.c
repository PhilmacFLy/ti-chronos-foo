
/*
  Filename: data.c
  Description: This file contains the code for the data management.
*/

#include "includes.h"
#include "data.h"

static uint16_t data[64];

// saves data in array
void Data_SetValue(uint8_t index, uint16_t value)
{
  if (index < 64)
  {
    data[index] = value;
  }
}

// reads data from array
uint16_t Data_GetValue(uint8_t index)
{
  if (index >= 64) return INVALID_VALUE;
  return data[index];
}

// initializes data array
void Data_Init()
{
  uint8_t i;
  for (i = 0; i < 64; i++) data[i] = INVALID_VALUE;
}

