#include "app_timer.h"

#include "nrf51.h"
#include "lx_nrf51Kit.h"

typedef struct
{
	/* in millisecond */
	uint16_t interval;

	/* is single shot */
	bool isSingleShot;

	/* running status */
	bool isRunning;

	/* increase in HW TIMER Interrupt handler, and when this value
	 * is larger than interval, timeout happen.	
	 */
	uint16_t elapsedTime;

	/* timeout message type */
	uint32_t msg_type;	

}APP_TimerType;

/* Timer instances array */
static APP_TimerType* instances[APP_TIMER_MAX_INSTANCE];

/* Keep track of how much time elapsed since ClipTimer started */
static uint64_t m_TimeElapsed = 0;

/* Number of timer instances created */
static uint8_t m_InstanceNumber = 0;

/* Number of timer instances are running */
static uint8_t m_RunningInstanceNumber = 0;


static void onHWTimeout(int channel)
{
	m_TimeElapsed += APP_TIMER_HARDWARE_INTERVAL;

	for (int i = 0; i < APP_TIMER_MAX_INSTANCE; i++)
	{
		APP_TimerType *timer = instances[i];
		
		if (timer != NULL && timer->isRunning)
		{
			timer->elapsedTime += APP_TIMER_HARDWARE_INTERVAL;
			
			if (timer->elapsedTime >= timer->interval)
			{
				timer->elapsedTime -= timer->interval;

				LX_NotificationType msg = {0};
				msg.type = timer->msg_type;
				LX_NotificationCenterPut(&msg);

				if (timer->isSingleShot)
				{		
					APP_TimerHander hander = i;
					APP_TimerDestroy(&hander);
				}
			}
		}
	}
}

int APP_TimerInit(void)
{
	/*
	 * step 1: calculate Hardware TIMER interval
	 */

	/* interval = 2^prescaler * cc / 16M; */
	/* cc = 16M / 2^prescaler * interval */

	uint32_t prescaler;
	uint32_t cc;

	if (1 <= APP_TIMER_HARDWARE_INTERVAL && APP_TIMER_HARDWARE_INTERVAL <= 10)
	{
		prescaler = 4;
		cc = 1000 * 1000 * APP_TIMER_HARDWARE_INTERVAL / 1000;
	}
	else if (APP_TIMER_HARDWARE_INTERVAL <= 100)
	{
		prescaler = 5;
		cc = 500 * 1000 * APP_TIMER_HARDWARE_INTERVAL / 1000;
	}
	else if (APP_TIMER_HARDWARE_INTERVAL <= 1000)
	{
		prescaler = 8;
		cc = 62500 * APP_TIMER_HARDWARE_INTERVAL / 1000;
	}
	else if (APP_TIMER_HARDWARE_INTERVAL <= 2000)
	{
		prescaler = 9;
		cc = 31250 * APP_TIMER_HARDWARE_INTERVAL / 1000;
	}
	else 
	{
		return -1;
	}

	// DEBUG_LOG("APP_TIMER_HARDWARE_INTERVAL is: %d, prescaler is: %d, cc is: %d",
	// 	APP_TIMER_HARDWARE_INTERVAL, prescaler, cc);

	/* 
	 * step 2: Initilise Hardware TIMER
	 */
	TIMER_InitType TIMER_InitStruct;

    TIMER_InitStruct.mode           = TIMER_MODE_MODE_Timer;
    TIMER_InitStruct.bitmode        = TIMER_BITMODE_BITMODE_16Bit;
    TIMER_InitStruct.prescaler      = prescaler;
    TIMER_InitStruct.cc[0]          = cc;    
    TIMER_InitStruct.cc[1]          = 0xFFFF;
    TIMER_InitStruct.cc[2]          = 0xFFFF;
    TIMER_InitStruct.cc[3]          = 0xFFFF;
    TIMER_InitStruct.cycle[0]		= 1;
    TIMER_InitStruct.cycle[1]		= 0;
    TIMER_InitStruct.cycle[2]		= 0;
    TIMER_InitStruct.cycle[3]		= 0;
    TIMER_InitStruct.event_handler  = onHWTimeout;

    TIMER_Init(NRF_TIMER1, &TIMER_InitStruct);
	
	/*
	 * step 3: reset global values
	 */
    memset(instances, 0, sizeof(instances));

	m_TimeElapsed 			= 0;
	m_InstanceNumber 		= 0;
 	m_RunningInstanceNumber = 0;

	return 0;
}

