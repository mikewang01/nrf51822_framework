#include "nrf51_spi.h"

#include "debug.h"

void SPI_Init(NRF_SPI_Type *SPIx, SPI_InitType *SPI_InitStruct)
{
	/* Power on */
	SPIx->POWER = SPI_POWER_POWER_Enabled << SPI_POWER_POWER_Pos;

	uint32_t config_mode;

    /* Configure GPIO pins used for SCK, MOSI, MISO */
	GPIO_InitType GPIO_InitStruct;
	GPIO_InitStruct.dir			= GPIO_PIN_CNF_DIR_Output;
	GPIO_InitStruct.pull		= GPIO_PIN_CNF_PULL_Disabled;
	GPIO_InitStruct.drive		= GPIO_PIN_CNF_DRIVE_S0S1;
	GPIO_InitStruct.sense 		= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input       = GPIO_PIN_CNF_INPUT_Disconnect;

	GPIO_Init(SPI_InitStruct->sck_pin_number, &GPIO_InitStruct);
	GPIO_Init(SPI_InitStruct->mosi_pin_number, &GPIO_InitStruct);

	GPIO_InitStruct.dir			= GPIO_PIN_CNF_DIR_Input;
	GPIO_InitStruct.pull		= GPIO_PIN_CNF_PULL_Disabled;
	GPIO_InitStruct.drive		= GPIO_PIN_CNF_DRIVE_S0S1;
	GPIO_InitStruct.sense 		= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input       = GPIO_PIN_CNF_INPUT_Connect;

	GPIO_Init(SPI_InitStruct->miso_pin_number, &GPIO_InitStruct);


	SPIx->PSELSCK 		= SPI_InitStruct->sck_pin_number;
	SPIx->PSELMOSI 		= SPI_InitStruct->mosi_pin_number;
	SPIx->PSELMISO 		= SPI_InitStruct->miso_pin_number;
  
  	/* Config frequency */
  	SPIx->FREQUENCY 	= SPI_InitStruct->freq << SPI_FREQUENCY_FREQUENCY_Pos;

  	/* Config mode */
    switch (SPI_InitStruct->mode)
    {
        case SPI_MODE0:
            config_mode = (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
            break;
        case SPI_MODE1:
            config_mode = (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
            break;
        case SPI_MODE2:
            config_mode = (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
            break;
        case SPI_MODE3:
            config_mode = (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos) | (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
            break;
        default:
            config_mode = 0;
            break;
    }

    /* Config bit sequency */
    if (SPI_InitStruct->lsb_first)
    {
        SPIx->CONFIG = (config_mode | (SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos));
    }
    else
    {
        SPIx->CONFIG = (config_mode | (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos));
    }

    SPIx->EVENTS_READY = 0;
}

void SPI_Deinit(NRF_SPI_Type *SPIx)
{
	SPIx->POWER = SPI_POWER_POWER_Disabled << SPI_POWER_POWER_Pos;
}

int SPI_Transmit(NRF_SPI_Type *SPIx, uint8_t *buf, uint16_t size, int timeout, bool isRead)
{
	int retry;
	int len = 0;

	SPIx->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);

    for (int i = 0; i < size; i++)
    {
        SPIx->TXD = buf[i];

        /* Wait for the transaction complete or timeout (about 10ms - 20 ms) */
        retry = 1000 * timeout / 1;
        
        while (SPIx->EVENTS_READY == 0)
        {
        	if (retry-- == 0)
        	{
        		DEBUG_ERROR("SPI transmit timeout! timeout is: %dms", timeout);
        		return -1;
        	}

        	DelayUS(1);
        }
				
        /* clear the event to be ready to receive next messages */
		SPIx->EVENTS_READY = 0U;

        uint8_t rxd = (uint8_t)(SPIx->RXD);
        if (isRead)
        {
            buf[i] = rxd;
        }
	
	    len++;
    }

    /* disable slave (slave select active low) */
	SPIx->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);

	return len;
}
