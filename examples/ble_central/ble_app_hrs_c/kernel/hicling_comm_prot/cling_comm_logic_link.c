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
#include "cling_comm_link.h"
#include "base_object.h"
#include "../../lx_nrf51Kit.h"
#include <string.h>
/*********************************************************************
 * TYPEDEFS
 */
#if 0
enum {
    NORMAL_PACKAGE,
    SINGLE_PACKAGE
};
#endif

enum {
    LOGIC_LINK_WAIT_FOR_SEND = 0,
    LOGIC_LINK_WAIT_FOR_ACK,
};

struct logic_link_package {
    struct    base_object parent;
    uint16_t  type;  /*single package or normal package*/
    uint16_t  package_id;/*used to identify the packages after being disassemble*/
    uint16_t  send_channel_uuid;
    uint8_t   need_ack;
    uint8_t   package_state;
    uint16_t  lenth;/**/
    uint8_t   pay_load[MAXIMUM_PACKET_PAYLOAD_SIZE];
};
// Timing parameters
// 300 ms RX timeout, in case of packet loss, a NACK will be sent out in 300 ms
#define PACKET_RX_TIMEOUT					5000
// 600 ms TX timeout, in case of no response from the receiver in 600 ms, the transmission
// is terminated, treated as "FAIL".
#define PACKET_TX_TIMEOUT				1000

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */

static struct	base_object *mac_send_list_header = NULL;
static struct	base_object *mac_rev_list_header = NULL;
static CLASS(base_object_implement) *list_mgr_obj =  NULL;
static struct logic_link_package *logic_link_layer_package_sending = NULL;
static struct logic_link_package *logic_link_layer_package_receving = NULL;
static int m_logic_link_send_timer_id = 0;
static int m_logic_link_recieve_timer_id = 0;
/*used to mark package sequnce*/
static  uint16_t package_mark = 0;
DECLARE_MUTEX(logic_link_mutex);

static int (*error_handle_callback)(uint8_t error_code) = NULL;
static int (*single_package_recieve_callback)(char *msg, uint32_t len);
static int (*normal_package_recieve_callback)(char *msg, uint32_t len);
/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */
static int ble_stack_logic_link_app_ack_process(void);
static int send_packet_list_demon(void * p_context);
static void logic_link_send_ack_packet(void);
/*********************************************************************
 * @fn      logic_link_send_ack_packet
 *
 * @brief   logic link layer used to add single data into send list
 *
 * @param   msg :  msg pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
static void logic_link_send_ack_packet( )
{
    // Update the message id for ACK message
    uint8_t msg = PROTOCOL_MESSAGE_ACK;
    logic_link_send_normal_package(&msg, 1, false);
}

/*********************************************************************
 * @fn      logic_link_send_single_package
 *
 * @brief   logic link layer used to add single data into send list
 *
 * @param   msg :  msg pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int logic_link_send_single_package(uint8_t *msg, uint16_t lenth)
{
    ASSERT(lenth <= MAXIMUM_PACKET_PAYLOAD_SIZE);
    struct logic_link_package *pkt = NULL;
    pkt = (struct logic_link_package *)LX_Malloc(sizeof(struct logic_link_package));
    ASSERT(pkt != NULL);
    pkt->lenth = lenth;
    pkt->type = SINGLE_PACKAGE;
    memcpy(pkt->pay_load, msg, lenth);
    // struct base_object **object_list , struct base_object *obj, enum os_object_class_type class_type , const char *name);
    return list_mgr_obj->add(list_mgr_obj, &mac_send_list_header, (struct base_object *)(pkt), OS_Object_Class_Mgr, "link");
}


/*********************************************************************
 * @fn      logic_link_send_normal_package
 *
 * @brief   logic link layer used to add normal data into send list
 *
 * @param   msg :  msg pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */

