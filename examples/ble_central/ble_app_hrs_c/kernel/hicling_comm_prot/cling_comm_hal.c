/***************************************************************
 * Name:      message.c
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-05-15
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/
/***************************************************************
 * Name:      message.c
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-05-15
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/
#include "cling_comm_mac.h"
#include "cling_comm_cmd.h"
#include "base_object.h"
#include "../../lx_nrf51Kit.h"
#include "cling_comm_link.h"
#include "cling_comm_hal.h"
#include <string.h>
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static int (*error_handle_callback)(uint8_t error_code) = NULL;
static int (*normal_package_recieve_callback)(char *msg, uint32_t len);
static int (*single_package_recieve_callback)(char *msg, uint32_t len);

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */
/*********************************************************************
 * @fn      comm_protocol_hal_send_packet
 *
 * @brief   hal layer function WHICH implement the function of relocating
 *          diffrent data stream into diffrent sending pipe
 *
 * @param   char type:message type uint8_t *msg: dat buffer uint32_t len : data lenth
 *
 * @return  LX_OK : SUCCESSFULLY LX_ERROR_ FUNCTION 
 */
int comm_protocol_hal_send_packet(char type, uint8_t *msg, uint32_t len)
{
    if ((type == PROTOCOL_MESSAGE_REGISTER_READ) || (type == PROTOCOL_MESSAGE_REGISTER_WRITE)) {
		// Register read/write is dedicated single-packet message
		 return logic_link_send_single_package(msg, len);
	}else if(type == PROTOCOL_MESSAGE_FILE_WRITE){ 
            /*there is no need to ack my package seeded*/        
        return logic_link_send_normal_package(msg, len, true);
    }else{   
        /*there is no need to ack my package seeded*/
        return logic_link_send_normal_package(msg, len, false);
    }
}



/*********************************************************************
 * @fn      comm_protocol_hal_single_package_recieved_callback
 *
 * @brief   hal layer sinle package recieve call back function resistered to logic layer
 *
 * @param  type uint8_t *msg: dat buffer uint32_t len : data lenth
 *
 * @return  LX_OK : SUCCESSFULLY LX_ERROR_ FUNCTION 
 */
int comm_protocol_hal_single_package_recieved_callback(char *msg, uint32_t len)
{
    if(single_package_recieve_callback != NULL){
        single_package_recieve_callback(msg, len);
    }
    return LX_OK;
}


/*********************************************************************
 * @fn      comm_protocol_hal_normal_package_recieved_callback
 *
 * @brief   hal layer normal package recieve call back function resistered to logic layer
 *
 * @param   type uint8_t *msg: dat buffer uint32_t len : data lenth
 *
 * @return  LX_OK : SUCCESSFULLY LX_ERROR_ FUNCTION 
 */
int comm_protocol_hal_normal_package_recieved_callback(char *msg, uint32_t len)
{
    DEBUG("HAL LAYER NORMAL PACKAGE RECIEVED\r\n");
    if(normal_package_recieve_callback != NULL){
        normal_package_recieve_callback(msg, len);
    }
    return LX_OK;
}


/*********************************************************************
 * @fn      comm_link_error_handler
 *
 * @brief   register single package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int hal_error_handler(uint8_t error_code)
{
    if(error_handle_callback != NULL) {
        error_handle_callback(error_code);
    }

}
/*********************************************************************
 * @fn     comm_link_normal_package_recieve_callback_register
 *
 * @brief  register normal package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_hal_single_package_recieve_callback_register(int (*p_callback)(char* , uint32_t))
{
    if(p_callback != NULL) {
        single_package_recieve_callback = p_callback;
    }
    return  LX_OK;
}
/*********************************************************************
 * @fn     comm_link_normal_package_recieve_callback_register
 *
 * @brief  register normal package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_hal_normal_package_recieve_callback_register(int (*p_callback)(char* , uint32_t))
{
    if(p_callback != NULL) {
        normal_package_recieve_callback = p_callback;
    }
    return  LX_OK;
}
/*********************************************************************
 * @fn     comm_hal_error_handle_register
 *
 * @brief  register normal package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_hal_error_handle_register(int (*p_callback)(uint8_t))
{
    if(p_callback != NULL) {
        error_handle_callback = p_callback;
    }
    return  LX_OK;
}
/*********************************************************************
 * @fn      cling_comm_hal_init
 *
 * @brief   This function is used for task to get message and message number.
 *
 * @param   task_state pointer ,msg pointer
 *
 * @return  message numbers
 */
int cling_comm_hal_init()
{
    //CLASS(base_object_implement) *list_mgr_obj
    cling_comm_logic_link_init();
    /*register callback function*/
    comm_link_normal_package_recieve_callback_register(comm_protocol_hal_normal_package_recieved_callback);
    comm_link_single_package_recieve_callback_register(comm_protocol_hal_single_package_recieved_callback);
    comm_link_error_handle_register(hal_error_handler);
    return LX_OK;
}
