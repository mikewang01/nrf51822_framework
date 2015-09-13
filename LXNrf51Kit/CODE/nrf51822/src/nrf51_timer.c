#include "nrf51_timer.h"

#include "debug.h"

static TIMER_EventHandler event_handlers[3];
void TIMER0_IRQHandler(void)
{
	// DEBUG_LOG("TIMER0!");

	for (int i = 0; i < 4; i++)
	{
		if (NRF_TIMER0->EVENTS_COMPARE[i] != 0)
		{
			NRF_TIMER0->EVENTS_COMPARE[i] = 0;

			if (event_handlers[0] != NULL)
			{
				event_handlers[0](i);
			}
		}
	}
}

void TIMER1_IRQHandler(void)
{
	// DEBUG_LOG("TIMER1!");

	for (int i = 0; i < 4; i++)
	{
		if (NRF_TIMER1->EVENTS_COMPARE[i] != 0)
		{
			NRF_TIMER1->EVENTS_COMPARE[i] = 0;

			if (event_handlers[1] != NULL)
			{
				event_handlers[1](i);
			}
		}
	}

}

void TIMER2_IRQHandler(void)
{
	// DEBUG_LOG("TIMER2!");

	for (int i = 0; i < 4; i++)
	{
		if (NRF_TIMER2->EVENTS_COMPARE[i] != 0)
		{
			NRF_TIMER2->EVENTS_COMPARE[i] = 0;

			if (event_handlers[2] != NULL)
			{
				event_handlers[2](i);
			}
		}
	}
}

void TIMER_Init(NRF_TIMER_Type *TIMERx, TIMER_InitType *TIMER_InitStruct)
{
	if (TIMERx == NRF_TIMER0)
	{
		TIMERx->TASKS_STOP 	= 1;
		TIMERx->TASKS_CLEAR = 1;

		TIMERx->MODE 		= TIMER_InitStruct->mode;
		TIMERx->BITMODE		= TIMER_InitStruct->bitmode;
		TIMERx->PRESCALER	= TIMER_InitStruct->prescaler;

		for (int i = 0; i < 4; i++)
		{
			TIMERx->CC[i] = TIMER_InitStruct->cc[i];
		}

		if (TIMER_InitStruct->cycle[0])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[1])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[2])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE2_CLEAR_Enabled << TIMER_SHORTS_COMPARE2_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[3])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Enabled << TIMER_SHORTS_COMPARE3_CLEAR_Pos;
		}

		/* Enable TIMER Interrupt */
		TIMERx->INTENCLR = 0xFFFFFFFFUL;
		TIMERx->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos)
							| (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos)
							| (TIMER_INTENSET_COMPARE2_Enabled << TIMER_INTENSET_COMPARE2_Pos)
							| (TIMER_INTENSET_COMPARE3_Enabled << TIMER_INTENSET_COMPARE3_Pos);

		NVIC_ClearPendingIRQ(TIMER0_IRQn);
		NVIC_SetPriority(TIMER0_IRQn, 3);
		NVIC_EnableIRQ(TIMER0_IRQn);

		event_handlers[0] = TIMER_InitStruct->event_handler;
	}
	else if (TIMERx == NRF_TIMER1)
	{
		TIMERx->TASKS_STOP 	= 1;
		TIMERx->TASKS_CLEAR = 1;

		TIMERx->MODE 		= TIMER_InitStruct->mode;
		TIMERx->BITMODE		= TIMER_InitStruct->bitmode;
		TIMERx->PRESCALER	= TIMER_InitStruct->prescaler;

		for (int i = 0; i < 4; i++)
		{
			TIMERx->CC[i] = TIMER_InitStruct->cc[i];
		}

		if (TIMER_InitStruct->cycle[0])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[1])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[2])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE2_CLEAR_Enabled << TIMER_SHORTS_COMPARE2_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[3])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Enabled << TIMER_SHORTS_COMPARE3_CLEAR_Pos;
		}

		/* Enable TIMER Interrupt */
		TIMERx->INTENCLR = 0xFFFFFFFFUL;
		TIMERx->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos)
							| (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos)
							| (TIMER_INTENSET_COMPARE2_Enabled << TIMER_INTENSET_COMPARE2_Pos)
							| (TIMER_INTENSET_COMPARE3_Enabled << TIMER_INTENSET_COMPARE3_Pos);

		NVIC_ClearPendingIRQ(TIMER1_IRQn);
		NVIC_SetPriority(TIMER1_IRQn, 3);
		NVIC_EnableIRQ(TIMER1_IRQn);

		event_handlers[1] = TIMER_InitStruct->event_handler;
	}
	else if (TIMERx == NRF_TIMER2)
	{
		TIMERx->TASKS_STOP 	= 1;
		TIMERx->TASKS_CLEAR = 1;

		TIMERx->MODE 		= TIMER_InitStruct->mode;
		TIMERx->BITMODE		= TIMER_InitStruct->bitmode;
		TIMERx->PRESCALER	= TIMER_InitStruct->prescaler;

		for (int i = 0; i < 4; i++)
		{
			TIMERx->CC[i] = TIMER_InitStruct->cc[i];
		}

		if (TIMER_InitStruct->cycle[0])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[1])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[2])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE2_CLEAR_Enabled << TIMER_SHORTS_COMPARE2_CLEAR_Pos;
		}
		else if (TIMER_InitStruct->cycle[3])
		{
			TIMERx->SHORTS = TIMER_SHORTS_COMPARE3_CLEAR_Enabled << TIMER_SHORTS_COMPARE3_CLEAR_Pos;
		}

		/* Enable TIMER Interrupt */
		TIMERx->INTENCLR = 0xFFFFFFFFUL;
		TIMERx->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos)
							| (TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos)
							| (TIMER_INTENSET_COMPARE2_Enabled << TIMER_INTENSET_COMPARE2_Pos)
							| (TIMER_INTENSET_COMPARE3_Enabled << TIMER_INTENSET_COMPARE3_Pos);

		NVIC_ClearPendingIRQ(TIMER2_IRQn);
		NVIC_SetPriority(TIMER2_IRQn, 3);
		NVIC_EnableIRQ(TIMER2_IRQn);

		event_handlers[2] = TIMER_InitStruct->event_handler;
	}
	else
	{
		DEBUG_LOG("error: TIMERx wrong!");
		return;
	}
}

void TIMER_Start(NRF_TIMER_Type *TIMERx)
{
	TIMERx->TASKS_START = 1;
}

void TIMER_Stop(NRF_TIMER_Type *TIMERx)
{
	TIMERx->TASKS_STOP = 1;
}

void TIMER_Clear(NRF_TIMER_Type *TIMERx)
{
	TIMERx->TASKS_CLEAR = 1;
}
