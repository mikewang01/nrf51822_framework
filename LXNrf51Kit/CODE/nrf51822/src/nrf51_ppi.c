#include "nrf51_ppi.h"

void PPI_Init(uint32_t PPI_Channel, PPI_InitType *PPI_InitStruct)
{
	NRF_PPI->CH[PPI_Channel].EEP = (uint32_t)PPI_InitStruct->eep;
	NRF_PPI->CH[PPI_Channel].TEP = (uint32_t)PPI_InitStruct->tep;
}

void PPI_Enable(uint32_t PPI_Channel)
{
	NRF_PPI->CHENSET = 1 << PPI_Channel;
}

void PPI_Disable(uint32_t PPI_Channel)
{
	NRF_PPI->CHENCLR = 1 << PPI_Channel;
}
