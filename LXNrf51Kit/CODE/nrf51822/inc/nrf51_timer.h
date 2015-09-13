#ifndef _NRF51_TIMER_H_
#define _NRF51_TIMER_H_

#include <stdint.h>

#include "nrf51.h"

typedef void (*TIMER_EventHandler)(int channel);

typedef struct
{
	uint32_t mode;
	uint32_t bitmode;
	uint32_t prescaler;

	uint32_t cc[4];
	uint32_t cycle[4];

	TIMER_EventHandler event_handler;

}TIMER_InitType;


void TIMER_Init(NRF_TIMER_Type *TIMERx, TIMER_InitType *TIMER_InitStruct);

void TIMER_Start(NRF_TIMER_Type *TIMERx);

void TIMER_Stop(NRF_TIMER_Type *TIMERx);

void TIMER_Clear(NRF_TIMER_Type *TIMERx);

#endif	/* _NRF51_TIMER_H_ */
