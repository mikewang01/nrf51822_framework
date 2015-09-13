/***************************************************************
 * Name:      event_hal.c
 * Purpose:   a event abstract layer that connect os event layer and ble lyaer
 * Author:    mikewang(s)
 * Created:   2014-05-15
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "../lx_nrf51Kit.h"
#include "hal_timer.h"
#include "stdint.h"
#include "stdio.h"
#include "hal_event.h"

/*********************************************************************
* MACROS
*/

static inline int event_hal_msg_send_message(uint8_t task_id, uint16_t event, void *data, uint16_t length)
{
    return send_message(task_id, SYSTEM_EVENT, EVENT_CENTER_MSG, event , data , length);
}


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */

static struct	base_object *event_hal_list_header = NULL;
static CLASS(event_hal_manager) *this =  NULL;
/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      os_event_resister
 *
 * @brief   bond a task id with a specific ble event  used for higher lyaer
 *
 * @param   uint8_t taskid registered task id  ,uint16_t event bonded event
 *
 * @return  lx_ok sucess
 */
int os_event_resister(CLASS(event_hal_manager) *arg, uint16_t taskid , uint16_t event)
{
    ASSERT(arg != NULL);
    char name[OS_NAME_MAX];
    sprintf(name, "%d",  event);
    struct event_hal_object *t = NULL;

    /*if this event has been registered*/
    if(arg->base_object_op_p->find(arg->base_object_op_p, event_hal_list_header, (struct base_object**)(&t), name) == LX_OK) {
        t->spec_event = event;
        /*mark taskid*/
        t->resistered_taskid_list.tmp |= (1 << taskid);
        return LX_OK;
    }

    t = (struct event_hal_object*)LX_Malloc(sizeof(struct event_hal_object));
    ASSERT(t != NULL);
    memset(t, 0 , sizeof(struct event_hal_object));
    /*add one object to chain list*/
    arg->base_object_op_p->add(arg->base_object_op_p, &event_hal_list_header, (struct base_object*)(t), OS_Object_Class_Mgr, name);
    return LX_OK;
}

/*********************************************************************
 * @fn      os_event_put
 *
 * @brief   used to put a event intto event center for lower layer
 *
 * @param   uint8_t taskid registered task id  ,uint16_t event bonded event
 *
 * @return  lx_ok sucess
 */
int os_event_put(CLASS(event_hal_manager) *arg, uint16_t event, void *data, uint16_t lenth)
{
    ASSERT(arg != NULL);
    char name[OS_NAME_MAX];
    sprintf(name, "%d",  event);
    struct event_hal_object *t = NULL;

    /*if this event has been registered*/
    if(arg->base_object_op_p->find(arg->base_object_op_p, event_hal_list_header, (struct base_object**)(&t), name) == LX_OK) {
        int i = 0;

        /*find out registered tassid*/
        for(i = 0; i < sizeof(t->resistered_taskid_list.tmp); i++) {
            /*fidnd out the task id resistered*/
            if(t->resistered_taskid_list.tmp | (1 << i)) {
                /*send data to speccific task id*/
                event_hal_msg_send_message(i, t->spec_event, data, lenth);
            }
        }

        return LX_OK;
    }

    return LX_ERROR;
}

/******************************************************************************
 * FunctionName : int deinit_event_hal_manager(CLASS(event_hal_manager) *arg)
 * Description  :  event_hal_manager object desconstruction method
 * Parameters   : level : output level of pin
 * Returns      : 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int deinit_event_hal_manager(CLASS(event_hal_manager) *arg) /*initiate http object*/
{
    ASSERT(arg != NULL);
    LX_Free(arg);
    return LX_OK;
}


/******************************************************************************
 * FunctionName : int  init_event_hal_manager(void* env)
 * Description  : base object construction method
	* Parameters   : arg: object pointer
 * Returns      : 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int init_event_hal_manager(CLASS(event_hal_manager) *arg) /*initiate http object*/
{
    ASSERT(arg != NULL);
    NEW(arg->base_object_op_p,  base_object_implement);
    /*indicate that this object has been created once*/
    arg->init = init_event_hal_manager;
    arg->de_init =  deinit_event_hal_manager;
    arg->event_register = os_event_resister;
    arg->event_put = os_event_put;
    this = arg;
    return LX_OK;
}



