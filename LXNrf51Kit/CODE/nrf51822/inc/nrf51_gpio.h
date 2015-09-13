#ifndef _NRF51_GPIO_H_
#define _NRF51_GPIO_H_

#include "nrf51.h"

typedef struct
{
    /*
     * GPIO_PIN_CNF_DIR_Input
     * GPIO_PIN_CNF_DIR_Output
     */    
    uint32_t dir;

    /*
     * GPIO_PIN_CNF_PULL_Disabled
     * GPIO_PIN_CNF_PULL_Pulldown
     * GPIO_PIN_CNF_PULL_Pullup
     */
    uint32_t pull;

    /*
     * GPIO_PIN_CNF_DRIVE_S0S1
     * GPIO_PIN_CNF_DRIVE_H0S1
     * GPIO_PIN_CNF_DRIVE_S0H1
     * GPIO_PIN_CNF_DRIVE_H0H1
     * GPIO_PIN_CNF_DRIVE_D0S1
     * GPIO_PIN_CNF_DRIVE_D0H1
     * GPIO_PIN_CNF_DRIVE_S0D1
     * GPIO_PIN_CNF_DRIVE_H0D1
     */
    uint32_t drive;

    /*
     * GPIO_PIN_CNF_SENSE_Disabled
     * GPIO_PIN_CNF_SENSE_High
     * GPIO_PIN_CNF_SENSE_Low
     */
    uint32_t sense;

    /*
     * GPIO_PIN_CNF_INPUT_Connect
     * GPIO_PIN_CNF_INPUT_Disconnect
     */
    uint32_t input;

}GPIO_InitType;

void GPIO_Init(uint32_t pin_number, GPIO_InitType *GPIO_InitStruct);

void GPIO_QuickConfigInput(uint32_t pin_number);

void GPIO_QuickConfigOutput(uint32_t pin_number);

void GPIO_SetBit(uint32_t pin_number);

void GPIO_ResetBit(uint32_t pin_number);

void GPIO_ToggleBit(uint32_t pin_number);

uint32_t GPIO_ReadBit(uint32_t pin_number);




#endif  /* _NRF51_GPIO_H_ */
