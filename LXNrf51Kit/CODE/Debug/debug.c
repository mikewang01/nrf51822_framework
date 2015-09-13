#include "debug.h"


#if (DEBUG_LEVEL > DEBUG_LEVEL_NONE)


#if (DEBUG_MODE_UART || DEBUG_MODE_RADIO)

#include "nrf51.h"

#endif	/* (DEBUG_MODE_UART || DEBUG_MODE_RADIO) */


#if DEBUG_MODE_FLASH

#include "MX25L4006E.h"

#define FLASH_SECTOR_SIZE				MX25L4006E_SECTOR_SIZE

#define FLASH_PAGE_SIZE 				MX25L4006E_PAGE_SIZE

#define SECTOR_LOG_BEGIN				96UL
#define SECTOR_LOG_END 					128UL

#define ADDRESS_LOG_BEGIN				(FLASH_SECTOR_SIZE * SECTOR_LOG_BEGIN)
#define ADDRESS_LOG_END					(FLASH_SECTOR_SIZE * SECTOR_LOG_END)

#define FLASH_FIFO_SIZE					FLASH_PAGE_SIZE / 8

static uint32_t currentFLASHWriteAddress = ADDRESS_LOG_BEGIN;
static LX_FIFOType fifoFLASH;


#endif 	/* DEBUG_MODE_FLASH */


int debug_status = 0;


int fputc(int ch, FILE *f)
{
	uint8_t byte = ch;

#if DEBUG_MODE_UART
	UART_Send((const uint8_t *)&byte, sizeof(uint8_t));
#endif	/* DEBUG_MODE_UART */


#if DEBUG_MODE_RADIO	
	RADIO_TX(&byte, 1);
#endif	/* DEBUG_MODE_RADIO */


#if DEBUG_MODE_FLASH
	
	if (currentFLASHWriteAddress < ADDRESS_LOG_END)
	{
		if (LX_FIFOIsFull(fifoFLASH))
		{
			MX25L4006E_WakeUp();

			uint8_t buf[FLASH_FIFO_SIZE] = {0};

			LX_FIFORead(fifoFLASH, buf, sizeof(buf));

			MX25L4006E_Write(currentFLASHWriteAddress, buf, sizeof(buf), 300);
				
			currentFLASHWriteAddress += sizeof(buf);	
		}
	}

	LX_FIFOWrite(fifoFLASH, &byte, 1);

#endif	/* DEBUG_MODE_FLASH */


    return ch;
}

void Debug_Init()
{
	if (debug_status)
	{
		return;
	}

#if DEBUG_MODE_UART

	UART_InitType UART_InitStruct;
	
	UART_InitStruct.rts_pin_number 		= 8;									// pin 0.8
	UART_InitStruct.txd_pin_number 		= 9;									// pin 0.9
	UART_InitStruct.hwfc 				= UART_CONFIG_HWFC_Enabled;
	UART_InitStruct.cts_pin_number 		= 10;									// pin 0.10
	UART_InitStruct.rxd_pin_number 		= 11;									// pin 0.11
	UART_InitStruct.baudrate 			= UART_BAUDRATE_BAUDRATE_Baud230400;
	UART_InitStruct.parity 				= UART_CONFIG_PARITY_Excluded;			// no parity

	UART_Init(&UART_InitStruct);

#endif	/* DEBUG_MODE_UART */

	
#if DEBUG_MODE_RADIO

	/* Start 16 MHz crystal oscillator */
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;

	/* Wait for the external oscillator to start up */
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
	{
	}
	
	RADIO_Init();

#endif	/* DEBUG_MODE_RADIO */


#if DEBUG_MODE_FLASH

	currentFLASHWriteAddress = ADDRESS_LOG_END;

	MX25L4006E_Init();
	MX25L4006E_WakeUp();

	bool isFound = false;
	for (int i = SECTOR_LOG_BEGIN; i < SECTOR_LOG_END; i++)
	{
		if (isFound)
		{
			break;
		}

		uint8_t byte;
		uint32_t address = FLASH_SECTOR_SIZE * i;
		MX25L4006E_Read(address, &byte, sizeof(byte), 10);
		
		if (byte != 0xFF)
		{
			uint8_t buf[FLASH_PAGE_SIZE];
			for (uint32_t j = 0; j < FLASH_SECTOR_SIZE; j += sizeof(buf))
			{
				if (isFound)
				{
					break;
				}

				MX25L4006E_Read(address + j, buf, sizeof(buf), 10);

				for (int k = 0; k < sizeof(buf); k++)
				{
					if (buf[k] == 0xFF)
					{
						currentFLASHWriteAddress = address + j + k;
						isFound = true;
						break;	
					} 
				}
			}
		}
		else
		{
			if (address == ADDRESS_LOG_BEGIN)
			{
				currentFLASHWriteAddress = ADDRESS_LOG_BEGIN;
			}
		}
	}

	fifoFLASH = LX_FIFOCreate(FLASH_FIFO_SIZE);

#endif	/* DEBUG_MODE_FLASH */

}


#endif	/* (DEBUG_LEVEL > DEBUG_LEVEL_NONE) */
