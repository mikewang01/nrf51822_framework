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

int cling_comm_hal_init(void);
int comm_protocol_hal_send_packet(char type, uint8_t *msg, uint32_t len);

int comm_hal_normal_package_recieve_callback_register(int (*p_callback)(char* , uint32_t));
int comm_hal_single_package_recieve_callback_register(int (*p_callback)(char* , uint32_t));
int comm_hal_error_handle_register(int (*p_callback)(uint8_t));
#endif // __VERSION_H__


