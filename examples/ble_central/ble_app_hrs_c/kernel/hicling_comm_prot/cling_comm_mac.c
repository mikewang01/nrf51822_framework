/***************************************************************
 * Name:      cling_comm_mac.c
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2015-09-14
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include "cling_comm_mac.h"
#include "cling_comm_cmd.h"
#include "../../lx_nrf51Kit.h"
#include <string.h>
/*********************************************************************
 * @fn      cling_cp_send_data
 *
 * @brief   send data funtion used to pass data to lower layer
 *
 * @param   uuid :  charactor uuid
 *          data :  data pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
#define CLING_PACKET_LENTH_SIZE 3
void ble_tx_send(uint16_t uuid, char *str , unsigned char len);
int cling_cp_send_data(uint16_t uuid, char *data, uint16_t lenth)
{

    struct protocol_serial_link_packet *pkt = NULL;
    pkt = (struct protocol_serial_link_packet *)LX_Malloc(sizeof(struct protocol_serial_link_packet));
    /*fullfill uuid feild*/
    uint16_t t = htons(uuid);
    memcpy(pkt->uuid, &t, sizeof(uuid));
    /*fullfill data area in payload field*/
    memcpy(pkt->payload, data, lenth);
    /*convert data from little endian to big edian*/
    uint32_t i = htonl(lenth);
    memcpy(pkt->length, &i, CLING_PACKET_LENTH_SIZE);

    ble_tx_send(uuid, (char *)pkt + sizeof(uuid), sizeof(struct protocol_serial_link_packet) - sizeof(uuid));
#if 1
    {
        char *t = (char *)pkt;
        DEBUG("Single message (22): %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",
              t[0], t[1], t[2], t[3], t[4],
              t[5], t[6], t[7], t[8], t[9]);
    }
#endif
}


static void _receivedPacketProcess(struct protocol_serial_link_packet *spacket);
/*********************************************************************
 * @fn      cling_cp_recieve_one_pacakge
 *
 * @brief   recieve data funtion called by lower layer to pass data to upper layer
 *
 * @param   uuid :  charactor uuid
 *          data :  data pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int cling_cp_recieve_one_pacakge(uint16_t uuid, char *data, uint32_t lenth)
{

    struct protocol_serial_link_packet p;
    /*fullfill uuid feild*/
    uuid = htons(uuid);
    memcpy(p.uuid, &uuid, sizeof(uuid));
    memcpy(p.payload, data, lenth);
    /*FULL FILL payload lenth field*/
    lenth = htonl(lenth);
    memcpy(p.length, &lenth, CLING_PACKET_LENTH_SIZE);
    _receivedPacketProcess(&p);


}



static void _receivedPacketProcess(struct protocol_serial_link_packet *spacket)
{
    COMMUNICATION_CHANNEL *c;
    uint16_t UUID = spacket->uuid[0];
    PROTOCOL_EVENT_TYPE evt;


    // Assemble a message
    UUID <<= 8;
    UUID += spacket->uuid[1];

    DEBUG("_receivedPacketProcess: UUID=0x%x", UUID);
    if (UUID == UUID_TX_SP) {
        DEBUG ("_receivedPacketProcess: msgid=%d", spacket->messageId);
        if (spacket->messageId == PROTOCOL_MESSAGE_STREAMING_SECOND) {

            return;
        }
        if (spacket->messageId == PROTOCOL_MESSAGE_STREAMING_MINUTE) {


            return;
        }
        if (spacket->messageId == PROTOCOL_MESSAGE_STREAMING_MULTI_MINUTES) {

            return;
        }
        if (spacket->messageId == PROTOCOL_MESSAGE_STREAMING_DAY) {

            return;
        }
    } else {

    }

    if ((UUID == UUID_TX_SP) || (UUID == UUID_TX_START) || (UUID == UUID_TX_MIDDLE) || (UUID == UUID_TX_END)) {


    }

    switch (UUID) {
        case UUID_TX_SP:

            break;
        case UUID_TX_START:

            break;
        case UUID_TX_MIDDLE:

            break;
        case UUID_TX_END:

            break;
        default:
            // No need for further processing.
            return;
    }


}


static void _sendSinglePacketMessage(char type, char *msg, uint32_t len)
{

    struct protocol_serial_link_packet *pkt;
    pkt = (struct protocol_serial_link_packet *)LX_Malloc(sizeof(struct protocol_serial_link_packet));;

    // Indiate it is a starting packet
    pkt->uuid[0] = (UUID_RX_SP >> 8) & 0xff;
    pkt->uuid[1] = UUID_RX_SP & 0xff;
    memcpy(&pkt->messageId, msg, MAXIMUM_PACKET_SIZE);

    DEBUG("wait 1020\r\n");

    uint16_t uuid = UUID_RX_SP;
    ble_tx_send(uuid, (char *)pkt + sizeof(uuid), sizeof(struct protocol_serial_link_packet) - sizeof(uuid));

    DEBUG("post 1020\r\n");

#if 1
    {
        char *data = (char *)pkt;

        DEBUG("Single message (22): %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
              data[0], data[1], data[2], data[3], data[4],
              data[5], data[6], data[7], data[8], data[9]);
    }
#endif
}

void PROTOCOL_sendPacketProcess(char type, char *msg, uint32_t len)
{
    struct protocol_serial_link_packet t;
    struct protocol_serial_link_packet *pkt;
    pkt = &t;
	// Indiate it is a starting packet
	pkt->uuid[0] = (UUID_RX_START>>8) & 0xff;
	pkt->uuid[1] = UUID_RX_START & 0xff;
	pkt->messageId = 0;
	pkt->length[0] = (len >> 16) & 0xff;
	pkt->length[1] = (len >> 8) & 0xff;
	pkt->length[2] = len & 0xff;
    memcpy(pkt->payload, msg, len);
    DEBUG("wait 1020\r\n");

    uint16_t uuid = UUID_RX_START;
    ble_tx_send(uuid, (char *)pkt + sizeof(uuid), sizeof(struct protocol_serial_link_packet) - sizeof(uuid));
    #if 1
    {
        char *data = (char *)pkt;

        DEBUG(" message (22): %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",
              data[0], data[1], data[2], data[3], data[4],
              data[5], data[6], data[7], data[8], data[9]);
    }
#endif
    DEBUG("post 1020\r\n");

}

void TERMINAL_loadDeviceInfo()
{
    char buf[2];

    // Load device info
    buf[0] = PROTOCOL_MESSAGE_LOAD_DEVICE_INFO;

    PROTOCOL_sendPacketProcess(PROTOCOL_MESSAGE_LOAD_DEVICE_INFO, buf, 1);

}

void cpapi_get_local_deviceinfo()
{
    TERMINAL_loadDeviceInfo();
}