/***************************************************************
 * Name:      __CLING_COMM_MAC_H__
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-06-12
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */

#ifndef __CLING_COMM_MAC_H__
#define __CLING_COMM_MAC_H__

/* Exported constants --------------------------------------------------------*/

#include "stdint.h"
#include <nrf.h>
#include <stdbool.h>
/* Exported types ------------------------------------------------------------*/
// Maximum message length: 256 KB

#define MAXIMUM_MESSAGE_LENGTH (256*1024)

// Maximum packet payload size
#define MAXIMUM_PACKET_PAYLOAD_SIZE 16
#define MAXIMUM_PACKET_SIZE           20
#define MAXIMUM_UUID_PACKET_SIZE 22

// Customized BLE characteristics
#define UUID_TX_SP      0xffe1
#define UUID_TX_START   0xffe2
#define UUID_TX_MIDDLE  0xffe3
#define UUID_TX_END     0xffe4


#define UUID_RX_SP      0xffe5
#define UUID_RX_START   0xffe6
#define UUID_RX_MIDDLE  0xffe7
#define UUID_RX_END     0xffe8


enum {
    SINGLE_PACKAGE = 0,
    NORMAL_PACKAGE = 1
};

int cling_comm_mac_init(void);
int ble_stack_mac_layer_write_rsp_callback_register(int (*p_callback)(void));
int comm_mac_single_package_recieve_callback_register(int (*p_callback)(uint16_t, char , char* , uint32_t));
int comm_mac_normal_package_recieve_callback_register(int (*p_callback)(uint16_t, char , char* , uint32_t));

void comm_protocol_mac_send_singlepacket(uint16_t uuid, char type, char *msg, uint32_t len);
void comm_protocol_mac_send_normal_package(uint16_t uuid, char type, char *msg, uint32_t len);
int comm_mac_error_handle_register(int (*p_callback)(uint8_t));

#endif 



