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
#include "cling_comm_cmd.h"
#include "cling_comm_mac.h"
#include "cling_comm_hal.h"
#include "base_object.h"
#include "../../lx_nrf51Kit.h"

#include <string.h>



//
// All the system commands
//
enum {
    SYSTEM_COMMAND_STREAMING_MODE,
    SYSTEM_COMMAND_AUTH_A0,
    SYSTEM_COMMAND_AUTH_A1,
    SYSTEM_COMMAND_AUTH_A2,
    SYSTEM_COMMAND_AUTH_FA,
    SYSTEM_COMMAND_TIME_SYNC,
    SYSTEM_COMMAND_DISK_FORMAT,
    SYSTEM_COMMAND_RESET_FOR_NEW_USER,
} ;

static int (*error_handler_callback)(uint8_t error_code) = NULL;
static int (*stream_package_recieve_callback)(char *msg, uint32_t len);
static int (*normal_package_recieve_callback)(char *msg, uint32_t len);

/*********************************************************************
 * @fn      cling_comm_cmd_set_system_mode
 *
 * @brief   This function is to peovide a interface for sending command and it is internal uesd
 *
* @param   uint8_t command: specifc command order
*          bool b_ena, :enable  register send or not
*          uint8_t *data : related data need seending
 *
 * @return  none
 */
static void cling_comm_cmd_set_system_mode(uint8_t command, bool b_ena, uint8_t *data, uint16_t lenth)
{
    uint8_t buf[MAXIMUM_PACKET_SIZE];
    uint8_t *p;
    uint8_t *reg;
    uint8_t mode = command;

    buf[0] = PROTOCOL_MESSAGE_REGISTER_WRITE;
    reg = buf + 1;
    reg[SYSTEM_DEVICE_REGISTER_CTL] = 0;
    reg[SYSTEM_DEVICE_REGISTER_CLEAR] = 0;

    // Enable / Disable a mode control bit
    if (b_ena ==  true)
        p = &reg[SYSTEM_DEVICE_REGISTER_CTL]; // Enable uses control register
    else
        p = &reg[SYSTEM_DEVICE_REGISTER_CLEAR]; // Disable uses clear register

    switch (mode) {
        case SYSTEM_COMMAND_STREAMING_MODE:
            *p |= CTL_IM;
            break;
        case SYSTEM_COMMAND_TIME_SYNC:
            *p |= CTL_TS;
            memcpy(buf + 4, data, lenth);
            break;
        case SYSTEM_COMMAND_DISK_FORMAT:
            *p |= CTL_DF;
            reg[SYSTEM_DEVICE_REGISTER_PROTECT] = *((uint8_t *)data);
            break;
        case SYSTEM_COMMAND_AUTH_A0:
            *p |= CTL_A0;
            break;
        case SYSTEM_COMMAND_RESET_FOR_NEW_USER:
            *p |= CTL_RFNU;
            reg[SYSTEM_DEVICE_REGISTER_PROTECT] = *((uint8_t *)data);
            break;
        case SYSTEM_COMMAND_AUTH_A1:
            *p |= CTL_A1;
            break;
        case SYSTEM_COMMAND_AUTH_A2:
            *p |= CTL_A2;
            break;
        case SYSTEM_COMMAND_AUTH_FA:
            *p |= CTL_FA;
            break;
        default:
            break;
    }
    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_REGISTER_WRITE, buf, MAXIMUM_PACKET_SIZE);
}
/*********************************************************************
 * @fn      cling_comm_cmd_get_daily_activity_total
 *
 * @brief   get daily activity total function
 *
* @param   none
*
* @return  none
*/
void cling_comm_cmd_get_daily_activity_total()
{
    uint8_t buf[2];
    // Load device info
    buf[0] = PROTOCOL_MESSAGE_LOAD_DAILY_ACTIVITY;
    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_LOAD_DAILY_ACTIVITY, buf, 2);
}
/*********************************************************************
 * @fn      cling_comm_cmd_load_device_info
 *
 * @brief   load device info function for user used
 *
* @param   none
*
* @return  none
*/
void cling_comm_cmd_load_device_info()
{
    uint8_t buf[2];
    buf[0] = PROTOCOL_MESSAGE_LOAD_DEVICE_INFO;
    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_LOAD_DEVICE_INFO, buf, 1);
}
/*********************************************************************
 * @fn      cling_comm_cmd_sync_time
 *
 * @brief   sync time with load device
 *
* @param   none
*
* @return  none
*/
void cling_comm_cmd_sync_time(char *data, uint16_t lenth)//(cling_device_time_contex_t t)
{
    // Sync client to UTC and local time
    cling_comm_cmd_set_system_mode(SYSTEM_COMMAND_TIME_SYNC, true, (uint8_t *)data, lenth);
}

