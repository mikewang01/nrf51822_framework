#ifndef _NRF51_ADC_H_
#define _NRF51_ADC_H_ value

#include "lx_nrf51Kit.h"
#include "nrf51.h"

typedef struct
{
	/*
	 * ADC_CONFIG_RES_8bit
	 * ADC_CONFIG_RES_9bit
	 * ADC_CONFIG_RES_10bit
	 */
	uint32_t res;

	/*
	 * ADC_CONFIG_INPSEL_AnalogInputNoPrescaling
     * ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling
     * ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling
     * ADC_CONFIG_INPSEL_SupplyTwoThirdsPrescaling
     * ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling
     */
	uint32_t inpsel;

	/*
	 * ADC_CONFIG_REFSEL_VBG 
     * ADC_CONFIG_REFSEL_External 
     * ADC_CONFIG_REFSEL_SupplyOneHalfPrescaling
     * ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling
	 */
	
	uint32_t refsel;

	/*
	 * ADC_CONFIG_PSEL_AnalogInput0
	 * ADC_CONFIG_PSEL_AnalogInput1
	 * ADC_CONFIG_PSEL_AnalogInput2
	 * ADC_CONFIG_PSEL_AnalogInput3
	 * ADC_CONFIG_PSEL_AnalogInput4
	 * ADC_CONFIG_PSEL_AnalogInput5
	 * ADC_CONFIG_PSEL_AnalogInput6
	 * ADC_CONFIG_PSEL_AnalogInput7
	*/
	uint32_t psel;

	/* 
	 * ADC_CONFIG_EXTREFSEL_None
	 * ADC_CONFIG_EXTREFSEL_AnalogReference0
	 * ADC_CONFIG_EXTREFSEL_AnalogReference1
	 */
	uint32_t extrefsel;

}ADC_InitType;

void ADC_Init(ADC_InitType *ADC_InitStruct);

int ADC_Read(void);

#endif	/* _NRF51_ADC_H_ */
