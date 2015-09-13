#ifndef _NRF51_PPI_H_
#define _NRF51_PPI_H_

#include <stdint.h>

#include "nrf51.h"

typedef struct
{
	uint32_t *eep;
	uint32_t *tep;
	
}PPI_InitType;

/*
 *	WARNING: PPI_Channel number is in range of 0 - 15
 */
void PPI_Init(uint32_t PPI_Channel, PPI_InitType *PPI_InitStruct);

void PPI_Enable(uint32_t PPI_Channel);

void PPI_Disable(uint32_t PPI_Channel);

#endif	/* _NRF51_PPI_H_ */
