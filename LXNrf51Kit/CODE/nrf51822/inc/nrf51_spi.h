#ifndef _NRF51_SPI_H_
#define _NRF51_SPI_H_

#include "nrf51.h"
#include "lx_nrf51Kit.h"


typedef enum
{
    //------------------------Clock polarity 0, Clock starts with level 0-------------------------------------------
    SPI_MODE0 = 0,          /*!< Sample data at rising edge of clock and shift serial data at falling edge */
    SPI_MODE1,              /*!< sample data at falling edge of clock and shift serial data at rising edge */
    //------------------------Clock polarity 1, Clock starts with level 1-------------------------------------------
    SPI_MODE2,              /*!< sample data at falling edge of clock and shift serial data at rising edge */
    SPI_MODE3               /*!< Sample data at rising edge of clock and shift serial data at falling edge */

}SPI_ModeType;

typedef struct
{
	/* PIN */
	uint32_t sck_pin_number;

	uint32_t miso_pin_number;
	
	uint32_t mosi_pin_number;

	/* frequency */
	// SPI_FREQUENCY_FREQUENCY_K125
	// SPI_FREQUENCY_FREQUENCY_K250
	// SPI_FREQUENCY_FREQUENCY_K500
	// SPI_FREQUENCY_FREQUENCY_M1
	// SPI_FREQUENCY_FREQUENCY_M2
	// SPI_FREQUENCY_FREQUENCY_M4
	// SPI_FREQUENCY_FREQUENCY_M8
	uint32_t freq;

	/* mode */
	SPI_ModeType mode;

	/* bit mode */
	bool lsb_first;

}SPI_InitType;

void SPI_Init(NRF_SPI_Type *SPIx, SPI_InitType *SPI_InitStruct);

void SPI_Deinit(NRF_SPI_Type *SPIx);

int SPI_Transmit(NRF_SPI_Type *SPIx, uint8_t *buf, uint16_t size, int timeout, bool isRead);

#endif	/* _NRF51_SPI_H_ */
