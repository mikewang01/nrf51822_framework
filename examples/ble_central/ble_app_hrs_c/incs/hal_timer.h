/***************************************************************
 * Name:      hal_timer.h
 * Purpose:   timer asbtract layer
 * Author:    mikewang(s)
 * Created:   2014-06-12
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */


#ifndef _HAL_TIMER_H
#define _HAL_TIMER_H
/* Includes ------------------------------------------------------------------*/

#include "stdint.h"
#include "message.h"
/* Exported constants --------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/
typedef struct timer
{
	uint8_t  type; //timer type like alarm,expired clock ...
	uint8_t  task_id;//the owner it belongs to 
	uint8_t  event;  // when time is up, the mcu gonna send this msg to corespond object
	uint32_t start_time;//the time when this counter established
	uint32_t destination_time;////the time when this counter ended
	uint32_t interval;//
	void (*triger_callback)(void);
	struct timer *next;

}stimer;


typedef  struct 
{
	uint8_t tasid;
	struct timer ptimer;//inherit the property from parent
	

}task_timer;

//local MACROS 
#define TIMER_EVENT uint16_t


/*timer types macros
x: taskid
y: message_type
z: detailed event
a: data to transfer
b: data legth
*/
#define timer_send_message(x, z ,a , b) \
				 send_message(x,SYSTEM_EVENT, TIMER_MSG, z ,a , b)
				



void timer_process(void);
int os_timer_period(uint8_t taskid ,TIMER_EVENT event, uint32_t  ticks_expired , void (*triger_callback)(void));
int os_timer_expired(uint8_t taskid ,TIMER_EVENT event, uint32_t  ticks_expired , void (*triger_callback)(void));
int del_timer_struct(uint8_t task_id,TIMER_EVENT event);
#endif

