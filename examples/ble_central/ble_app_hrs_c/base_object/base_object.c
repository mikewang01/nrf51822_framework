/******************************************************************************
 * Copyright 2013-2014 hicling Systems (shanghai)
 *
 * FileName: base object.c
 *
 * Description: implement thefunction of heap
 * Modification history:
 *     2015/9/12, v1.0 create this file.
*******************************************************************************/
#include "lx_heap.h"
#include "base_object.h"
#include "../lx_nrf51Kit.h"
/*********************************************************************
* MACROS
*/

/*********************************************************************
* TYPEDEFS
*/
/*********************************************************************
* GLOBAL VARIABLES
*/
/*set to 1 to open the key trigger function */

/*********************************************************************
* LOCAL VARIABLES
*/

/*********************************************************************
* LOCAL FUNCTIONS
*/



/***************************************************************************
 ***************************************************************************
 **************** START OF PRIVATE  PROCEDURE IMPLEMENTATIONS **************
 ***************************************************************************/
/*********************************************************************
 * @fn      OS_err_t os_object_detach
 *
 * @brief   This function registers a device driver with specified name
 *
 * 			    @param dev the pointer of device driver structure
 *					@param name the device driver's name
 *					@param flags the flag of device
 *
 *   @return the LX_ERROR code, OS_EOK on initialization successfully.
 */
int os_object_find(CLASS(base_object_implement) *arg, struct base_object *object_dev_list, struct base_object **object_finded, const char * name) {
    struct base_object *obj = object_dev_list;

    if(obj == NULL) {
        return LX_ERROR; //no device in list
    }

    while(obj != NULL) {
        if(strcmp(name, obj->name) == 0){
						*object_finded = obj;
						return LX_OK;					
				}
        obj = obj->next;


    }

    return LX_OK;
}
/*********************************************************************
 * @fn      OS_err_t os_object_detach
 *
 * @brief   This function registers a device driver with specified name
 *
 * 			    @param dev the pointer of device driver structure
 *					@param name the device driver's name
 *					@param flags the flag of device
 *
 *   @return the LX_ERROR code, OS_EOK on initialization successfully.
 */
int os_object_add(CLASS(base_object_implement) *arg, struct base_object **object_list , struct base_object *obj, enum os_object_class_type class_type , const char *name)
{
    if(obj == NULL || name == NULL) {
        return LX_ERROR;
    }

    if((sizeof(name)) > OS_NAME_MAX) {
        return LX_ERROR;
    }

    strcpy(obj->name , name);
    obj->type = class_type;
    obj->next = NULL;
		
		LX_ENTER_CRITICAL();
    if(*object_list == NULL) {
        *object_list = obj;
    } else {
        struct base_object *object_temp = *object_list;

        while((object_temp)->next != NULL) {
            (object_temp) = (object_temp)->next;
        }

        (object_temp)->next = obj;
    }
		LX_EXIT_CRITICAL();
    return LX_OK;
}

/*********************************************************************
 * @fn      OS_err_t os_object_detach
 *
 * @brief   This function registers a device driver with specified name
 *
 * 			    @param dev the pointer of device driver structure
 *					@param name the device driver's name
 *					@param flags the flag of device
 *
 *   @return the LX_ERROR code, OS_EOK on initialization successfully.
 */
int os_object_detach(CLASS(base_object_implement) *arg, struct base_object **object_list_header, struct base_object *obj)
{

    struct base_object *obj_pre = NULL;
    struct base_object *object = *object_list_header;

    if(obj == NULL) {
        return LX_ERROR;
    }

    if(object == NULL) {
        return LX_OK;
    }

    while(object == NULL) {
        if(strcmp(object->name, obj->name) == 0) {
            if(obj_pre == NULL) {
                *object_list_header = object->next;
            } else {
                obj_pre = object->next;

            }

            return LX_OK;
        }

        obj_pre = object;
        object = object->next;
    }

    return LX_ERROR;
}
/******************************************************************************
 * FunctionName : int  socket_recieved(void* env)
 * Description  :data recieved function after data sended
 * Parameters   : level : output level of pin
 * Returns      : 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int deinit_base_object_implement(CLASS(base_object_implement) *arg) /*initiate http object*/
{
//	ASSERT(arg != NULL);
	LX_Free(arg);
	return LX_OK;
}


/******************************************************************************
 * FunctionName : int  init_base_object(void* env)
 * Description  : base object construction method
	* Parameters   : arg: object pointer
 * Returns      : 0: init successfully
 *				 -1: init failed
 *
*******************************************************************************/
int init_base_object_implement(CLASS(base_object_implement) *arg) /*initiate http object*/
{
//	assert(arg != NULL);
	arg->init = init_base_object_implement;
 	arg->de_init =  deinit_base_object_implement;
	arg->add =  os_object_add;
	arg->detach =  os_object_detach;
	arg->find = os_object_find;
	
	return LX_OK;
}
