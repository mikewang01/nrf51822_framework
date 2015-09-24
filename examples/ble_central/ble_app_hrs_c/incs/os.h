
/***************************************************************
 * Name:      MEM.H
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-06-12
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/
#ifndef _OS_H_
#define _OS_H_
/*********************************************************************
 * INCLUDES
 */
#include "../lx_nrf51Kit.h"


/* Exported macros --------------------------------------------------------*/


/* Exported constants --------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/

typedef void (*task)();
typedef  unsigned char  os_err_t;

typedef struct {
    uint16_t task_state; /*record current messgage types sended to this task*/
    uint8_t  task_delay; /*task delay time*/
    uint32_t task_tick; /*record running times of current task to analize the occpupation of cpu */
    void *ptr;     /*personal data ptr for each task*/
} tcb_t;

/* Exported functions ------------------------------------------------------------*/
void task_ticks_inc(uint16_t taskid);
/*initiate each task before os start*/
void os_init(void);
/*shedule machine fo this os*/
void   os_shedule(void);
/*get task stae queue ptr*/
tcb_t* os_get_taskstate(void);
/*get cuurent system tick-tock count*/
uint32_t   os_get_tick(void);
/*show cuurent system software version*/
void   os_show_version(void);
/*get current task id*/
uint16_t os_get_current_taskid(void);
uint16_t os_get_task_max(void);
void  exam_assert(char * file_name, unsigned int line_no);

/*
*/

#endif

