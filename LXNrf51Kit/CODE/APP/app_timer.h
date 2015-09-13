/*
 * Created by lxian1988@gmail.com.
 * 2014/8/15
 */

#ifndef _APP_TIMER_H_
#define _APP_TIMER_H_

#include "lx_nrf51Kit.h"

/* Hardware timeout interval in millisecond */
#define APP_TIMER_HARDWARE_INTERVAL		20

/* Number of timer instance can be created and running concurrently */
#define APP_TIMER_MAX_INSTANCE			8

#define APP_TIMER_HANDER_NULL			-1

typedef int APP_TimerHander;

/* WARNING: DO NOT can these functions in interrupt mode */

/*
 * Initialize timer, call once after program reset
 */
int APP_TimerInit(void);

/*
 * Create a timer instance, specify interval and message type
 */
int APP_TimerCreate(APP_TimerHander *hander, uint16_t interval, uint32_t msg_type);

/*
 * Destroy a timer instance
 */
void APP_TimerDestroy(APP_TimerHander *hander);

/*
 * Start a timer instance
 */
int APP_TimerStart(APP_TimerHander hander);

/*
 * Stop a timer instance
 */
int APP_TimerStop(APP_TimerHander hander);

/*
 * Create and start an singleshot timer
 */
int APP_TimerSingleShot(uint16_t interval, uint32_t msg_type);

/*
 * Retrieve all time elapsed (in millisecond) since Clip timer first start.
 * Time elapse begin to increase once the first timer instance has been 
 * started (not always the first one has been created) by calling APP_TimerStart. 
 * And if all timer instances has been stopped by calling APP_TimerStop or 
 * even destroyed by calling APP_TimerDestroy, the time elapsed will stop 
 * increase until another timer instance has been started.
 */

int APP_TimerGetTimeElapsed(void);

/*
 * Get created timer instance number
 */
int APP_TimerGetInstanceNumber(void);

/*
 * Get running timer instance number
 */
int APP_TimerGetRunningInstanceNumber(void);


#endif	/* _APP_TIMER_H_ */
