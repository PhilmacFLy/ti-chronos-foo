/*
  Filename: temperature.h
  Description: This file contains the declarations for the temperature initialization. 
	       Additionally, it provides functions for temperature retrival.
*/

#ifndef __TEMPERATURE_H__
# define __TEMPERATURE_H__

// Defines for Temperature either in Grad Farenheit or Grad Celcius
# define TEMP_C 0x0
# define TEMP_F 0x1

void Temperature_Init();
uint8_t Temperature_Get(uint8_t TEMP_ART);

#endif /* __TEMPERATURE_H__ */
 
