#ifndef _BASE_OBJECT_H_
#define _BASE_OBJECT_H_

#include <stdint.h>
#include "oop_hal.h"

/*base structor*/


enum os_object_class_type {
    OS_Object_Class_Thread = 0,

    OS_Object_Class_Clock,
	
		OS_Object_Class_Mgr,

#ifdef OS_USING_SEMAPHORE
    OS_Object_Class_Semaphore,
#endif

#ifdef OS_USING_MUTEX
    OS_Object_Class_Mutex,
#endif

#ifdef OS_USING_EVENT
    OS_Object_Class_Event,
#endif

#ifdef OS_USING_MAILBOX

    OS_Object_Class_MailBox,

#endif
#ifdef OS_USING_MESSAGEQUEUE

    OS_Object_Class_MessageQueue,

#endif

#ifdef OS_USING_MEMPOOL
    OS_Object_Class_MemPool,
#endif

#ifdef OS_USING_DEVICE
    OS_Object_Class_Device,
#endif

    OS_Object_Class_Timer,

#ifdef OS_USING_MODULE
    OS_Object_Class_Module,
#endif

    OS_Object_Class_Unknown,

    OS_Object_Class_Static = 0x80
};


#define OS_NAME_MAX 8
struct base_object {
    char       				  name[OS_NAME_MAX];//name
    uint8_t 				    type;//kernel object type
    uint8_t				  	flag;//kernel object flasg
    struct base_object *next;//obejct list
};


DEF_CLASS(base_object_implement)
int (*init)(CLASS(base_object_implement)*);
int (*de_init)(CLASS(base_object_implement)*);
int (*add)(CLASS(base_object_implement) *arg, struct base_object **object_list , struct base_object *obj, enum os_object_class_type class_type , const char *name);
int (*find)(CLASS(base_object_implement) *arg, struct base_object *object_dev_list, struct base_object **object_finded, const char * name);
int (*detach)(CLASS(base_object_implement) *arg, struct base_object **object_list_header, struct base_object *obj);

END_DEF_CLASS(base_object_implement)


/* Hash a single 512-bit block. This is the core of the algorithm.*/
#endif	/* _BASE_OBJECT_H_ */
