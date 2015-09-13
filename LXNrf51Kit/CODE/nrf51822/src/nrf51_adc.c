#include "nrf51_adc.h"

#include "debug.h"


void ADC_Init(ADC_InitType *ADC_InitStruct)
{
	/* Power on */
	NRF_ADC->POWER = ADC_POWER_POWER_Enabled << ADC_POWER_POWER_Pos;

	/* Config */
	NRF_ADC->CONFIG = (ADC_InitStruct->res << ADC_CONFIG_RES_Pos)
						| (ADC_InitStruct->inpsel << ADC_CONFIG_INPSEL_Pos)
						| (ADC_InitStruct->refsel << ADC_CONFIG_REFSEL_Pos)
						| (ADC_InitStruct->psel << ADC_CONFIG_PSEL_Pos)
						| (ADC_InitStruct->extrefsel << ADC_CONFIG_EXTREFSEL_Pos);

	NRF_ADC->ENABLE = (ADC_ENABLE_ENABLE_Enabled << ADC_ENABLE_ENABLE_Pos);
}

void ADC_Deinit(void)
{
	NRF_ADC->POWER = ADC_POWER_POWER_Disabled << ADC_POWER_POWER_Pos;
}

int ADC_Read(void)
{
	int retry = 100;

	NRF_ADC->TASKS_START = 1;

	while (NRF_ADC->BUSY != (ADC_BUSY_BUSY_Ready << ADC_BUSY_BUSY_Pos))
	{
		if (retry-- == 0)
		{
			DEBUG_LOG("ADC timeout!");
			return -1;
		}

		DelayUS(1);
	}

	return NRF_ADC->RESULT & 0x3FF;
}
