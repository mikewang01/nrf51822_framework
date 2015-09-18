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
#include "base_object.h"
#include "../../lx_nrf51Kit.h"
#include <string.h>

/*********************************************************************
 * TYPEDEFS
 */

struct protocol_serial_link_packet {
    struct base_object *parent;
    // Unique ID to file/register type
    uint8_t uuid[2];
    uint8_t type; /*single package or normal package*/
    union {
        struct {
            // Short header - 4 uint8_ts
            uint8_t messageId;
            // Message length
            uint8_t length[3];
            // Payload - 16 uint8_ts
            uint8_t payload[MAXIMUM_PACKET_PAYLOAD_SIZE];
        } normal_package;
        struct {
            uint8_t payload[MAXIMUM_PACKET_PAYLOAD_SIZE + (sizeof(uint8_t) << 2)];
        } single_package;
    }package;
};



/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static  CLASS(base_object_implement) *list_mgr_obj = NULL;

static int (*error_handle_callback)(uint8_t error_code) = NULL;
 /*for upper layer receive call back use*/
static int (*normal_package_recieve_callback)(uint16_t uuid, char type, char *msg, uint32_t len) = NULL;
static int (*single_package_recieve_callback)(uint16_t uuid, char type, char *msg, uint32_t len) = NULL;
/*ble stack wrtite rsp callback*/
static int (*ble_mac_layer_write_rsp_callback)(void) = NULL;
/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */



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
    memcpy(pkt->package.normal_package.payload, data, lenth);
    /*convert data from little endian to big edian*/
    uint32_t i = htonl(lenth);
    memcpy(pkt->package.normal_package.length, &i, CLING_PACKET_LENTH_SIZE);

    ble_tx_send(uuid, (char *)pkt + sizeof(uuid), sizeof(struct protocol_serial_link_packet) - sizeof(uuid));
#if 0
    {
        char *t = (char *)pkt;
        DEBUG("Single message (22): %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",
              t[0], t[1], t[2], t[3], t[4],
              t[5], t[6], t[7], t[8], t[9]);
    }
#endif
    return LX_OK;
}
#if 0
void _receivedPacketProcess(uint16_t uuid, char type, char *msg, uint32_t len);
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
    memcpy(p.package.normal_package.payload, data, lenth);
    /*FULL FILL payload lenth field*/
    lenth = htonl(lenth);
    memcpy(p.package.normal_package.length, &lenth, CLING_PACKET_LENTH_SIZE);
    // _receivedPacketProcess(uuid, data, lenth);


}
#endif
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
void _receivedPacketProcess(uint16_t uuid, char type, char *msg, uint32_t len)
{


    struct protocol_serial_link_packet t;
    
   if(uuid == UUID_TX_SP){
     DEBUG("UUID_TX_SPPacketProcess: UUID=0x%x lenth=  %d\r\n", uuid, len);
     memcpy(&(t.package.single_package), msg, len);
       /*call registered call back function if existed*/
      if(single_package_recieve_callback != NULL){
          single_package_recieve_callback(uuid, type, (char*)t.package.single_package.payload, len);
      }
   }else{
    /*as we can see data only transfer part of the construction*/
    memcpy(&(t.package.normal_package), msg, len);
    len = t.package.normal_package.length[0];
    len <<= 8;
    len += t.package.normal_package.length[1];
    len <<= 8;
    len += t.package.normal_package.length[2];

    DEBUG("_receivedPacketProcess: UUID=0x%x lenth=  %d\r\n", uuid, len);
       if(normal_package_recieve_callback != NULL){
          normal_package_recieve_callback(uuid, type, (char*)t.package.single_package.payload, len);
      }
   }
}

