#ifndef __UART_MASSAGE_CMD_H__
#define __UART_MASSAGE_CMD_H__

#include "ets_sys.h"
#include "osapi.h"
#include "C_types.h"
#include "oop_hal.h"
#include "cling_healthdata_json.h"
/*********************************************************************
* MACROS
*
/*object prototype declaration*/
DEF_CLASS(massage_protocol)
	bool (*init)    	(CLASS(massage_protocol) *arg); 			/*initiate uart object*/
	bool (*de_init) 	(CLASS(massage_protocol) *arg);				/*delete uart object*/
	bool (*send_data) 		(CLASS(massage_protocol) *arg, char *pinf, size_t lenth);/*data send   function*/
	bool (*send_cmd) 		(CLASS(massage_protocol) *arg, char *pinf, size_t lenth);/*data send   function*/
	bool (*send_heart_rate_level)	(CLASS(massage_protocol) *arg, uint8 heartrate);
	bool (*power_on)	(CLASS(massage_protocol) *arg);
	bool (*fatigue_mode_on)	(CLASS(massage_protocol) *arg);
	bool (*fatigue_mode_off)	(CLASS(massage_protocol) *arg);
	bool (*power_off)	(CLASS(massage_protocol) *arg);
	bool (*enable_audio_interaction)	(CLASS(massage_protocol) *arg);
	bool (*disable_audio_interaction)	(CLASS(massage_protocol) *arg);
	
	bool (*notify_user_approach)	(CLASS(massage_protocol) *arg);
	bool (*notify_user_leave)	(CLASS(massage_protocol) *arg);
	
	bool (*enable_recieving) (CLASS(massage_protocol) *arg);
	bool (*disable_recieving) (CLASS(massage_protocol) *arg);
	bool (*task_register) 	(CLASS(massage_protocol) *arg, uint16 task_id);			/*register  object*/
	bool (*recv_callback_register) 	(CLASS(massage_protocol) *arg, void (*callback)(void *pbuffer));/*register  object*/
	void *user_data;/*point to user private data*/
END_DEF_CLASS(massage_protocol)


#endif