int logic_link_send_normal_package(uint8_t *msg, uint16_t lenth, uint8_t need_ack)
{
    uint16_t pkt_number = 0;
    uint16_t positon = 0;
    /*the package number that is gonna splited*/
    pkt_number = (uint16_t)(lenth / MAXIMUM_PACKET_PAYLOAD_SIZE);
    if (lenth % MAXIMUM_PACKET_PAYLOAD_SIZE) {
        pkt_number ++;
    }
#define _START_COUNT_ 0
#define __APP_ACK_UP_THREHOLD_ 7
    int i = 0;
    int ret = LX_ERROR;
    int package_ack_count = __APP_ACK_UP_THREHOLD_;
    /*imcrese the maek varires of package number*/
    package_mark++;
    for(i = _START_COUNT_; i < pkt_number + _START_COUNT_; i++) {
        struct logic_link_package *pkt = (struct logic_link_package *)LX_Malloc(sizeof(struct logic_link_package));
        ASSERT(pkt != NULL);
        pkt->need_ack = false;
        /*this area means the package lenth taht has been sended*/
        pkt->lenth = i*MAXIMUM_PACKET_PAYLOAD_SIZE;
        /*this is the first package*/
        if(i == _START_COUNT_) {
            pkt->send_channel_uuid = (uint16_t)UUID_RX_START;
            pkt->lenth = lenth;
        } else if(i == pkt_number + _START_COUNT_ - 1) {
            /*this is the last package data*/
            pkt->send_channel_uuid = (uint16_t)UUID_RX_END;
            if(need_ack == true) {
                pkt->need_ack = true;
            }
        } else {
            /*this is the middle package data*/
            if(need_ack == true && i >= package_ack_count) {
                package_ack_count += __APP_ACK_UP_THREHOLD_;
                pkt->need_ack = true;
            }
            pkt->send_channel_uuid = (uint16_t)UUID_RX_MIDDLE;
        }
        /*used to identify which group these spltted pachage blonged to */
        pkt->package_id = package_mark;
        /*indicate package state  wait for send or wait for ack*/
        pkt->package_state = LOGIC_LINK_WAIT_FOR_SEND;
        pkt->type = (uint16_t)NORMAL_PACKAGE;
        memcpy(pkt->pay_load, msg + (i * MAXIMUM_PACKET_PAYLOAD_SIZE), lenth);
        /*add message to */
        // logic_link_layer_package_sending = pkt;
#if 1
        MUTEX_WAIT(logic_link_mutex);
        ret = list_mgr_obj->add(list_mgr_obj, &mac_send_list_header, (struct base_object *)(pkt), OS_Object_Class_Mgr, "l");
        MUTEX_POST(logic_link_mutex);
    }
    if(ret == LX_OK && logic_link_layer_package_sending == NULL) {
        DEBUG("ist_mgr_o bj->add(list_mgr_obj\r\n");
        send_packet_list_demon(NULL);
        TIMER_START(m_logic_link_send_timer_id, PACKET_TX_TIMEOUT, NULL);
        return LX_OK;
    }
#endif
    return LX_OK;
}

