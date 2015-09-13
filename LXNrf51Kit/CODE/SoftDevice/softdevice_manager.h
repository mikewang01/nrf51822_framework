#ifndef _SOFTDEVICE_MANAGER_H_
#define _SOFTDEVICE_MANAGER_H_

#include <stdbool.h>

#include "nrf_sdm.h"
#include "ble.h"

int Softdevice_Init(nrf_clock_lfclksrc_t clock_source);

void Softdevice_Deinit(void);

bool Softdevice_IsEnabled(void);

#endif  /* _SOFTDEVICE_MANAGER_H_ */

