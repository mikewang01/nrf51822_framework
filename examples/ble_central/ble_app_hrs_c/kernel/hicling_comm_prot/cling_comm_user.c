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

/*********************************************************************
 * TYPEDEFS
 */
//Radio communication TX states
typedef enum {
    PROTOCOL_TX_IDLE = 0,
    PROTOCOL_TX_TRYING_SEND,
    PROTOCOL_TX_INVALID,
} PROTOCOL_TX_STATE;

//Radio communication RX states
typedef enum {
    PROTOCOL_RX_IDLE = 0,
    PROTOCOL_RX_RECEIVING,
    PROTOCOL_RX_COMPLETE,
    PROTOCOL_RX_INVALID,
} PROTOCOL_RX_STATE;

//Communication states are also bit flags
typedef enum {
    PROTOCOL_EVENT_INVALID,
    PROTOCOL_EVENT_RCVR_TO,
    PROTOCOL_EVENT_RCVR_DIRECT_PKT,
    PROTOCOL_EVENT_RCVR_LAST_PKT,
    PROTOCOL_EVENT_RCVR_ACK_PKT,
    PROTOCOL_EVENT_RCVR_MSG_CALLBACK,
    PROTOCOL_EVENT_RCVR_BUSY,
    PROTOCOL_EVENT_SEND_TO,
    PROTOCOL_EVENT_SEND_CANCEL,
} PROTOCOL_EVENT_TYPE;

typedef enum {
    PROTOCOL_TERMINAL_STATE_OK = 0,
    PROTOCOL_TERMINAL_STATE_BUSY,
    PROTOCOL_TERMINAL_STATE_FAIL,
    PROTOCOL_TERMINAL_STATE_INVALID_PARAMETER,
    PROTOCOL_TERMINAL_STATE_ERROR_LOW_MEMORY,
    PROTOCOL_TERMINAL_STATE_ERROR_WRITE_FILE_NAME,
    PROTOCOL_TERMINAL_STATE_ERROR_WRITE_FILE_LEN,
    PROTOCOL_TERMINAL_STATE_ERROR_GET_DEV_INFO,
    PROTOCOL_TERMINAL_STATE_ERROR_INSUFFICIENT_RESOURCE,
    PROTOCOL_TERMINAL_STATE_UNAVAILABLE,
    PROTOCOL_TERMINAL_STATE_MAX
} PROTOCOL_TERMINAL_STATE;



typedef struct tagPROTOCOL_PACKET_CONTEXT {
    // Received packet list
    //BASE_OBJECTLIST rxPacketList;
    // Packet list to be sent out
    // BASE_OBJECTLIST txPacketList;
    // Overall message are sent
    uint16_t messageNumber;
    // Message TX time stamp
    uint32_t messageTxTimeStamp;
} PROTOCOL_PACKET_CONTEXT;

typedef enum {
    PROTOCOL_MESSAGE_STATE_IDLE = 0,
    PROTOCOL_MESSAGE_STATE_BUSY,
} MESSAGE_STATE;

typedef struct device_info_context{
    uint8_t                statusReg;
    bool                bUpdated;

} device_info_context_t;

//cling_device_time_contex_t
// Device Hardware Configuration information
//
typedef struct cling_device_info_context {
    uint8_t                touch_version;/*touch version serial*/
    uint8_t                sensor_type;  /*senser type*/
    uint8_t                battery_level; /*bettery level status report*/
    uint8_t                software_major;/*software version major id*/
    uint8_t                software_minor;/*software version minor id*/
    uint8_t                ble_minor;/**/
    uint8_t                memory_free[4]; /*memory avalable left*/
    uint8_t                available_file_writes[2];/*avalable files  that can be writed*/
    uint8_t                bond_token[2]; /*bond token*/
    uint8_t                bond_crc[2];/*bond crc number*/ 
    uint8_t                bonded_user_id[4]; /*the user id boned with this cling device*/
    uint8_t                bond_epoch[4];/**/
    uint8_t                cling_id[20]; /*cling id string*/
    uint8_t				   reset_count[4]; /*reset times of cling device*/
    uint8_t                model_number[6]; /**/
    uint8_t                minute_streaming_file;/**/
    bool                   bUpdated;/**/
} cling_device_info_context_t;

//
// Device daily total information
//
typedef struct tagcling_daily_activity_contex_t {
    uint8_t steps[4];
    uint8_t distance[4];
    uint8_t caloriesActive[2];
    uint8_t caloriesIdle[2];
    uint8_t wakeUpTimes;
    uint8_t sleepLight[2];
    uint8_t sleepSound[2];
    uint8_t sleepRem[2];
    uint8_t heartRate;
    uint8_t skinTemperature[2];
    uint8_t acttype : 4;
    uint8_t uv : 4;
    bool  bUpdated;
} cling_daily_activity_contex_t;

