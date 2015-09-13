#include "app.h"

#include "nrf51.h"
#include "lx_nrf51Kit.h"
#include "debug.h"

#include "app_timer.h"
#include "app_button.h"

/*
 * Timers
 */

static const int TIMER_INTERVAL_REPEAT			= 1000;
static APP_TimerHander m_TimerRepeat 				= APP_TIMER_HANDER_NULL;

static const int TIMER_INTERVAL_SINGLE_SHOT	= 30 * 1000;


static int init(void)
{
	/*
	 * step 1: init softdevice
	 */
	
   	// if (0 != Softdevice_Init(NRF_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION))
   	if (0 != Softdevice_Init(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM))
    {
        DEBUG_ERROR("error: softdevice_init failed!");
        return -1;
    }   
 	
 	/*
 	 * step 2: init button
 	 */
 	APP_ButtonInit();

 	/* 
 	 * step 3: create timers 
 	 * WARNING: This step must be done last or memory will be full of unhandled timer messages
 	 * before running event loop.
 	 */

	/* TIMER Interrupt happens every 20 millisecond */
	APP_TimerInit();	

	APP_TimerCreate(&m_TimerRepeat, TIMER_INTERVAL_REPEAT, APP_MSG_TIMER_REPEAT_TIMEOUT);
	APP_TimerStart(m_TimerRepeat);

	APP_TimerSingleShot(TIMER_INTERVAL_SINGLE_SHOT, APP_MSG_TIMER_REPEAT_SINGLE_SHOT);

	return 0;
}

static int reset(void)
{
	/* Flush log to FLASH */
	DEBUG_LOG("reseting ... ");

	NVIC_SystemReset();
	
	return 0;
}

static int handleSoftDeviceEvent(LX_NotificationType *msg)
{
	// DEBUG_LOG("handleSoftDeviceEvent!");

    bool no_more_soc_evts = false;

    bool no_more_ble_evts = false;

    for (;;)
    {
        uint32_t ret;

        if (!no_more_soc_evts)
        {
            // Pull event from SOC.
         
            uint32_t evt_id;
            ret = sd_evt_get(&evt_id);
            
            if (ret == NRF_ERROR_NOT_FOUND)
            {
                no_more_soc_evts = true;
            }
            else if (ret != NRF_SUCCESS)
            {
                return -1;
            }
            else 
            {
            	DEBUG_LOG("SOC event!");
            	/* TODO: handle SOC system event here */
            }
        }

        // Fetch BLE Events.
        if (!no_more_ble_evts)
        {
        	uint8_t evt_buf[64];
        	uint16_t evt_len = sizeof(evt_buf);

            // Pull event from stack
            ret = sd_ble_evt_get(evt_buf, &evt_len);
            if (ret == NRF_ERROR_NOT_FOUND)
            {
                no_more_ble_evts = true;
            }
            else if (ret != NRF_SUCCESS)
            {
                return -1;
            }
            else
            {
            	// DEBUG_LOG("BLE event!");
                // Call application's BLE stack event handler.
                // Bluetooth_DealBLEEvent((ble_evt_t *)evt_buf);
            }
        }

        if (no_more_soc_evts && no_more_ble_evts)
        {
            // There are no remaining System (SOC) events to be fetched from the SoftDevice.
            // Check if there are any remaining BLE events.
            break;
        }
    }

    return 0;
}

static int handleTimerRepeatTimeout(LX_NotificationType *msg)
{
    DEBUG_LOG("repeat! APP_TimerGetTimeElapsed() is: %d", APP_TimerGetTimeElapsed());
	LX_DateAddOneSecond();
	return 0;
}

static int handleTimerSingleShotTimeout(LX_NotificationType *msg)
{
	DEBUG_LOG("single shot! APP_TimerGetTimeElapsed() is: %d", APP_TimerGetTimeElapsed());
	LX_DateAddOneSecond();
	return 0;
}

static int handleButtonClicked(LX_NotificationType *msg)
{
	DEBUG_LOG("clicked!");
	return 0;
}

static int handleButtonDoubleClicked(LX_NotificationType *msg)
{
	DEBUG_LOG("double clicked!");
	return 0;
}

static int event_loop(void)
{
	while (LX_NotificationCenterCount() != 0)
	{
		// DEBUG_LOG("memory usage is: %d", LX_Memusage());
	
		LX_NotificationType msg;
		if (0 != LX_NotificationCenterGet(&msg))
		{
			DEBUG_ERROR("get msg failed!");
			return -1;
		}

		switch (msg.type)
		{
			case APP_MSG_SOFTDEVICE_EVENT: 
			{
				// DEBUG_LOG("CLIP_MSG_SOFTDEVICE_EVENT");
				if (0 != handleSoftDeviceEvent(&msg))
				{
					DEBUG_ERROR("handleSoftDeviceEvent failed!");
					return -1;
				}
			} 
			break;

			case APP_MSG_TIMER_REPEAT_TIMEOUT:
			{
				if (0 != handleTimerRepeatTimeout(&msg))
				{
					DEBUG_ERROR("handleTimerRepeatTimeout failed!");
					return -1;
				}
			}
			break;

			case APP_MSG_TIMER_REPEAT_SINGLE_SHOT:
			{
				if (0 != handleTimerSingleShotTimeout(&msg))
				{
					DEBUG_ERROR("handleTimerSingleShotTimeout failed!");
					return -1;
				}
			}
			break;

			case APP_MSG_BUTTON_CLICKED:
			{
				// DEBUG_LOG("CLIP_MSG_BUTTON_CLICKED!");
				if (0 != handleButtonClicked(&msg))
				{
					DEBUG_ERROR("handleButtonClicked failed!");
					return -1;
				}
			}
			break;

			case APP_MSG_BUTTON_DOUBLE_CLICKED:
			{
				// DEBUG_LOG("CLIP_MSG_BUTTON_DOUBLE_CLICKED!");
				if (0 != handleButtonDoubleClicked(&msg))
				{
					DEBUG_ERROR("handleButtonDoubleClicked failed!");
					return -1;
				}
			}
			break;

			default:
			{
				DEBUG_WARNING("find unknown message type: %d", msg.type);
			}
			break;
		}
	}

	return 0;
}

int APP_Run(void)
{
	if (0 != init())
	{
		DEBUG_ERROR("init failed!");
		return -1;
	}

	int error;
	for (;;)
	{
		if (LX_OK != (error = event_loop()))
		{
			DEBUG_ERROR("event_loop failed!");
			break;
		}

		/* Switch to a low power state until an event is available for the application */
	    if (NRF_SUCCESS != sd_app_evt_wait())
	    {
	    	DEBUG_ERROR("sd_app_evt_wait failed!");
	    	break;
	    }
	}
	
	DEBUG_ERROR("App failed with code: %d", error);

    reset();

	return 0;
}
