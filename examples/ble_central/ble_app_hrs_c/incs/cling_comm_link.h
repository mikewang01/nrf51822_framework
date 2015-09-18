/***************************************************************
 * Name:      __CLING_COMM_LINK_H__
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-06-12
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */

#ifndef __CLING_COMM_LINK_H__
#define __CLING_COMM_LINK_H__

/* Exported constants --------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/

int cling_comm_logic_link_init();

int logic_link_send_single_package(uint8_t *msg, uint16_t lenth);
int logic_link_send_normal_package(uint8_t *msg, uint16_t lenth, uint8_t need_ack);

int comm_link_error_handle_register(int (*p_callback)(uint8_t));
int comm_link_normal_package_recieve_callback_register(int (*p_callback)(char* , uint32_t));
int comm_link_single_package_recieve_callback_register(int (*p_callback)(char* , uint32_t));
#endif // __VERSION_H__


