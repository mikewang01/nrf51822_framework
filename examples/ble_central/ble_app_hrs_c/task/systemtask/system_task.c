
/************************************************************
Copyright (C), 2008-2014, Colorful Sea Tech. Co., Ltd.
FileName:       manufacture_task.c
Author:      MikeWang   Version : 0.0          Date: 2014/8/18
Description:      s task to process the order from the host sofware
Version:  0.0
Function List:



task init function;
   void  task_manufacture_init()

process the message from other device ,thread ...;
   static void     process_msg(uint16_t taskid , uint16_t msg_num , smessage_t* msg);

process the detailed order distributed from host;
    static os_err_t process_order(data_packge package);

1. -------History:
<author>   <time>    <version >    <desc>
Mike      14/8/18      0.0       build this moudle
***********************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "stdio.h"
#include "stdint.h"
#include "event.h"
#include "task.h"
#include "message.h"
#include "hal_timer.h"
#include "cling_comm_user.h"
/*********************************************************************
* MACROS
*/
#define ABS(x)  ( (x)>0?(x):-(x) )

#define ABS_MINUS(x,y) ((x>y)?(x-y):(y-x))

#define  SYSTEM_RUNNING   1
#define  SYSTEM_POWER_OFF  0



/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 *  FUNCTIONS
 */

void  task_free(void);
void  task_free_init(void);
void  task_gui_init(void);
void  task_gui(void);
void  task_shell_init(void);
void  task_shell(void);
static void task_system_init(void);
static void task_system(void);
void  task_comm_ble_init(void);
void task_comm_ble(void);
/*********************************************************************
 * GLOBAL VARIABLES
 */
 
const task my_task_init[] = {
    task_system_init,
    task_comm_ble_init,
    task_free_init,
};
 
const task my_task[] = {

    task_system,
    task_comm_ble,
    task_free		     /*free time task*/
};



uint16_t task_num = (sizeof(my_task) / sizeof(my_task[0]));

/*********************************************************************
 * LOCAL VARIABLES
 */
static tcb_t *task_state = NULL;	//process control block

CLASS(event_hal_manager) *event_mgr_obj_p = NULL;
/*********************************************************************
 * EXTERNAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 *********************************************************************/


/*********************************************************************
 * @fn      void task_free_init()
 *
 * @brief   come to run when cpu is free
 *
 * @param   void
 *
 * @return  none
 */

void task_free_init()
{

    //send_message(3,SYSTEM_EVENT,IPC_MSG,START_TICK,NULL,(uint16_t)0);
}


/*********************************************************************
 * @fn      void task_free()
 *
 * @brief   come to run when cpu is free
 *
 * @param   void
 *
 * @return  none
 */
void task_free()
{
    uint8_t my_id = os_get_current_taskid();
    // LED4=1;
    //os_clock_close("ALL");
    //	 Sys_Standby(sleepmode);
    //os_clock_restore("ALL");

    //   LED4=!LED4;

    task_ticks_inc(my_id);
}


/*********************************************************************
 * @fn      void task_system_init()
 *
 * @brief   come to run when cpu is free
 *
 * @param   void
 *
 * @return  none
 */

    
static void task_system_init()
{
    uint8_t this_task_id = os_get_current_taskid();
    os_timer_period(this_task_id , TIMEOUT_SYSTEM_STATE_SWITCH , 1 , NULL);
    NEW(event_mgr_obj_p, event_hal_manager);
    event_mgr_obj_p->event_register(event_mgr_obj_p, 1, this_task_id);
}


/*********************************************************************
 * @fn      void task_system()
 *
 * @brief   come to run when cpu is free
 *
 * @param   void
 *
 * @return  none
 */
#if 1
static void process_msg(uint16_t taskid , uint16_t msg_num , smessage_t* msg);
#endif

static void task_system()
{

    uint8_t my_id = os_get_current_taskid() ;
    uint8_t msg_num = 0;
#if 1
    /*get self tcb_t to lead a further process */
    get_self_taskstate(my_id , task_state);

    // printf("taskid = %d" , My_Id );
    /*it actully get some message recieve and process it*/
    if(((task_state->task_state) & SYSTEM_EVENT) == SYSTEM_EVENT) {

        smessage_t* msg_get;
        /*get detailed message through tcb_t*/
        msg_num = get_message(task_state , &msg_get);

       // DEBUG("recive message SYSTEM_EVENT\r\n");

        /*process the message has been recieved*/
        process_msg(my_id, msg_num , msg_get);

    }

    /*delete messge recieved or memmory leakage gonna happen*/
    delete_message(my_id , SYSTEM_EVENT , msg_num);
#endif
    /*statistic the the oppupation of cpu*/
    task_ticks_inc(my_id);
}

/*********************************************************************
 * @fn      process_msg()
 *
 * @brief   process the ,message this task has received.
 *
 * @param   uint16_t    taskid  ;task_id of task calling this function
 *          uint16_t   msg_num  ;total message number recieved
 *          smessage_t* msg  ;recieved message ptr
 *
 * @return  none
 */
#if 1
static void process_msg(uint16_t taskid , uint16_t msg_num , smessage_t* msg)
{


   // DEBUG("message type =%d\r\n" , msg->type);

    do {

        switch(msg->type) {
                /* inter process communication message*/
            case IPC_MSG:

                break;

                /*timer related message*/
            case TIMER_MSG:
                if((msg->event) == TIMEOUT_SYSTEM_STATE_SWITCH) {
                   // DEBUG("HELLO\r\n");
                    //send_packet_list_demon(NULL);
                    //tx_buffer_process();
                    LEDS_INVERT(BSP_LED_1_MASK);
                }

                break;

                /*device related message*/
            case DEV_MSG:
                break;
            case EVENT_CENTER_MSG:
                LEDS_INVERT(BSP_LED_2_MASK);
            default:
                break;
        }

        /*get next message from message queue*/
        msg = msg->next;
    } while((--msg_num) > 0);
}
#endif
/*********************************************************************
 * @fn      board_hardware_initiate()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */

void board_hardware_initiate()
{

}
/*********************************************************************
 * @fn     void System_Tick_Hook()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */

void System_Tick_Hook()
{

}






