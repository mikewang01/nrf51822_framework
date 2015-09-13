#include "nrf51_gpiote.h"

#include <stdint.h>
#include <string.h>

static GPIOTE_EventHandler event_handlers[4] = {0};

void GPIOTE_IRQHandler(void)
{
	for (int i = 0; i < 4; i++)
	{
		if (NRF_GPIOTE->EVENTS_IN[i] != 0)
		{
			NRF_GPIOTE->EVENTS_IN[i] = 0;

			if (event_handlers[i] != NULL)
			{
				event_handlers[i]();
			}
		}
	}
    
}

void GPIOTE_Deinit(uint32_t GPIOTE_Channel)
{
	NRF_GPIOTE->CONFIG[GPIOTE_Channel] = 0;

	NRF_GPIOTE->INTENCLR = (1 << GPIOTE_Channel);

	event_handlers[GPIOTE_Channel] = NULL;

	// NVIC_DisableIRQ(GPIOTE_IRQn);
}

void GPIOTE_Init(uint32_t GPIOTE_Channel, GPIOTE_InitType *GPIOTE_InitStruct)
{
	NRF_GPIOTE->CONFIG[GPIOTE_Channel] = (GPIOTE_InitStruct->mode << GPIOTE_CONFIG_MODE_Pos)
										| (GPIOTE_InitStruct->psel << GPIOTE_CONFIG_PSEL_Pos)
										| (GPIOTE_InitStruct->polarity << GPIOTE_CONFIG_POLARITY_Pos)
										| (GPIOTE_InitStruct->outinit << GPIOTE_CONFIG_OUTINIT_Pos);



	if (GPIOTE_InitStruct->mode == GPIOTE_CONFIG_MODE_Event)
	{
		NRF_GPIOTE->EVENTS_IN[GPIOTE_Channel] = 0;

		NRF_GPIOTE->INTENSET = (1 << GPIOTE_Channel);
    
	    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
	    NVIC_SetPriority(GPIOTE_IRQn, 3);
	    NVIC_EnableIRQ(GPIOTE_IRQn);
	}

}

void GPIOTE_TaskOut(uint32_t GPIOTE_Channel)
{
	NRF_GPIOTE->TASKS_OUT[GPIOTE_Channel] = 1;
}

void GPIOTE_SetEventHandler(uint32_t GPIOTE_Channel, GPIOTE_EventHandler cb)
{
	event_handlers[GPIOTE_Channel] = cb;
}
