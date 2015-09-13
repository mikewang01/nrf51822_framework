#include "nrf51_uart_simulate.h"

#include <string.h>

#include "debug.h"

UARTSimulate_InitType cfg;
uint8_t delay;

static char fifo[UART_SIMULATE_RECV_BUF_SIZE];
static int fifoWriteIndex = 0;
static int fifoReadIndex = 0;

#define fifo_count() 	(fifoWriteIndex - fifoReadIndex)

void onRXStart(void)
{
	uint32_t rxd = cfg.rxd_pin_number;

	DelayUS(delay);

	uint8_t byte = 0;
	for (int i = 0; i < 8; i++)
	{
		byte >>= 1;
		if (GPIO_ReadBit(rxd))
		{
			byte |= 0x80;
		}

		DelayUS(delay);
	}
	while(!GPIO_ReadBit(rxd));

	if (fifoWriteIndex < UART_SIMULATE_RECV_BUF_SIZE)
    {
        fifo[fifoWriteIndex++] = byte;
    }

	NRF_GPIOTE->EVENTS_IN[GPIOTE_CHANNEL_0] = 0;
	NVIC_ClearPendingIRQ(GPIOTE_IRQn);
}


int UARTSimulate_Init(UARTSimulate_InitType *UARTSimulate_InitStruct)
{
	memcpy(&cfg, UARTSimulate_InitStruct, sizeof(UARTSimulate_InitType));

	/* initialize TXD */
    GPIO_InitType GPIO_InitStruct;

	GPIO_InitStruct.dir 	= GPIO_PIN_CNF_DIR_Output;
	GPIO_InitStruct.pull 	= GPIO_PIN_CNF_PULL_Disabled;
	GPIO_InitStruct.drive 	= GPIO_PIN_CNF_DRIVE_S0S1;
	GPIO_InitStruct.sense	= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input 	= GPIO_PIN_CNF_INPUT_Disconnect;

	GPIO_Init(cfg.txd_pin_number, &GPIO_InitStruct);

	GPIO_SetBit(cfg.txd_pin_number);

    /* initialize RXD */
	GPIO_InitStruct.dir 	= GPIO_PIN_CNF_DIR_Input;
	GPIO_InitStruct.pull 	= GPIO_PIN_CNF_PULL_Disabled;
	GPIO_InitStruct.drive 	= GPIO_PIN_CNF_DRIVE_S0S1;
	GPIO_InitStruct.sense	= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input 	= GPIO_PIN_CNF_INPUT_Connect;

	GPIO_Init(cfg.rxd_pin_number, &GPIO_InitStruct);

	GPIOTE_InitType GPIOTE_InitStruct;

	GPIOTE_InitStruct.mode		= GPIOTE_CONFIG_MODE_Event;
	GPIOTE_InitStruct.psel 		= cfg.rxd_pin_number;
	GPIOTE_InitStruct.polarity  = GPIOTE_CONFIG_POLARITY_HiToLo;
	GPIOTE_InitStruct.outinit   = GPIOTE_CONFIG_OUTINIT_High;	// no effect

	GPIOTE_Init(GPIOTE_CHANNEL_0, &GPIOTE_InitStruct);

	GPIOTE_SetEventHandler(GPIOTE_CHANNEL_0, onRXStart);

	/* set baudrate */
	delay = 1000 * 1000 / cfg.baudrate;
	
	return 0;
}

void UARTSimulate_Send(const uint8_t *buf, int size)
{
	uint32_t txd = cfg.txd_pin_number;

	for (int i = 0; i < size; i++)
	{
		uint8_t byte = buf[i];

		GPIO_ResetBit(txd);
		DelayUS(delay);
		for (int j = 0; j < 8; j++)
		{
			if (byte & 1)
			{
				GPIO_SetBit(txd);
			}
			else
			{
				GPIO_ResetBit(txd);
			}
			byte >>= 1;
			DelayUS(delay);
		}

		GPIO_SetBit(txd);
		DelayUS(delay);
	}
}

int UARTSimulate_Read(uint8_t *buf, int size, int timeout)
{
	/* retry every 1ms */
    int retry = timeout / 1; 
    

    for (;;)
    {
        if (fifo_count() < size)
        {
            /* return cause timeout */
            if (retry-- <= 0)
            {
                break;
            }

            DelayMS(1);
            continue;
        }

        break;
    }

    if (fifo_count() == 0)
    {
        return 0;
    }

    int readLen = fifo_count() < size ? fifo_count() : size;
	memcpy((void *)buf, (void *)(fifo + fifoReadIndex), sizeof(uint8_t) * readLen);
	fifoReadIndex += readLen;

	if (0 == fifo_count())
	{
		fifoWriteIndex = 0;
		fifoReadIndex = 0;	
	}

    return readLen;
}

void UARTSimulate_Clear(void)
{
	fifoWriteIndex = 0;
	fifoReadIndex = 0;
}
