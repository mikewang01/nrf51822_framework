/************************************************************
Copyright (C), 2008-2014, Colorful Sea Tech. Co., Ltd.
FileName:       os.c
Author:      MikeWang   Version : 0.0          Date: 2014/8/18
Description:     core of this spmle system aimed at sheduling initializing tasks
Function List:

initiate each task before os start
void os_init(void);
shedule machine fo this os
void   os_shedule(void);
get task stae queue ptr
tcb_t* os_get_taskstate(void);
get cuurent system tick-tock count
uint32_t   os_get_tick(void);
show cuurent system software version
void   os_show_version(void);
get current task id
uint16_t os_get_current_taskid(void);

1. -------History:
<author>   <time>    <version >    <desc>
Mike      12/12/18      0.0       build this moudle
MIKE      14/10/14      0.1       modify this file according to standrd principle
***********************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "../lx_nrf51Kit.h"
#include "stdio.h"
#include "stdint.h"
#include "os.h"
#include "hal_timer.h"
#include "version.h"
/*********************************************************************
* MACROS
*/

/*increase individual clock count by task_id*/


/*********************************************************************
 * TYPEDEFS
 */



/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */

/*system clock tick count*/
static uint32_t tick_tok = 0;
/*task state record queue*/
static tcb_t *task_state = NULL;	//process control block
static  uint8_t current_task = 0;
/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern const task my_task[];
extern const task my_task_init[];
extern       uint16_t task_num;
/*********************************************************************
 * FUNCTIONS
 */
static void systick_process(void *p);
/*********************************************************************
 * @fn      os_err_t rt_device_set_tx_complete
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none
 *
 *
 *   @return the error code, success on initialization successfully.
 */

const char *const corp_logo =
    " \
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n \
        @~~~~~~~~~~~~~~~~#~~~~~~~~#~~~#~~~~~~~~@\r\n \
        @~##########~~~~#######~~~~#~~#######~~@\r\n \
        @~~~~~#~~~~~~~~#~~~~~~#~~~~~~#~~~~~~~~~@\r\n \
        @~~~~~#~~~~~~~#~~~~~~#~~~~~~#~######~~~@\r\n \
        @~~~~~#~~~~~~~~#########~~#~~~#~~~~#~~~@\r\n \
        @~~########~~~~#~~~#~~~#~~~#~~#~#~~#~~~@\r\n \
        @~~~~#~~~~#~~~~#~~~#~~~#~~~~#########~~@\r\n \
        @~~~~#~~~~#~~~~#########~~~~~#~~~~~#~~~@\r\n \
        @~~~~#~~~~#~~~~#~~~~~~~~~~~~~#~~#~~#~~~@\r\n \
        @~~~#~~~~~#~~~~#~~~~~~~~#~~#~########~~@\r\n \
        @~~~#~~~~~#~~~~#~~~~~~~~#~#~~~~~~~~#~~~@\r\n \
        @~###########~~~#########~~~~~~~~##~~~~@\r\n \
        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
void os_show_version(void)
{
#if 0
    printf("\r\n%s", corp_logo);
    printf("\r\n\\ | /\r\n");
    printf("-WSH-    WSH Operating System\r\n");
    printf("/ | \\     %d.%d.%d build %s\r\n",
           VER_MAJOR, VER_MINOR, REVISION, __DATE__);
    printf(" 20012 - 2013 Copyright by MikeWang\r\n");
#endif
}

/*********************************************************************
* @fn      os_init()
*
* @brief   This function  init all functions invoked in my_task_tnit and establish
                    task_state structor for each peocess.
*
* @param   void
*
* @return  none
*/
void os_init()
{
    uint8_t count;
    /*register system tick function*/
    REGISTER_TIMER_PROCESS(100, systick_process);
    task_state = (tcb_t*)LX_Malloc(sizeof(tcb_t) * task_num);	//

    for(count = 0 ; count < task_num ; count++) {
        current_task = count;
        task_state[count].ptr = NULL;
        my_task_init[count]();
    }

}





/*********************************************************************
 * @fn      os_get_taskstate
 *
 * @brief   delay Task and Suspend itself
 *
 * @param   Task_Id
 *
 * @return  none
 */
tcb_t* os_get_taskstate()
{
    return task_state;
}

/*********************************************************************
 * @fn      os_get_tick()
 *
 * @brief   get current tick count
 *
 * @param
 *
 * @return  current_tick count
 */
uint32_t os_get_tick()
{
    return tick_tok;
}
/*********************************************************************
 * @fn      os_get_task_max()
 *
 * @brief   return max task number
 *
 * @param
 *
 * @return  current_tick count
 */
uint16_t os_get_task_max()
{
    return task_num;
}

/*********************************************************************
 * @fn      task_ticks_inc()
 *
 * @brief   inc task tick timr
 *
 * @param
 *
 * @return  current_tick count
 */
void task_ticks_inc(uint16_t taskid)
{
    (os_get_taskstate())[taskid].task_tick++;
}

/*********************************************************************
 * @fn      os_get_current_taskid()
 *
 * @brief   get current tick count
 *
 * @param
 *
 * @return  current_tick count
 */
uint16_t os_get_current_taskid()
{
    return current_task;
}

/*********************************************************************
 * @fn      os_shedule()
 *
 * @brief   a simple shedule machine
 *
 * @param
 *
 * @return  current_tick count
 */

void os_shedule()
{
    uint8_t idx = 0;

    while(1) {

        do {
            /* Task is highest priority that is ready.*/
            if(task_state[idx].task_state) {
                break;
            }
        } while(++idx < task_num);

        /*make sure that a task is ready if no ,no function gonna be excecuted*/
        if(idx < task_num) {
            current_task = idx;
            my_task[idx]();
            idx = 0;
            continue;
        }else{
            break;
        }
        /*clear task index or result gonna be a unkown error*/
        idx = 0;
    }


}



static void systick_process(void *p)
{

    p = p;
    // uint8_t count;
#ifdef System_Hook
    system_tick_hook();
#endif
    tick_tok++;
    timer_process();

}

/*********************************************************************
 * @fn      exam_assert()
 *
 * @brief   check
 *
 * @param
 *
 * @return  current_tick count
 */
void  exam_assert(char * file_name, unsigned int line_no)
{
    printf("\n[EXAM]Assert failed: %s, line %u\n",
           file_name, line_no);
    abort();
}