/*********************************************************************
 * @fn      comm_protocol_mac_send_singlepacket
 *
 * @brief   recieve data funtion called by lower layer to pass data to upper layer
 *
 * @param   uuid :  charactor uuid
 *          data :  data pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
void comm_protocol_mac_send_singlepacket(uint16_t uuid, char type, char *msg, uint32_t len)
{

    struct protocol_serial_link_packet *pkt;
    pkt = (struct protocol_serial_link_packet *)LX_Malloc(sizeof(struct protocol_serial_link_packet));;

    // Indiate it is a starting packet
    pkt->uuid[0] = (uuid >> 8) & 0xff;
    pkt->uuid[1] = uuid & 0xff;
    memcpy(&(pkt->package.single_package.payload), msg, MAXIMUM_PACKET_SIZE);

    DEBUG("wait 1020\r\n");

    ble_tx_send(uuid, (char*)(&(pkt->package.single_package)) , sizeof(pkt->package.single_package));

    DEBUG("post 1020\r\n");

#if 0
    {
        char *data = (char *)pkt;

        DEBUG("Single message (22): %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
              data[0], data[1], data[2], data[3], data[4],
              data[5], data[6], data[7], data[8], data[9]);
    }
#endif
}

/*********************************************************************
 * @fn      comm_protocol_mac_send_singlepacket
 *
 * @brief   recieve data funtion called by lower layer to pass data to upper layer
 *
 * @param   uuid :  charactor uuid
 *          data :  data pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
void comm_protocol_mac_send_normal_package(uint16_t uuid, char type, char *msg, uint32_t len)
{
    struct protocol_serial_link_packet t;
    struct protocol_serial_link_packet *pkt;
    pkt = &t;
    // Indiate it is a starting packet
    pkt->uuid[0] = (uuid >> 8) & 0xff;
    pkt->uuid[1] = uuid & 0xff;
    pkt->package.normal_package.messageId = 0;
    //len = htonl(len);
    //memcpy(pkt->package.normal_package.length, &len, CLING_PACKET_LENTH_SIZE);

    pkt->package.normal_package.length[0] = (len >> 16) & 0xff;
    pkt->package.normal_package.length[1] = (len >> 8) & 0xff;
    pkt->package.normal_package.length[2] = len & 0xff;
    memcpy(pkt->package.normal_package.payload, msg, len);
    DEBUG("wait 1020\r\n");

    ble_tx_send(uuid, (char *)(&(pkt->package.normal_package)), sizeof(pkt->package.normal_package));
#if 1
    {
        char *data = (char *)(pkt->uuid);

        DEBUG(" message (22): %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",
              data[0], data[1], data[2], data[3], data[4],
              data[5], data[6], data[7], data[8], data[9]);
    }
#endif
    DEBUG("post 1020\r\n");

}

/*********************************************************************
 * @fn      ble_stack_mac_layer_write_rsp_call_back
 *
 * @brief   process stack level write rsp call back
 *
 * @param   none
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int ble_stack_mac_layer_write_rsp_call_back(){
    if(ble_mac_layer_write_rsp_callback != NULL){
        ble_mac_layer_write_rsp_callback();
    }
    return  LX_OK;
}

/*********************************************************************
 * @fn      ble_mac_layer_write_rsp_callback_register
 *
 * @brief   register ble statck write rsp callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int ble_stack_mac_layer_write_rsp_callback_register(int (*p_callback)(void)){
    if(p_callback != NULL){
        ble_mac_layer_write_rsp_callback = p_callback;
    }
    return  LX_OK;
}
/*********************************************************************
 * @fn      comm_mac_single_package_recieve_callback_register
 *
 * @brief   register single package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_mac_single_package_recieve_callback_register(int (*p_callback)(uint16_t, char , char* , uint32_t)){
    if(p_callback != NULL){
        single_package_recieve_callback = p_callback;
    }
    return  LX_OK;
}
/*********************************************************************
 * @fn      comm_mac_normal_package_recieve_callback_register
 *
 * @brief  register normal package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_mac_normal_package_recieve_callback_register(int (*p_callback)(uint16_t, char , char* , uint32_t)){
    if(p_callback != NULL){
        normal_package_recieve_callback = p_callback;
    }
    return  LX_OK;
}

/*********************************************************************
 * @fn     comm_link_error_handle_register
 *
 * @brief  register normal package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_mac_error_handle_register(int (*p_callback)(uint8_t))
{
    if(p_callback != NULL) {
        error_handle_callback = p_callback;
    }
    return  LX_OK;
}
/*********************************************************************
 * @fn      cling_comm_mac_init
 *
 * @brief  cling comm mac layer initia function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int cling_comm_mac_init()
{
    //CLASS(base_object_implement) *list_mgr_obj
    NEW(list_mgr_obj, base_object_implement);
    return LX_OK;
}

