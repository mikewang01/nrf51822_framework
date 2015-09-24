#ifndef  __UART_PROTOCAL_MAC_H__
#define __UART_PROTOCAL_MAC_H__


#include "ets_sys.h"
#include "osapi.h"
#include "C_types.h"
#include "oop_hal.h"



struct mac_layer_payload_rev{
	uint8 *ppayload;/*uart paly load pointer*/
	size_t lenth;/*data lenth*/
	uint8 num; /*current state like wai*/
	struct mac_layer_payload_rev *next;
};


struct mac_layer_payload_send{
	uint8  *ppayload;	/*uart paly load pointer*/
	size_t lenth;		/*data lenth*/
	uint8  resend_times;/* recoed times the uart has been sended*/
	uint8 state;		/*current state like wai*/
	struct mac_layer_payload_send *next;
};

/*object prototype declaration*/
DEF_CLASS(uart_implement)
	bool (*init)    	(CLASS(uart_implement) *arg); 			/*initiate uart object*/
	bool (*de_init) 	(CLASS(uart_implement) *arg);				/*delete uart object*/
	bool (*get_data) 	(char *pinf, size_t lenth);/*data send   function*/
	bool (*send_data) 	(char *pinf, size_t lenth);/*data send   function*/
	bool (*enable) 		();/*data send   function*/
	bool (*disable) 	();/*data send   function*/
END_DEF_CLASS(uart_implement)


bool obtain_payload_from_revlist(struct mac_layer_payload_rev **ppayload);
bool mac_send_payload(char *ppayload, size_t lenth);
void set_recieved_cmd_call_back(void (*callback)(char));
bool init_protocol_mac_layer();


#endif

