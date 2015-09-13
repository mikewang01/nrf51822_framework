#ifndef _NRF51_RADIO_H_
#define _NRF51_RADIO_H_

#include "nrf51.h"

void RADIO_Init(void);

void RADIO_Deinit(void);

void RADIO_TX(const uint8_t *buf, int size);

int RADIO_RX(uint8_t *buf, int size, int timeout);

void RADIO_Clear(void);

#endif	/* _NRF51_RADIO_H_ */
