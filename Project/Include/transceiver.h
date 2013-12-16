#ifndef __TRANSCEIVER_H__
# define __TRANSCEIVER_H__

#include "RF1A.h"
#include "hal_pmm.h"

#define TRCV_STATE_OFF    (0x00u)
#define TRCV_STATE_RX     (0x01u)
#define TRCV_STATE_TX     (0x02u)

/*******************
 * Function Definition
 */
void Radio_Init(void);
void PrepareTransmit(unsigned char *buffer, unsigned char length);
void StartTransmit(void);
void ReceiveOn(void);
void ReceiveOff(void);
uint8_t ReadRxData(uint8_t* buffer);

void InitButtonLeds(void);
void InitRadio(void);

#endif /* __TRANSCEIVER_H__ */