/*********************************************************************
 * @fn      send_packet_list_demon
 *
 * @brief   logic link layer used to add normal data into send list
 *
 * @param   msg :  msg pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
static int send_packet_list_demon(void * p_context)
{
    struct	base_object *t = NULL;
    int ret =  LX_ERROR;
    DEBUG("[%s]:\r\n", __FUNCTION__);
    if(logic_link_layer_package_sending == NULL) {
        DEBUG("[%s]:%s \r\n", __FUNCTION__, "NULL");
        /*pop one element from list and add it to send pointer*/
        /*use mutex to protect the saftety of shared data*/
        MUTEX_WAIT(logic_link_mutex);
        ret = list_mgr_obj->pop(list_mgr_obj, &mac_send_list_header, &t);
        MUTEX_POST(logic_link_mutex);
        if(ret == LX_OK) {
            /*add new element to sending buffer*/
            logic_link_layer_package_sending = (struct logic_link_package *)t;

        } else {
            /*this means there is no more data stored in this chain so stop timer to save chip resoucrse*/
            TIMER_STOP(m_logic_link_send_timer_id);
        }
    } else {
        DEBUG("[%s]:%s \r\n", __FUNCTION__, "OK");
        /* if this package is still not sended ,resend it*/
        if(logic_link_layer_package_sending->package_state == LOGIC_LINK_WAIT_FOR_SEND) {
            DEBUG("[%s]:%s \r\n", __FUNCTION__, "LOGIC_LINK_WAIT_FOR_SEND");
            ret = LX_OK;

        } else if(logic_link_layer_package_sending->package_state == LOGIC_LINK_WAIT_FOR_ACK) {
            DEBUG("[%s]:%s \r\n", __FUNCTION__, "LOGIC_LINK_WAIT_FOR_ACK");
            uint16_t package_id = logic_link_layer_package_sending->package_id;
            MUTEX_WAIT(logic_link_mutex);
            LX_Free(logic_link_layer_package_sending);
            logic_link_layer_package_sending = NULL;
            /* ack package is not recieved int time for the packaged hooked in sendding chain, then abdond this data*/
            ret = list_mgr_obj->pop(list_mgr_obj, &mac_send_list_header, &t);
            /*find other package in the same group and release them ,even one is sended failed*/
            if(ret == LX_OK) {
                logic_link_layer_package_sending = (struct logic_link_package *)t;
                while(package_id == logic_link_layer_package_sending->package_id && ret == LX_OK) {
                    LX_Free(logic_link_layer_package_sending);
                    logic_link_layer_package_sending = NULL;
                    ret = list_mgr_obj->pop(list_mgr_obj, &mac_send_list_header, &t);
                    if(ret == LX_OK) {
                        logic_link_layer_package_sending = (struct logic_link_package *)t;
                    }
                }
            }
            MUTEX_POST(logic_link_mutex);
            if(ret == LX_ERROR) {
                /*this means there is no more data stored in this chain so stop timer to save chip resoucrse*/
                TIMER_STOP(m_logic_link_send_timer_id);
            }
            /*call user error hanlder*/
            if(error_handle_callback != NULL) {
                error_handle_callback(COMM_PROTOCOL_ERROR_TX_FAILED);
            }
        }

    }

    if( ret == LX_OK) {
        /*after package sended, change state of this pac*/
        logic_link_layer_package_sending->package_state = LOGIC_LINK_WAIT_FOR_ACK;
        /*this is a  single process package*/
        if(logic_link_layer_package_sending->type == SINGLE_PACKAGE) {
            comm_protocol_mac_send_singlepacket(logic_link_layer_package_sending->send_channel_uuid,
                                                logic_link_layer_package_sending->type,
                                                (char*)logic_link_layer_package_sending->pay_load,
                                                logic_link_layer_package_sending->lenth);

        } else if(logic_link_layer_package_sending->type == NORMAL_PACKAGE) {
            comm_protocol_mac_send_normal_package(logic_link_layer_package_sending->send_channel_uuid,
                                                  logic_link_layer_package_sending->type,
                                                  (char*)logic_link_layer_package_sending->pay_load,
                                                  logic_link_layer_package_sending->lenth);

        }


    }

    return LX_OK;
}
/*==============================
type for single package
  PROTOCOL_MESSAGE_STREAMING_SECOND
  PROTOCOL_MESSAGE_STREAMING_MINUTE
  PROTOCOL_MESSAGE_STREAMING_MULTI_MINUTES
  PROTOCOL_MESSAGE_STREAMING_DAY
=============================================*/
/*********************************************************************
 * @fn      send_packet_list_demon
 *
 * @brief   logic link layer used to add normal data into send list
 *
 * @param   msg :  msg pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_logic_link_recevive_single_package_call_back(uint16_t uuid, char type, char *msg, uint32_t len)
{
    struct rev_logic_link_single_package {
        uint8_t type;
        uint8_t pay_load[MAXIMUM_PACKET_PAYLOAD_SIZE];
    };
    struct rev_logic_link_single_package *t = (struct rev_logic_link_single_package*)msg;
    switch(t->type) {
        case PROTOCOL_MESSAGE_STREAMING_SECOND:
        case PROTOCOL_MESSAGE_STREAMING_MINUTE:
        case PROTOCOL_MESSAGE_STREAMING_MULTI_MINUTES:
        case PROTOCOL_MESSAGE_STREAMING_DAY:
            break;

    }
    /*call user callback function*/
    if(single_package_recieve_callback != NULL) {
        single_package_recieve_callback(msg, len);
    }
    return LX_OK;
}
/*********************************************************************
 * @fn      send_packet_list_demon
 *
 * @brief   logic link layer used to add normal data into send list
 *
 * @param   msg :  msg pointer
 *          lenth : data lenth
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
struct rev_logic_link_normal_package {
    union {
        struct {
            /*for ack package*/
            uint8_t type;
            // Payload - 16 uint8_ts
            uint8_t payload[MAXIMUM_PACKET_PAYLOAD_SIZE - sizeof(uint8_t)];
        } normal_package;
        /*when no ack package is revieved the erea is valid*/
        uint8_t payload[MAXIMUM_PACKET_PAYLOAD_SIZE];
    } package;
};

//===============================================================================
//================================================================================


/*********************************************************************
 * @fn      comm_logic_link_recevive_normal_package_call_back
 *
 * @brief   uint16_t uuid, char type, char *msg, uint32_t len
 *
 * @param   task_state pointer ,msg pointer
 *
 * @return  message numbers
 */
//len: first package means the total lenth of the data
//     after second package it means the data has been transfered

