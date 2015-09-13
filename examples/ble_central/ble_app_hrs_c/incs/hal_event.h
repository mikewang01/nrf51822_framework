/***************************************************************
 * Name:      hal_event.h
 * Purpose:   event asbtract layer
 * Author:    mikewang(s)
 * Created:   2015-09-13
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */


#ifndef _HAL_EVENT_H_
#define _HAL_EVENT_H_
/* Includes ------------------------------------------------------------------*/
#include "base_object.h"
#include "stdint.h"
#include "message.h"
/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
struct event_hal_object {
    struct    base_object parent; /*parent object used to manage the children onject*/
    uint16_t  spec_event;/*registered specic event*/
    union {
        uint16_t tmp; /*task state indication vary*/
        struct {
            uint8_t task_0: 1; /*task 0 registered state bit*/
            uint8_t task_1: 1; /*task 1 registered state bit*/
            uint8_t task_2: 1;/*task 1 registered state bit*/
            uint8_t task_3: 1;
            uint8_t task_4: 1;
            uint8_t task_5: 1;
            uint8_t task_6: 1;
            uint8_t task_8: 1;
            uint8_t task_9: 1;
            uint8_t task_10: 1;
            uint8_t task_11: 1;
            uint8_t task_12: 1;
            uint8_t task_13: 1;
            uint8_t task_14: 1;
            uint8_t task_15: 1;
            uint8_t task_16: 1;
        } task_id;
    } resistered_taskid_list;
};

DEF_CLASS(event_hal_manager)
CLASS(base_object_implement) *base_object_op_p; 
int (*init)(CLASS(event_hal_manager)*);
int (*de_init)(CLASS(event_hal_manager)*);
int (*event_register)(CLASS(event_hal_manager) *arg, uint16_t event, uint16_t task_id);
int (*event_put)(CLASS(event_hal_manager) *arg, uint16_t event, void *data, uint16_t lenth);
END_DEF_CLASS(event_hal_manager)


#endif /*_HAL_EVENT_H_*/