int APP_TimerCreate(APP_TimerHander *hander, uint16_t interval, uint32_t msg_type)
{
	if (interval < APP_TIMER_HARDWARE_INTERVAL)
	{
		DEBUG_ERROR("interval < Hardware timeout interval!");
		return -1;
	}

	APP_TimerHander new_hander = APP_TIMER_HANDER_NULL;

	LX_ENTER_CRITICAL();

	for (int i = 0; i < APP_TIMER_MAX_INSTANCE; i++)
	{
		if (instances[i] == NULL)
		{
			new_hander = i;
			break;
		}
	} 

	if (new_hander == APP_TIMER_HANDER_NULL)
	{
		DEBUG_ERROR("too many instances!");

		LX_EXIT_CRITICAL();
		
		return -1;
	}

	APP_TimerType *new_timer;
	if (NULL == (new_timer = (APP_TimerType *) LX_Malloc(sizeof(APP_TimerType))))
	{
		DEBUG_ERROR("no memory!");

		LX_EXIT_CRITICAL();

		return -1;
	}

	new_timer->interval 		= interval;
	new_timer->isSingleShot 	= false;
	new_timer->isRunning 		= false;
	new_timer->elapsedTime  	= 0;
	new_timer->msg_type 		= msg_type;

	instances[new_hander] = new_timer;
	m_InstanceNumber++;

	*hander = new_hander;

	LX_EXIT_CRITICAL();

	return 0;
}

void APP_TimerDestroy(APP_TimerHander *hander)
{
	APP_TimerHander temp_hander = *hander;

	if (temp_hander == APP_TIMER_HANDER_NULL)
	{
		// DEBUG_ERROR("invalid hander!");

		return;
	}

	if (temp_hander > APP_TIMER_MAX_INSTANCE)
	{
		DEBUG_ERROR("invalid hander!");

		return;
	}

	LX_ENTER_CRITICAL();

	if (instances[temp_hander] == NULL)
	{
		DEBUG_ERROR("invalid hander!");

		LX_EXIT_CRITICAL();

		return;
	}

	APP_TimerStop(temp_hander);

	LX_Free(instances[temp_hander]);

	instances[temp_hander] = NULL;
	m_InstanceNumber--;

	*hander = APP_TIMER_HANDER_NULL;

	LX_EXIT_CRITICAL();
}

int APP_TimerStart(APP_TimerHander hander)
{
	if (hander == APP_TIMER_HANDER_NULL)
	{
		DEBUG_ERROR("invalid hander!");
		return -1;
	}

	if (hander > APP_TIMER_MAX_INSTANCE)
	{
		DEBUG_ERROR("invalid hander!");
		return -1;
	}

	LX_ENTER_CRITICAL();

	APP_TimerType *timer = instances[hander];

	if (timer == NULL)
	{
		DEBUG_ERROR("invalid hander!");

		LX_EXIT_CRITICAL();

		return -1;
	}

	if (timer->isRunning)
	{
		DEBUG_LOG("timer is already running!");
		return 0;
	}

	timer->isRunning 	= true;
	timer->elapsedTime 	= 0;

	m_RunningInstanceNumber++;
	if (m_RunningInstanceNumber == 1)
	{
		TIMER_Clear(NRF_TIMER1);
		TIMER_Start(NRF_TIMER1);
	}

	LX_EXIT_CRITICAL();

	return 0;
}

int APP_TimerStop(APP_TimerHander hander)
{
	if (hander == APP_TIMER_HANDER_NULL)
	{
		DEBUG_ERROR("invalid hander!");
		return -1;
	}

	if (hander > APP_TIMER_MAX_INSTANCE)
	{
		DEBUG_ERROR("invalid hander!");
		return -1;
	}

	LX_ENTER_CRITICAL();

	APP_TimerType *timer = instances[hander];

	if (timer == NULL)
	{
		DEBUG_ERROR("invalid hander!");

		LX_EXIT_CRITICAL();

		return -1;
	}

	if (!(timer->isRunning))
	{
		DEBUG_LOG("timer is already stopped!");
		return 0;
	}

	timer->isRunning = false;
	if (--m_RunningInstanceNumber == 0)
	{
		TIMER_Stop(NRF_TIMER1);
	}

	LX_EXIT_CRITICAL();

	return 0;
}

int APP_TimerSingleShot(uint16_t interval, uint32_t msg_type)
{
	APP_TimerHander hander;

	if (0 != APP_TimerCreate(&hander, interval, msg_type))
	{
		DEBUG_ERROR("Create single shot failed!");
		return -1;
	}

	instances[hander]->isSingleShot = true;

	if (0 != APP_TimerStart(hander))
	{
		DEBUG_ERROR("Start single shot failed!");
		return -1;
	}

	return 0;
}

int APP_TimerChangeInterval(APP_TimerHander hander, int interval)
{
	if (hander == APP_TIMER_HANDER_NULL)
	{
		DEBUG_ERROR("invalid hander!");
		return -1;
	}

	if (hander > APP_TIMER_MAX_INSTANCE)
	{
		DEBUG_ERROR("invalid hander!");
		return -1;
	}

	LX_ENTER_CRITICAL();

	APP_TimerType *timer = instances[hander];

	if (timer == NULL)
	{
		DEBUG_ERROR("invalid hander!");

		LX_EXIT_CRITICAL();

		return -1;
	}

	timer->interval = interval;

	LX_EXIT_CRITICAL();

	return 0;
}

int APP_TimerGetTimeElapsed(void)
{
	return m_TimeElapsed;
}

int APP_TimerGetInstanceNumber(void)
{
	return m_InstanceNumber;
}

int APP_TimerGetRunningInstanceNumber(void)
{
	return m_RunningInstanceNumber;
}
