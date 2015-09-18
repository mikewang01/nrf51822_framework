/***************************************************************
 * Name:      __CLING_COMM_CMD_H__
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-06-12
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */

#ifndef __CLING_COMM_CMD_H__
#define __CLING_COMM_CMD_H__

/* Exported constants --------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/
// ----------------------------------
// Control register definition
#define CTL_IM   0x80   /* Streaming */
#define CTL_A2   0x40   /* Auth A1 */
#define CTL_A1   0x20   /* Auth A2*/
#define CTL_TS   0x10  /* time sync */
#define CTL_DF   0x08  /* disk format */
#define CTL_A0   0x04  /* keep alive */
#define CTL_FA   0x02  /* Auth FA */
#define CTL_RFNU 0x01  /* Reset for new user .*/

// Protocal message order type
typedef enum {
    PROTOCOL_MESSAGE_REGISTER_READ = 0,        // 0
    PROTOCOL_MESSAGE_REGISTER_WRITE,
    PROTOCOL_MESSAGE_FILE_LOAD_LIST,
    PROTOCOL_MESSAGE_FILE_READ,
    PROTOCOL_MESSAGE_FILE_WRITE,
    PROTOCOL_MESSAGE_FILE_DELETE,              // 5
    PROTOCOL_MESSAGE_LOAD_DEVICE_INFO,
    PROTOCOL_MESSAGE_STREAMING_SECOND,
    PROTOCOL_MESSAGE_STREAMING_MINUTE,
    PROTOCOL_MESSAGE_STREAMING_MULTI_MINUTES,
    PROTOCOL_MESSAGE_ACK,                      // 10
    PROTOCOL_MESSAGE_AUTHENTICATIOIN,
    PROTOCOL_MESSAGE_REBOOT,
    PROTOCOL_MESSAGE_START_OTA,
    PROTOCOL_MESSAGE_WEATHER,
    PROTOCOL_MESSAGE_USER_REMINDER,            // 15
    PROTOCOL_MESSAGE_SIMULATION,
    PROTOCOL_MESSAGE_BLE_DISCONNECT,
    PROTOCOL_MESSAGE_LOAD_DAILY_ACTIVITY,
    PROTOCOL_MESSAGE_SET_ANCS,
    PROTOCOL_MESSAGE_DEBUG_MSG,                // 20
    PROTOCOL_MESSAGE_DEVICE_SETTING,
    PROTOCOL_MESSAGE_STREAMING_DAY,
    PROTOCOL_MESSAGE_SET_SMART_NOTIFICATION,
} PROTOCOL_MESSAGE_TYPE;

typedef enum {
    SYSTEM_DEVICE_REGISTER_CTL = 0,
    SYSTEM_DEVICE_REGISTER_CLEAR,
    SYSTEM_DEVICE_REGISTER_REVH,
    SYSTEM_DEVICE_REGISTER_REVL,
    SYSTEM_DEVICE_REGISTER_HWINFO,
    SYSTEM_DEVICE_REGISTER_HW_REV,
    SYSTEM_DEVICE_REGISTER_BATTERY,
    SYSTEM_DEVICE_REGISTER_MUTEX,
    SYSTEM_DEVICE_REGISTER_PROTECT,

} SYSTEM_DEVICE_REGISTER_LIST;


enum{
     COMM_PROTOCOL_ERROR_RX_FAILED,
     COMM_PROTOCOL_ERROR_TX_FAILED,
};

#endif // __VERSION_H__


