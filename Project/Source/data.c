
/*
  Filename: data.c
  Description: This file contains the code for the data management.
*/

#include "includes.h"
#include "data.h"

static uint16_t data[64];
static uint8_t datacount[64];

// saves data in array and returns 1 if save is accepted
uint8_t Data_SetValue(uint8_t index, uint16_t value, uint8_t count)
{
  if (index < 64)
  {
    //if (datacount[index] < count || (datacount[index] > 220 && count < 30))
    {
      data[index] = value;
      datacount[index] = count;
      return 1;
    }
  }
  return 0;
}

// reads data from array
uint16_t Data_GetValue(uint8_t index)
{
  if (index >= 64) return INVALID_VALUE;
  return data[index];
}

uint8_t Data_GetCount(uint8_t index)
{
  if (index >= 64) return 0;
  return datacount[index];
}

// initializes data array
void Data_Init()
{
  uint8_t i;
  for (i = 0; i < 64; i++)
  {
    data[i] = INVALID_VALUE;
    datacount[i] = 0;
  }
}

