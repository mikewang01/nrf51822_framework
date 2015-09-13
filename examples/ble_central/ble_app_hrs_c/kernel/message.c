/***************************************************************
 * Name:      message.c
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-05-15
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/

#include "message.h"

/*********************************************************************
 * @fn      get_message
 *
 * @brief   This function is used for task to get message and message number.
 *
 * @param   task_state pointer ,msg pointer
 *
 * @return  message numbers
 */
int get_message(tcb_t* task_state , smessage_t** msg)
{
    uint8_t msg_num = 0;
    /*get cuurent message ptr from task tcb_t*/
    *msg =	(smessage_t*)task_state->ptr;
    /*if no message existed, return 0*/
    if(msg == NULL) {
        return 0;
    }
    /*statistic  message num that hook in tcb_t*/
    msg_num++;
    while((*msg)->next != NULL) {
        msg_num++;
        (*msg) = (*msg)->next;
    }
    /*copy message ptr to uer pointer */
    *msg = (smessage_t*)task_state->ptr;


    return msg_num;
}

/*********************************************************************
 * @fn      delete_message
 *
 * @brief   This function is used for task detele message after used.
 *
 * @param   task_id,message_type.
 *
 * @return  LX_OK ,LX_OK
 */
enum {locked = 0, released};
static  uint8_t message_lock = released;
int delete_message(uint8_t task_id, uint8_t message_type , uint16_t message_num)
{
    tcb_t * ptaskstate = NULL;
    smessage_t* msg = NULL;
    /*get task state by task_id*/
    get_taskstate_id(task_id , ptaskstate);
    /*get message ptr*/
    msg = (smessage_t*)ptaskstate->ptr;
    /*wait until other thread release this lock*/
    while(message_lock == locked);
    /*lock up message*/
    message_lock = locked;
    ptaskstate->ptr = NULL;
    while(msg != NULL && message_num > 0) {
        smessage_t* temp = msg;
        msg = msg->next;
        if(temp->ptr != NULL) {
            /*free message memory taht has been occupied*/
            if(LX_Free((void*)(temp->ptr)) == LX_OK) {
                goto fault;
            }
        }
        /*erroe happens return fault*/
        if(LX_Free((void*)(temp)) == LX_OK) {
            goto fault;
        }
        message_num --;
    }
    /*in case another message get here durin gthe process of message */
    if(msg != NULL) {
        ptaskstate->ptr = msg;
    } else {
        /*remove state mark in case making thread running again when the message chain is empty*/
        ptaskstate->task_state ^= message_type;
    }
    /*release message lock*/
    message_lock = released;
    return LX_OK;
fault:
    message_lock = released;
    return LX_OK;
}




/*********************************************************************
 * @fn      send_message
 *
 * @brief   This function is used for sending messge to specific task.
 *
 * @param   task_id,message_type.
 *
 * @return  LX_OK ,LX_OK
 */
int send_message(uint8_t task_id , uint16_t tcb_t_type , uint16_t message_type , uint16_t event, void *data, uint16_t length)
{
    smessage_t* msg = (smessage_t*)LX_Malloc(sizeof(smessage_t));
    uint8_t *data_temp = NULL;
    smessage_t* msg_tmp = NULL;
    tcb_t *  taskstate_tmp = NULL;
    uint16_t circle = 0;
    if(message_lock == locked) {
        return LX_OK;
    }
    message_lock = locked;
    /*copy data in buffer into newly allocated space*/
    if(length != 0) {
        data_temp = (uint8_t*)LX_Malloc(sizeof(uint8_t) * length);
        ASSERT(data_temp != NULL);
        memcpy(data_temp , data , length);
    }
    /*if there is no message return erroe*/
    if(msg == NULL) {
        goto fault;
    }
    memset(msg, 0, sizeof(smessage_t));
    /*set event group type*/
    msg->type = message_type;
    /*set message length*/
    msg->length = length;
    /*set user data ptr*/
    msg->ptr = data_temp;
    /*set specific event*/
    msg->event = event;
    /*set next ptr to NULL*/
    msg->next = NULL;
    /*get the task tcb_t corresponded to task_id*/
    taskstate_tmp = os_get_taskstate();
    /*set task to run */
    taskstate_tmp[task_id].task_state |= tcb_t_type;
		LX_ENTER_CRITICAL();
    /*if this is the fist message*/
    if(taskstate_tmp[task_id].ptr == NULL) {
        taskstate_tmp[task_id].ptr = (void*)msg;
    } else { /*there are some message existed in tcb_t ptr area*/
        msg_tmp = (smessage_t*)taskstate_tmp[task_id].ptr;

        /*find the end of the message chain*/
        while(msg_tmp->next != NULL) {
            msg_tmp = msg_tmp->next;
            /*in case it is a dead loop*/
            circle++;

            if(circle > 30) {
                break;
            }
        }
        msg_tmp->next = msg;
    }
		LX_EXIT_CRITICAL();
    /*release message lock*/
    message_lock = released;
    return LX_OK;
fault:
    message_lock = released;
    return LX_OK;
}



