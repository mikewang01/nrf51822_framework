#include "nrf51_uart.h"

#include "lx_nrf51Kit.h"

static LX_FIFOType fifo;

void UART0_IRQHandler(void)
{
    if (NRF_UART0->EVENTS_RXDRDY != 0)
    {
        // Clear UART RX event flag
        NRF_UART0->EVENTS_RXDRDY = 0;
        uint8_t byte = (uint8_t)NRF_UART0->RXD;

        if (LX_FIFOCount(fifo) < UART_RECV_BUF_SIZE)
        { 
            LX_FIFOWrite(fifo, &byte, 1);
        }
    }
}

void UART_Init(UART_InitType *UART_InitStruct)
{
    /* Power on */
    NRF_UART0->POWER = (UART_POWER_POWER_Enabled << UART_POWER_POWER_Pos);

    /* initialize TXD */
    NRF_UART0->PSELTXD = UART_InitStruct->txd_pin_number;

    /* initialize RXD */
    NRF_UART0->PSELRXD = UART_InitStruct->rxd_pin_number;

    /* initialize hardware flow control */
    if (UART_InitStruct->hwfc == UART_CONFIG_HWFC_Enabled)
    {
        /* initialize RTS */
        NRF_UART0->PSELRTS = UART_InitStruct->rts_pin_number;

        /* initialize CTS */
        NRF_UART0->PSELCTS = UART_InitStruct->cts_pin_number;
        
        /* enable hardware flow control */
        NRF_UART0->CONFIG &= ~(UART_InitStruct->hwfc << UART_CONFIG_HWFC_Pos);
        NRF_UART0->CONFIG |= (UART_InitStruct->hwfc << UART_CONFIG_HWFC_Pos);
    }

    /* set baudrate */
    NRF_UART0->BAUDRATE = (UART_InitStruct->baudrate << UART_BAUDRATE_BAUDRATE_Pos);
    
    /* set parity */
    NRF_UART0->CONFIG &= ~(UART_InitStruct->parity << UART_CONFIG_PARITY_Pos);
    NRF_UART0->CONFIG |= (UART_InitStruct->parity << UART_CONFIG_PARITY_Pos);
    

    NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->TASKS_STARTTX    = 1;
    NRF_UART0->TASKS_STARTRX    = 1;
    NRF_UART0->EVENTS_RXDRDY    = 0;

    /* Enable UART interrupt */
    NRF_UART0->INTENCLR = 0xFFFFFFFFUL; // clear all
    NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Set << UART_INTENSET_RXDRDY_Pos;

    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 3);
    NVIC_EnableIRQ(UART0_IRQn);

    fifo = LX_FIFOCreate(UART_RECV_BUF_SIZE);
}

void UART_Deinit(void)
{
    /* Power off */
    NRF_UART0->POWER = (UART_POWER_POWER_Disabled << UART_POWER_POWER_Pos);

    LX_FIFODestroy(&fifo);
}

void UART_Send(const uint8_t *buf, int size)
{
    for (int i = 0; i < size; i++)
    {
        NRF_UART0->TXD = buf[i];

        while (NRF_UART0->EVENTS_TXDRDY != 1)
        {
            /* do nothing */
        }
        NRF_UART0->EVENTS_TXDRDY = 0;
    }    
}

int UART_Read(uint8_t *buf, int size, int timeout)
{
    /* retry every 1ms */
    int retry = timeout / 1; 
    
    for (;;)
    {
        if (LX_FIFOCount(fifo) == 0)
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

    if (LX_FIFOCount(fifo) == 0)
    {
        return 0;
    }

    int readLen = LX_FIFORead(fifo, (uint8_t *)buf, size);

    return readLen;
}

void UART_Clear(void)
{
    LX_FIFOFlush(fifo);
}