/*********************************************************************
 * @fn      cling_comm_cmd_device_over_the_air_update
 *
 * @brief   make device enter update in the air mode
 *
* @param   none
*
* @return  none
*/
int cling_comm_cmd_device_over_the_air_update()
{
    uint8_t buf[2];
    // PROTOCOL_MESSAGE_START_OTA
    buf[0] = PROTOCOL_MESSAGE_START_OTA;
    return comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_START_OTA, buf, 1);
}
/*********************************************************************
 * @fn      cling_comm_cmd_update_weather_forcast
 *
 * @brief   syc device with weather infomation
 *
* @param   uint8_t *data: specific data
*          int numOfDays: the amount of wather information it contained
*
* @return  none
*/
int cling_comm_cmd_update_weather_forcast(char *data, uint16_t len)
{
    return comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_WEATHER, (uint8_t*)data, len);

}
/*********************************************************************
 * @fn      cling_comm_cmd_update_user_reminder
 *
 * @brief   syc device with clock remider
 *
* @param   uint8_t *data: specific data
*          int numOfDays: the amount of reminder information it contained
*
* @return  none
*/
int cling_comm_cmd_update_user_reminder(uint8_t *data, uint16_t len)
{
    uint8_t buf[200];

    // update user reminder message
    buf[0] = PROTOCOL_MESSAGE_USER_REMINDER;
    buf[1] = len;

    if (len > 0) {
        memcpy(buf + 2, data, len);
    } else {
        memset(buf + 2, 0xff, 100);
        DEBUG("[USER reminder] NO REMINDER IS SET");
    }
    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_USER_REMINDER, buf, len + 2);


}
/*********************************************************************
 * @fn      cling_comm_cmd_update_simulation
 *
 * @brief   syc device with clock remider
 *
* @param   uint8_t *data: specific data
*          int numOfDays: the amount of reminder information it contained
*
* @return  none
*/
int cling_comm_cmd_update_simulation(uint8_t *data, uint16_t lenth)
{
    uint8_t buf[200];

    // Load device info
    buf[0] = PROTOCOL_MESSAGE_SIMULATION;
    buf[1] = data[0];
    return comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_USER_REMINDER, buf, 2);
}
/*********************************************************************
 * @fn      cling_comm_cmd_set_ancs
 *
 * @brief   set ancs data
 *
* @param   uint8_t *data: specific data
*
* @return  none
*/
int cling_comm_cmd_set_ancs(uint8_t *data, uint16_t len)
{
    uint8_t buf[5];
    // Load device info
    buf[0] = PROTOCOL_MESSAGE_SET_ANCS;
    buf[1] = data[0]; // Enable ANCS
    buf[2] = data[1];
    buf[3] = data[2];
    return  comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_SET_ANCS, buf, 5);
}
/*********************************************************************
 * @fn      cling_comm_cmd_set_smart_notification
 *
 * @brief   set ancs data
 *
* @param   uint8_t *data: specific data
*
* @return  none
*/
int cling_comm_cmd_set_smart_notification(uint8_t * data, uint16_t len)
{
    uint8_t buf[5];
    //ENTER_FUNC();
    buf[0] = PROTOCOL_MESSAGE_SET_SMART_NOTIFICATION;
    buf[1] = data[0]; // smart notification mode
    buf[2] = data[1]; // id
    buf[3] = data[2]; // counts
    return comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_SET_SMART_NOTIFICATION, buf, 5);

}
/*********************************************************************
 * @fn      cling_comm_cmd_device_setup
 *
 * @brief   set ancs data
 *
* @param   uint8_t *data: specific data
*
* @return  none
*/
int cling_comm_cmd_device_setup(uint8_t *data, uint16_t len)
{
    uint8_t buf[32];

    //SET UP DEVICE DATA
    buf[0] = PROTOCOL_MESSAGE_DEVICE_SETTING;
    memcpy(buf + 1, data, len);
    return comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_DEVICE_SETTING, buf, len + 1);
}
/*********************************************************************
 * @fn      cling_comm_cmd_device_reboot
 *
 * @brief   reboot system order
 *
* @param   void
*
* @return  none
*/
int cling_comm_cmd_device_reboot()
{
    uint8_t buf[2];
    // Load device info
    buf[0] = PROTOCOL_MESSAGE_REBOOT;
    return comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_REBOOT, buf, 1);
}
/*********************************************************************
 * @fn      cling_comm_cmd_write_one_file
 *
 * @brief   write the data of flen lenth into specific file whose name is f_name
 *
* @param   uint8_t *f_name: file name in cling device
*          uint8_t *data: data buffer pointer
*          uint32_t f_len  :data lenth
*
* @return  none
*/
int cling_comm_cmd_write_one_file(uint8_t *f_name, uint8_t *data, uint32_t f_len)
{
    uint32_t msg_len;
    uint8_t string_len = (uint8_t)(strlen((char *)f_name) + 1);
    uint8_t *msg;
    uint32_t pos = 0;
    msg_len = 1;  // Message type - file write
    msg_len += 4; // File length
    msg_len += 1; // 1 uint8_t of string length
    msg_len += string_len; // String
    msg_len += f_len; // file content;
    msg = (uint8_t *)LX_Malloc(msg_len);
    // Put in message type
    msg[pos++] = PROTOCOL_MESSAGE_FILE_WRITE;
    // Put in file length
    msg[pos++] = (f_len >> 24) & 0xff;
    msg[pos++] = (f_len >> 16) & 0xff;
    msg[pos++] = (f_len >> 8) & 0xff;
    msg[pos++] = f_len & 0xff;

    // Put in file name string length
    msg[pos++] = string_len;

    // Put in file name
    memcpy(msg + pos, f_name, string_len);
    pos += string_len;

    // Put in file content
    memcpy(msg + pos, data, f_len);
    pos += f_len;

    // Send out this message
    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_FILE_WRITE, msg, msg_len);

    LX_Free(msg);
    
    return  LX_OK;
}
/*********************************************************************
 * @fn      cling_comm_cmd_read_one_file
 *
 * @brief   read specific file data from specific file
 *
* @param   uint8_t *f_name: file name in cling device
*
* @return  none
*/
int cling_comm_cmd_read_one_file(uint8_t *f_name)
{
    uint32_t msg_len;
    uint8_t string_len = (uint8_t)(strlen((char *)f_name) + 1);
    uint8_t *msg;
    uint32_t pos = 0;

    msg_len = 1;  // Message type - file write
    msg_len += 1; // 1 uint8_t of string length
    msg_len += string_len; // String

    msg = (uint8_t *)LX_Malloc(msg_len);

    msg[pos++] = PROTOCOL_MESSAGE_FILE_READ;

    msg[pos++] = string_len;

    memcpy(msg + pos, f_name, string_len);
    //pos += string_len; // Update message position for extended field, but not used at the moment.

    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_FILE_READ, msg, msg_len);

    LX_Free(msg);
    
    return LX_OK;
}
/*********************************************************************
 * @fn      cling_comm_cmd_delete_one_file
 *
 * @brief   delete specific file data from specific file
 *
* @param   uint8_t *f_name: file name in cling device
*
* @return  none
*/
int cling_comm_cmd_delete_one_file(uint8_t *f_name)
{
    // TERM sends delete command to the CLING with a specific file name
    uint32_t msg_len;
    uint8_t string_len = (uint8_t)(strlen((char *)f_name) + 1);
    uint8_t *msg;
    uint32_t pos = 0;

    msg_len = 1;  // Message type - file write
    msg_len += 1; // 1 uint8_t of string length
    msg_len += string_len; // String

    msg = (uint8_t *)LX_Malloc(msg_len);

    // Delete a file
    msg[pos++] = PROTOCOL_MESSAGE_FILE_DELETE;

    // The file name length
    msg[pos++] = string_len;
    // copy the file name string
    memcpy(msg + pos, f_name, string_len);
    //pos += string_len; // Update message position for extended field, but not used at the moment.
    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_FILE_DELETE, msg, msg_len);
    LX_Free(msg);
    return LX_OK;
}
/*********************************************************************
* @fn      cling_comm_cmd_get_write_file_status
*
* @brief   delete specific file data from specific file
*
* @param   uint8_t *f_name: file name in cling device
*
* @return  none
*/
void cling_comm_cmd_get_write_file_status(uint32_t *len, uint8_t *f_name)
{

}

