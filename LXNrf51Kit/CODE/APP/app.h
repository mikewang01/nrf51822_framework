#ifndef _APP_H_
#define _APP_H_

#include "lx_nrf51Kit.h"

enum APP_Msg_Type
{
	APP_MSG_TEST					= 0,

	/* 
	 * APP Timer messages 
	 */

	APP_MSG_TIMER_REPEAT_TIMEOUT,
	APP_MSG_TIMER_REPEAT_SINGLE_SHOT,

	/*
	 * Softdevice messages
	 */
	APP_MSG_SOFTDEVICE_EVENT,
	
	/* 
	 * APP Button messages 
	 */
	APP_MSG_BUTTON_CLICKED,
	APP_MSG_BUTTON_DOUBLE_CLICKED,
};

#include "nrf51.h"

/* 
 * definitions of hardware resource on device board
 */
/* definitions of pins */
// pins for button
#define BOARD_PIN_BUTTON											25

// pins for SPI
#define BOARD_PIN_SPI_SCK											6
#define BOARD_PIN_SPI_MOSI											5
#define BOARD_PIN_SPI_MISO											7

// pins for FLASH chip select
#define BOARD_PIN_FLASH_CS											12

// pins for I2C
#define BOARD_PIN_I2C_SCK											17
#define BOARD_PIN_I2C_SDA											18			

/* definitions of GPIOTE channel */
#define BOARD_GPIOTE_CHANNEL_BUTTON									GPIOTE_CHANNEL_0

int APP_Run(void);

#endif	/* _APP_H_ */
