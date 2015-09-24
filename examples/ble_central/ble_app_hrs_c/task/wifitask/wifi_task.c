/***************************************************************
 * Name:      commtask.c
 * Purpose:   code for communication task
 * Author:    mikewang(s)
 * Created:   2015-09-24
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "stdio.h"
#include "stdint.h"
#include "event.h"
#include "task.h"
#include "message.h"
#include "hal_timer.h"
#include "comm_task.h"
#include "os.h"
#include "cling_comm_user.h"
/*********************************************************************
* MACROS
*/



/*********************************************************************
 * TYPEDEFS
 */



/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static CLASS(cling_comm_controller) *comm_controller_obj = NULL;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      Task_Led()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */

static void process_msg(uint16_t taskid , uint16_t msg_num , smessage_t* msg)
{

    do {
        switch(msg->type) {
            case IPC_MSG:
                if(((msg->event)&START_TICK) == START_TICK) {

                }
                break;
            case TIMER_MSG:
                if((msg->event & TIMEOUT_500MS) == TIMEOUT_500MS) {


                }
                break;
            case DEV_MSG:
                if((msg->event) == BAT_POWER_USB_PULLOUT) {
                    DEBUG("[%s]:[%s]:BAT_POWER_USB_PULLOUT recieved\r\n", __FILE__, __FUNCTION__);
                    //comm_controller_obj->load_device_info(comm_controller_obj);
                    comm_controller_obj->reboot(comm_controller_obj);
                } else if((msg->event) == CTRL_DEV_ERROR_PACKAGE) {
                    DEBUG("[%s]:[%s]:CTRL_DEV_ERROR_PACKAGE recieved\r\n", __FILE__, __FUNCTION__);

                } else if((msg->event) == CTRL_DEV_NORMAL_PACKAGE) {
                    DEBUG("[%s]:[%s]:CTRL_DEV_NORMAL_PACKAGE recieved lenth = %d\r\n", __FILE__, __FUNCTION__, msg->length);
#if 0
                    int i = 0;
                    for (i = 0; i < msg->length; i++) {
                        DEBUG("0x%02x ", msg->ptr[i]);
                    }
#endif

                } else if((msg->event) == CTRL_DEV_STREAM_PACKAGE) {
                    DEBUG("[%s]:[%s]:CTRL_DEV_STREAM_PACKAGE recieved\r\n", __FILE__, __FUNCTION__);
                }
                break;
        }
        msg = msg->next;
    } while((--msg_num) > 0);
}


/*********************************************************************
 * @fn      task_comm_ble_init()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */

void  task_comm_ble_init()
{
    uint8_t this_task_id = os_get_current_taskid();
    NEW(comm_controller_obj, cling_comm_controller);
    comm_controller_obj->register_task_id(comm_controller_obj, this_task_id);
    //send_message(this_task_id, SYSTEM_EVENT, IPC_MSG, START_TICK, NULL, (uint16_t)0);
}


/*********************************************************************
 * @fn      task_comunication_ble()
 *
 * @brief   This function invokes the initialization function for each task.
 *
 * @param   void
 *
 * @return  none
 */
void task_comm_ble()
{
    tcb_t* task_state = NULL;
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



