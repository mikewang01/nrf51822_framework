/*
 * Create by lxian1988@gmail.com.com
 * 2014/7/15
 */

#ifndef _NRF51_UART_H_
#define _NRF51_UART_H_

#include <stdint.h>

#include "nrf51.h"
#include "debug.h"

/*
 * Internal receive buffer size for UART, user can modify this according to his/her own condition
 */
#define UART_RECV_BUF_SIZE   256


typedef struct
{
	uint32_t txd_pin_number;
	uint32_t rxd_pin_number;
	
	uint32_t hwfc;
	uint32_t rts_pin_number;
	uint32_t cts_pin_number;

	uint32_t baudrate;

	uint32_t parity;

}UART_InitType;

/*
 * initialize UART
 */
void UART_Init(UART_InitType *UART_InitStruct);

/*
 * reset UART
 */
void UART_Deinit(void);

/*
 * send data via UART
 */
void UART_Send(const uint8_t *buf, int size);

/*
 * read data in UART receive buffer
 * timeout is in millisecond
 * return bytes number actually read
 */
int UART_Read(uint8_t *buf, int size, int timeout);

/*
 * number of bytes to be read in read buffer
 */
int UART_ReadBufCount(void);

/*
 *	flush all data in RX buffer
 */
void UART_Clear(void);


#endif	/* _NRF51_UART_H_ */
