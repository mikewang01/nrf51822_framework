#include "app_button.h"

#include "lx_nrf51Kit.h"
#include "nrf51.h"
#include "app.h"
#include "debug.h"

#define APP_BUTTON_DEBOUNCE_TIME			10	/* milliseconds */
#define APP_BUTTON_DOUBLE_CLICKED_TIME		300	/* milliseconds */


typedef enum
{
	APP_BUTTON_STATUS_IDLE = 0,
	APP_BUTTON_STATUS_DEBOUNCE,
	APP_BUTTON_STATUS_CLICKED

}APP_ButtonStatus;

static APP_ButtonStatus status;
static int clickCount;

static void startDebounce(void);
static void stopDebounce(void);
static void startDetectDoubleClicked(void);
static void stopDetectDoubleClicked(void);

static void onGPIOTEEvent(void)
{
	// DEBUG_LOG("button!");

	switch (status)
	{
		case APP_BUTTON_STATUS_IDLE:
		{
			status = APP_BUTTON_STATUS_DEBOUNCE;
			
			clickCount = 0;
			
			startDebounce();
		}
		break;

		case APP_BUTTON_STATUS_DEBOUNCE:
		{
			
		}
		break;

		case APP_BUTTON_STATUS_CLICKED:
		{
			status = APP_BUTTON_STATUS_DEBOUNCE;

			stopDetectDoubleClicked();
			startDebounce();
		}
		break;
	}
}

static void onTIMER2Event(int channel)
{
	// DEBUG_LOG("ontime!");

	uint32_t btn = GPIO_ReadBit(BOARD_PIN_BUTTON);

	switch (status)
	{
		case APP_BUTTON_STATUS_DEBOUNCE:
		{
			stopDebounce();
			
			status = btn ? APP_BUTTON_STATUS_CLICKED : APP_BUTTON_STATUS_IDLE;
			// DEBUG_LOG("btn is: %d", btn);

			if (btn == 0)
			{
				if (++clickCount < 2)
				{
					status = APP_BUTTON_STATUS_CLICKED;
					
					startDetectDoubleClicked();
				}
				else
				{
					clickCount = 0;
					status = APP_BUTTON_STATUS_IDLE;

					LX_NotificationType msg = {0};
					msg.type = APP_MSG_BUTTON_DOUBLE_CLICKED;
					LX_NotificationCenterPut(&msg);
				}
			}
			else
			{
				status = APP_BUTTON_STATUS_IDLE;

				if (clickCount > 0)
				{
					LX_NotificationType msg = {0};
					msg.type = APP_MSG_BUTTON_CLICKED;
					LX_NotificationCenterPut(&msg);
				}
			}
		}
		break;

		case APP_BUTTON_STATUS_CLICKED:
		{
			stopDetectDoubleClicked();

			/*
			 * If code reach here, means no click happened in APP_BUTTON_DOUBLE_CLICKED_TIME.
			 */

			LX_NotificationType msg = {0};
			msg.type = (clickCount == 1) ? APP_MSG_BUTTON_CLICKED : APP_MSG_BUTTON_DOUBLE_CLICKED;
			LX_NotificationCenterPut(&msg);
			

			status = APP_BUTTON_STATUS_IDLE;
			clickCount = 0;
		}
		break;
	}
}

static void startDebounce(void)
{
	TIMER_InitType TIMER_InitStruct;

    TIMER_InitStruct.mode           = TIMER_MODE_MODE_Timer;
    TIMER_InitStruct.bitmode        = TIMER_BITMODE_BITMODE_16Bit;
    TIMER_InitStruct.prescaler      = 4;
    TIMER_InitStruct.cc[0]          = 1000 * 1000 * APP_BUTTON_DEBOUNCE_TIME / 1000;
    TIMER_InitStruct.cc[1]          = 0xFFFF;
    TIMER_InitStruct.cc[2]          = 0xFFFF;
    TIMER_InitStruct.cc[3]          = 0xFFFF;
    TIMER_InitStruct.cycle[0]		= 1;
    TIMER_InitStruct.cycle[1]		= 0;
    TIMER_InitStruct.cycle[2]		= 0;
    TIMER_InitStruct.cycle[3]		= 0;
    TIMER_InitStruct.event_handler  = onTIMER2Event;

    TIMER_Init(NRF_TIMER2, &TIMER_InitStruct);
    TIMER_Start(NRF_TIMER2);
}

static void stopDebounce(void)
{
	TIMER_Stop(NRF_TIMER2);
}

static void startDetectDoubleClicked(void)
{
	TIMER_InitType TIMER_InitStruct;

    TIMER_InitStruct.mode           = TIMER_MODE_MODE_Timer;
    TIMER_InitStruct.bitmode        = TIMER_BITMODE_BITMODE_16Bit;
    TIMER_InitStruct.prescaler      = 9;
    TIMER_InitStruct.cc[0]          = 62500 * APP_BUTTON_DOUBLE_CLICKED_TIME / 1000;
    TIMER_InitStruct.cc[1]          = 0xFFFF;
    TIMER_InitStruct.cc[2]          = 0xFFFF;
    TIMER_InitStruct.cc[3]          = 0xFFFF;
    TIMER_InitStruct.cycle[0]		= 1;
    TIMER_InitStruct.cycle[1]		= 0;
    TIMER_InitStruct.cycle[2]		= 0;
    TIMER_InitStruct.cycle[3]		= 0;
    TIMER_InitStruct.event_handler  = onTIMER2Event;

    TIMER_Init(NRF_TIMER2, &TIMER_InitStruct);
    TIMER_Start(NRF_TIMER2);
}

static void stopDetectDoubleClicked(void)
{
	TIMER_Stop(NRF_TIMER2);
}

void APP_ButtonInit(void)
{
	GPIOTE_InitType GPIOTE_InitStruct;

	GPIOTE_InitStruct.mode 			= GPIOTE_CONFIG_MODE_Event;
	GPIOTE_InitStruct.psel 			= BOARD_PIN_BUTTON;
	GPIOTE_InitStruct.polarity 		= GPIOTE_CONFIG_POLARITY_HiToLo;
	GPIOTE_InitStruct.outinit		= GPIOTE_CONFIG_OUTINIT_High;

	GPIOTE_Init(BOARD_GPIOTE_CHANNEL_BUTTON, &GPIOTE_InitStruct);
	GPIOTE_SetEventHandler(BOARD_GPIOTE_CHANNEL_BUTTON, onGPIOTEEvent);

	status 		= APP_BUTTON_STATUS_IDLE;
	clickCount 	= 0;
}


