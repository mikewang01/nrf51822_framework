
#ifndef _NRF51_I2C_H
#define _NRF51_I2C_H

#include "nrf51.h"


typedef struct
{ 
    uint32_t sda_pin;

    uint32_t sck_pin;

}I2C_InitType;

void I2C_Init(I2C_InitType *I2C_InitStruct);

int I2C_ReadCommand(uint8_t slave_address, uint8_t *value, uint16_t len);

int I2C_WriteCommand(uint8_t slave_address, uint8_t command);

int I2C_Write(uint8_t slave_address, uint8_t reg, uint8_t *value, uint16_t len);

int I2C_Read(uint8_t slave_address, uint8_t reg, uint8_t *value, uint16_t len);

#endif
