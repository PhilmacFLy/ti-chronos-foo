#include "RF1A.h"
#include "hal_pmm.h"

/*******************
 * Function Definition
 */
void Radio_Init(void);
void PrepareTransmit(unsigned char *buffer, unsigned char length);
void StartTransmit(void);
void ReceiveOn(void);
void ReceiveOff(void);

void InitButtonLeds(void);
void InitRadio(void);
