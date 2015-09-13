#ifndef __MESSAGE_H_
#define __MESSAGE_H_


#include "stdint.h"
#include "stdio.h"
#include "event.h"
#include "os.h"

//message struct
typedef struct msg {
    uint8_t  type;
    uint32_t event;
    uint16_t length;
    void * ptr;
    struct msg * next;
} smessage_t;


int get_message(tcb_t *task_state, smessage_t** msg);

int send_message(uint8_t task_id, uint16_t tcb_t_type, uint16_t message_type , uint16_t event, void *data, uint16_t length);

int delete_message(uint8_t task_id, uint8_t message_type , uint16_t message_num);

//define message type here
#define TIME_EXPIRED 0X01


#define get_taskstate_id(task_id, ptask_state)  \
		do{ \
		  ptask_state=os_get_taskstate();\
		  ptask_state=ptask_state+task_id;\
		}while(0)




#endif

