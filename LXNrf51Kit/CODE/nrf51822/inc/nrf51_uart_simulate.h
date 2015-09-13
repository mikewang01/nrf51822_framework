#ifndef _NRF51_UART_SIMULATE_H_
#define _NRF51_UART_SIMULATE_H_

#include "nrf51.h"

#include <stdint.h>

/*
 * Internal receive buffer size for UART, user can modify this according to his/her own need
 */
#define UART_SIMULATE_RECV_BUF_SIZE   1


typedef struct
{
	uint32_t txd_pin_number;
	uint32_t rxd_pin_number;

	uint32_t baudrate;

}UARTSimulate_InitType;

/*
 * initialize UART
 */
int UARTSimulate_Init(UARTSimulate_InitType *UARTSimulate_InitStruct);

/*
 * send data via UART
 */
void UARTSimulate_Send(const uint8_t *buf, int size);

/*
 * read data in UART receive buffer
 * timeout is in millisecond
 * return bytes number actually read
 */
int UARTSimulate_Read(uint8_t *buf, int size, int timeout);

/*
 * clear bytes in receive fifo
 */
void UARTSimulate_Clear(void);

#endif	/* _NRF51_UART_SIMULATE_H_ */