#define LOGINC_LINK_REC_BUFFER_SIZE  128
static char rev_buffer[LOGINC_LINK_REC_BUFFER_SIZE];
static uint16_t positon = 0;
static uint16_t total_lenth = LOGINC_LINK_REC_BUFFER_SIZE;
int comm_logic_link_recevive_normal_package_call_back(uint16_t uuid, char type, char *msg, uint32_t len)
{
    if(uuid == UUID_TX_START) {
        struct rev_logic_link_normal_package *t = (struct rev_logic_link_normal_package *)msg;
        if(t->package.normal_package.type == PROTOCOL_MESSAGE_ACK) {
            ble_stack_logic_link_app_ack_process();
        } else {
            if(logic_link_layer_package_receving == NULL) {
                /*once start package recieved. reset state machine*/
                positon = 0;
                total_lenth = len;
                LX_Memcpy(rev_buffer + positon, msg,  MAXIMUM_PACKET_PAYLOAD_SIZE);
                positon += MAXIMUM_PACKET_PAYLOAD_SIZE;
                // struct logic_link_package *pkt = (struct logic_link_package *)LX_Malloc(sizeof(struct logic_link_package));
            }
            TIMER_STOP(m_logic_link_recieve_timer_id);
            /*after first pckage revecied, start recevie timeout function*/
            TIMER_START(m_logic_link_recieve_timer_id, PACKET_RX_TIMEOUT, NULL);
        }
    } else if(uuid == UUID_TX_MIDDLE) {
        if(positon == 0) {
            return LX_ERROR;
        }
        LX_Memcpy(rev_buffer + positon, msg,  MAXIMUM_PACKET_PAYLOAD_SIZE);
        positon += (MAXIMUM_PACKET_PAYLOAD_SIZE);
        TIMER_STOP(m_logic_link_recieve_timer_id);
        /*after first pckage revecied, start recevie timeout function*/
        TIMER_START(m_logic_link_recieve_timer_id, PACKET_RX_TIMEOUT, NULL);

    } else if(uuid == UUID_TX_END) {
        if(positon == 0) {
            return LX_ERROR;
        }
        LX_Memcpy(rev_buffer + positon, msg,  MAXIMUM_PACKET_PAYLOAD_SIZE);
        positon += MAXIMUM_PACKET_PAYLOAD_SIZE;
        /*check  if this is the package revece sucessfully*/
        if(positon >= total_lenth) {
            /*IF RECIEVE WHOLE PACKAGE SUCESSFULLY STOP TIME OUT FUNCTIOIN*/
            TIMER_STOP(m_logic_link_recieve_timer_id);
#if 0
            int i = 0;
            for (i = 0; i < positon; i++) {
                DEBUG("%c ", rev_buffer[i]);
            }
#endif
            /*recieved sucessfully*/
            if(normal_package_recieve_callback != NULL) {
                normal_package_recieve_callback(rev_buffer, total_lenth);
            }
            total_lenth = LOGINC_LINK_REC_BUFFER_SIZE;
            positon = 0;

        } else {
            TIMER_STOP(m_logic_link_recieve_timer_id);
            /*some package among the group has been missed*/
            /*
             call user layer call back function

            */
            if(error_handle_callback != NULL) {
                error_handle_callback(COMM_PROTOCOL_ERROR_RX_FAILED);
            }
            DEBUG("[%s]: package recieve error\r\n", __FUNCTION__);
            /*call user callback function*/
            total_lenth = LOGINC_LINK_REC_BUFFER_SIZE;
            positon = 0;
        }


    }
    return LX_OK;
}

static void recieve_packet_timeout_process(void * p_context)
{
    DEBUG("[%s]: package recieve error\r\n", __FUNCTION__);
    total_lenth = LOGINC_LINK_REC_BUFFER_SIZE;
    positon = 0;
    if(error_handle_callback != NULL) {
        error_handle_callback(COMM_PROTOCOL_ERROR_RX_FAILED);
    }
}