void cling_comm_cmd_write_file(uint8_t *f_name, uint8_t *p_f_content, uint32_t f_len)
{
    // Notify CLING that we are ready for file transfering
    cling_comm_cmd_write_one_file(f_name, p_f_content, f_len);
}

int cling_comm_cmd_read_file(uint8_t *f_name, uint8_t *f_content, uint32_t f_len)
{
    // Notify CLING that we are ready for file transfering
    cling_comm_cmd_read_one_file(f_name);
    return 0;
}

void cling_comm_cmd_get_read_file_stat(uint32_t *len, uint8_t *f_name)
{

}

// Get local file list
void cling_comm_cmd_get_file_list()
{
    uint8_t buf[2];
    buf[0] = PROTOCOL_MESSAGE_FILE_LOAD_LIST;
    comm_protocol_hal_send_packet(PROTOCOL_MESSAGE_FILE_LOAD_LIST, buf, 1);
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
int comm_cmd_stream_recieved_callback(char *msg, uint32_t len)
{
    if(stream_package_recieve_callback != NULL) {
        stream_package_recieve_callback(msg, len);
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
int comm_cmd_normal_recieved_callback(char *msg, uint32_t len)
{
    DEBUG("cmd LAYER NORMAL PACKAGE RECIEVED\r\n");
    if(normal_package_recieve_callback != NULL) {
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
static int comm_cmd_error_handler(uint8_t error_code)
{
    if(error_handler_callback != NULL) {
        error_handler_callback(error_code);
    }
    return LX_OK;
}
/*********************************************************************
 * @fn     comm_link_normal_package_recieve_callback_register
 *
 * @brief  register normal package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_cmd_stream_recieved_callback_register(int (*p_callback)(char* , uint32_t))
{
    if(p_callback != NULL) {
        stream_package_recieve_callback = p_callback;
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
int comm_cmd_normal_package_recieve_callback_register(int (*p_callback)(char* , uint32_t))
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
int comm_cmd_error_handle_register(int (*p_callback)(uint8_t))
{
    if(p_callback != NULL) {
        error_handler_callback = p_callback;
    }
    return  LX_OK;
}
/*********************************************************************
* @fn      cling_comm_cmd_init
*
* @brief   comm protocol communication initiate and resister coresponed callback function
*
* @param   void
*
* @return  none
*/
void cling_comm_cmd_init()
{
    cling_comm_hal_init();
    comm_hal_normal_package_recieve_callback_register(comm_cmd_normal_recieved_callback);
    comm_hal_single_package_recieve_callback_register(comm_cmd_stream_recieved_callback);
    comm_hal_error_handle_register(comm_cmd_error_handler);
}

