#include "nrf51_gpio.h"

void GPIO_Init(uint32_t pin_number, GPIO_InitType *GPIO_InitStruct)
{
	NRF_GPIO->PIN_CNF[pin_number] = (GPIO_InitStruct->dir << GPIO_PIN_CNF_DIR_Pos)
									| (GPIO_InitStruct->pull << GPIO_PIN_CNF_PULL_Pos)
									| (GPIO_InitStruct->drive << GPIO_PIN_CNF_DRIVE_Pos)
									| (GPIO_InitStruct->sense << GPIO_PIN_CNF_SENSE_Pos)
									| (GPIO_InitStruct->input << GPIO_PIN_CNF_INPUT_Pos);
}

void GPIO_QuickConfigInput(uint32_t pin_number)
{
	GPIO_InitType GPIO_InitStruct;

	GPIO_InitStruct.dir 	= GPIO_PIN_CNF_DIR_Input;
	GPIO_InitStruct.pull 	= GPIO_PIN_CNF_PULL_Disabled;
	GPIO_InitStruct.drive 	= GPIO_PIN_CNF_DRIVE_S0S1;
	GPIO_InitStruct.sense 	= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input 	= GPIO_PIN_CNF_INPUT_Connect;

	GPIO_Init(pin_number, &GPIO_InitStruct);
}

void GPIO_QuickConfigOutput(uint32_t pin_number)
{
	GPIO_InitType GPIO_InitStruct;

	GPIO_InitStruct.dir 	= GPIO_PIN_CNF_DIR_Output;
	GPIO_InitStruct.pull 	= GPIO_PIN_CNF_PULL_Disabled;
	GPIO_InitStruct.drive 	= GPIO_PIN_CNF_DRIVE_S0S1;
	GPIO_InitStruct.sense 	= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input 	= GPIO_PIN_CNF_INPUT_Disconnect;

	GPIO_Init(pin_number, &GPIO_InitStruct);
}

void GPIO_SetBit(uint32_t pin_number)
{
	NRF_GPIO->OUTSET = (1 << pin_number);
}

void GPIO_ResetBit(uint32_t pin_number)
{
	NRF_GPIO->OUTCLR = (1 << pin_number);
}

uint32_t GPIO_ReadBit(uint32_t pin_number)
{
	return ((NRF_GPIO->IN >> pin_number) & 1);
}

void GPIO_ToggleBit(uint32_t pin_number)
{
	NRF_GPIO->OUT ^= (1 << pin_number);
}