/*********************************************************************
 * @fn      ble_stack_write_rsp_logic_link_call_back
 *
 * @brief   logic layer stack write resp call back
 *
 * @param   none
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
static int ble_stack_logic_link_write_rsp_call_back()
{
#if 1
    if(logic_link_layer_package_sending != NULL) {
        /*validate this is a package need app ack*/
        if(logic_link_layer_package_sending->need_ack == false && logic_link_layer_package_sending->package_state == LOGIC_LINK_WAIT_FOR_ACK) {
            /*stop timer in case strive for one resouce for  ac process and list demon process*/
            TIMER_STOP(m_logic_link_send_timer_id);
            LX_Free(logic_link_layer_package_sending);
            logic_link_layer_package_sending = NULL;
            struct	base_object *t = NULL;
            DEBUG("[%s]:\r\n", __FUNCTION__);
            /*use mutex to protect shared data*/
            MUTEX_WAIT(logic_link_mutex);
            int ret = list_mgr_obj->pop(list_mgr_obj, &mac_send_list_header, &t);
            MUTEX_POST(logic_link_mutex);
            /*if there is some other data waited to be sended in the list*/
            if(ret == LX_OK) {
                DEBUG("[%s]:%s\r\n", __FUNCTION__, "more data existed");
                /*add new element to sending buffer*/
                logic_link_layer_package_sending = (struct logic_link_package *)t;
                send_packet_list_demon(NULL);
                /*restart timer to check out ack time out*/
                TIMER_START(m_logic_link_send_timer_id, PACKET_TX_TIMEOUT, NULL);
            } else {
                DEBUG("[%s]:%s\r\n", __FUNCTION__, "no more data existed");
            }

        }
    }
#endif
    return LX_OK;
}
/*********************************************************************
 * @fn      ble_stack_logic_link_app_ack_process
 *
 * @brief   logic layer app ack call back
 *
 * @param   none
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
static int ble_stack_logic_link_app_ack_process()
{
    DEBUG("[%s]: ACK PACKAGE\r\n", __FUNCTION__);
    /*this is a ack package*/
    if(logic_link_layer_package_sending != NULL) {
        /*validate this is a package need app ack*/
        if(logic_link_layer_package_sending->need_ack == true) {
            DEBUG("[%s]:  package need ACK existed\r\n", __FUNCTION__);
            struct	base_object *t = NULL;
            /*free the data that has been succesfully acked*/
            LX_Free(logic_link_layer_package_sending);
            logic_link_layer_package_sending = NULL;
            /*stop timer for this routine so as to  save cpu resource*/
            TIMER_STOP(m_logic_link_send_timer_id);
            /*use mutex to protect shared data*/
            MUTEX_WAIT(logic_link_mutex);
            int ret = list_mgr_obj->pop(list_mgr_obj, &mac_send_list_header, &t);
            MUTEX_POST(logic_link_mutex);

            /*if there is some other data waited to be sended in the list*/
            if(ret == LX_OK) {
                /*add new element to sending buffer*/
                logic_link_layer_package_sending = (struct logic_link_package *)t;
                //send_packet_list_demon(NULL);
                /*once a new package sended timeout timer started to count down*/
                TIMER_START(m_logic_link_send_timer_id, PACKET_TX_TIMEOUT, NULL);

            }
        }
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
int comm_link_error_handler(uint8_t error_code)
{
    if(error_handle_callback != NULL) {
        error_handle_callback(error_code);
    }
    return LX_OK;
}
/*********************************************************************
 * @fn      comm_mac_single_package_recieve_callback_register
 *
 * @brief   register single package recieve callback function
 *
* @param   p_callback: callback function pointer
 * @return  LX_OK : sucessfully  LX_ERROR: ERROR
 */
int comm_link_single_package_recieve_callback_register(int (*p_callback)(char* , uint32_t))
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
int comm_link_normal_package_recieve_callback_register(int (*p_callback)(char* , uint32_t))
{
    if(p_callback != NULL) {
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
int comm_link_error_handle_register(int (*p_callback)(uint8_t))
{
    if(p_callback != NULL) {
        error_handle_callback = p_callback;
    }
    return  LX_OK;
}


/*********************************************************************
 * @fn      cling_comm_logic_link_init
 *
 * @brief   This function is used for task to get message and message number.
 *
 * @param   task_state pointer ,msg pointer
 *
 * @return  message numbers
 */
int cling_comm_logic_link_init()
{
    //CLASS(base_object_implement) *list_mgr_obj
    NEW(list_mgr_obj, base_object_implement);
    cling_comm_mac_init();
    comm_mac_single_package_recieve_callback_register(comm_logic_link_recevive_single_package_call_back);
    comm_mac_normal_package_recieve_callback_register(comm_logic_link_recevive_normal_package_call_back);
    ble_stack_mac_layer_write_rsp_callback_register(ble_stack_logic_link_write_rsp_call_back);
    comm_mac_error_handle_register(comm_link_error_handler);

    TIMER_INSTALL(&m_logic_link_send_timer_id,  send_packet_list_demon);
    TIMER_INSTALL(&m_logic_link_recieve_timer_id , recieve_packet_timeout_process);
    return LX_OK;
}