//
// Device binding information
//
typedef struct tagcling_binding_info_contex_t {
    uint8_t state;
    uint8_t errorCode;
    uint8_t userId[4];
    bool bUpdated;
} cling_binding_info_contex_t;

//
// Device File List Structure
//
#define MAXIMUM_FILE_NAME_LEN		128
typedef struct cling_file_list_context {
    uint16_t totalFiles;
    uint8_t name[500][MAXIMUM_FILE_NAME_LEN];
    uint32_t length[500];
    uint16_t crc[500];
    bool bUpdated;
} cling_file_list_context_t;

//
// Device weather info structure
//
typedef struct cling_weather_contex{
    uint8_t month;
    uint8_t day;
    uint8_t type;
    uint8_t low_temperature;
    uint8_t high_temperature;
} cling_weather_contex_t;

//
// Device weather info structure
//
typedef struct cling_user_reminder_contex {
    uint8_t hour;
    uint8_t minute;
} cling_user_reminder_contex_t;

//
// Device time context
//
typedef struct cling_device_time_contex {
    uint8_t tm_hour_local; // +/- 12 hours (in 15 minutes) -> +/- 48 unit in 15 minutes
    uint8_t tm_epoch[4];
} cling_device_time_contex_t;

typedef enum {
    CLING_AUTH_IDLE,
    CLING_AUTH_A1_DONE,
    CLING_AUTH_A2_DONE,
    CLING_AUTH_FA_DONE,
    CLING_AUTH_ERROR,
} CLING_AUTH_STATES;

//
// CP API Internal Object
//
typedef struct cpapi_object {
    // All the file list
    cling_file_list_context_t *pFileList;
    // File Content
    uint8_t *pFileData;
    // Cling Device Info
    cling_device_info_context_t *pClingInfo;
    // CLING Bind/authentication Info
    cling_binding_info_contex_t *pLinkInfo;
    // Callback function pointer
    int (*pCallback)(uint8_t, uint8_t*);
} cpapi_object_t;

typedef struct file_context {
    uint32_t downloadLength; // file length downloaded
    uint32_t uploadLength;   // file length uploaded
    uint8_t name[128];
    bool bUpdated;
    bool bUploading;
    uint32_t length;          // file actual length

} file_context_t;

typedef struct cling_cp_object {
    // Cling communication channel list
    //BASE_OBJECTLIST channelList;
    // Packet TX/RX state
    PROTOCOL_PACKET_CONTEXT packet;
    // File operation context
    file_context_t file;
    // critical section (TX)
    //OS_CRITICALSECTION criticalSectionTX;
    // critical section (RX)
    //OS_CRITICALSECTION criticalSectionRX;
    // device state
    MESSAGE_STATE messageState;
    // Device info
    device_info_context_t device;								// Local device information
    // Terminal state
    PROTOCOL_TERMINAL_STATE terminalState;
#if 0
    CLING_AUTHENTICATION_CTX auth;
#endif
} cling_cp_object;

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static int (*error_handler_callback)(uint8_t error_code) = NULL;
static int (*stream_package_recieve_callback)(char *msg, uint32_t len);
static int (*normal_package_recieve_callback)(char *msg, uint32_t len);

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */


/*********************************************************************
 * @fn      comm_protocol_hal_single_package_recieved_callback
 *
 * @brief   hal layer sinle package recieve call back function resistered to logic layer
 *
 * @param  type uint8_t *msg: dat buffer uint32_t len : data lenth
 *
 * @return  LX_OK : SUCCESSFULLY LX_ERROR_ FUNCTION 
 */
int comm_user_stream_recieved_callback(char *msg, uint32_t len)
{
 
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
int comm_user_normal_recieved_callback(char *msg, uint32_t len)
{
    DEBUG("user RECIEVED lenth = %d\r\n", len);
    struct cling_device_info_context *t = (struct cling_device_info_context *)msg;
    t->cling_id[19] = 0;
                int i = 0;
            for (i = 0; i < len; i++) {
                DEBUG("0x%02x ", msg[i]);
            }
   // DEBUG("[ling id] = %s\r\n", t->cling_id);
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
static int comm_user_error_handler(uint8_t error_code)
{
    if(error_handler_callback != NULL) {
        error_handler_callback(error_code);
    }
    return LX_OK;
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
void cling_comm_user_init()
{
    cling_comm_hal_init();
    comm_hal_normal_package_recieve_callback_register(comm_user_normal_recieved_callback);
    comm_hal_single_package_recieve_callback_register(comm_user_stream_recieved_callback);
    comm_hal_error_handle_register(comm_user_error_handler);
}

