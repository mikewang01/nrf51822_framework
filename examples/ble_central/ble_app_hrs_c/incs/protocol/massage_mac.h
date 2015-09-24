#ifndef  __UART_MASSAGE_MAC_H__
#define __UART_MASSAGE_MAC_H__


#include "ets_sys.h"
#include "osapi.h"
#include "C_types.h"
#include "oop_hal.h"


//#define SONGYAN_A8A
#define SONGYAN_A8L

/*package type emurate*/
enum package_type {
    PACKAGE_ACK = 0,
    PACKAGE_DATA,
    PACKAGE_CMD,
    PACKAGE_CMD_ACK,
    PACKAGE_HIGH_FIVEBYTES,
    PACKAGE_LOW_FIVEBYTES


};


#define MASSAGE_MAX_BUFFER 16
struct massage_mac_layer_payload_rev{
	uint8 ppayload[MASSAGE_MAX_BUFFER];/*uart paly load pointer*/
	size_t lenth;/*data lenth*/
	uint8 num; /*current state like wai*/
	struct massage_mac_layer_payload_rev *next;
};

struct massage_mac_layer_payload_send{
	uint8  ppayload[MASSAGE_MAX_BUFFER];	/*uart paly load pointer*/
	size_t lenth;		/*data lenth*/
	uint8 	pakage_type;
	uint8  resend_times;/* recoed times the uart has been sended*/
	uint8 state;		/*current state like wai*/
	struct massage_mac_layer_payload_send *next;
};

/*object prototype declaration*/
DEF_CLASS(cling_protocol_data)
	bool (*init)    	(CLASS(cling_protocol_data) *arg); 			/*initiate uart object*/
	bool (*de_init) 	(CLASS(cling_protocol_data) *arg);				/*delete uart object*/
	bool (*get_data) 	(char *pinf, size_t lenth);/*data send   function*/
	bool (*send_data) 	(char *pinf, size_t lenth);/*data send   function*/
	bool (*enable) 		();/*data send   function*/
	bool (*disable) 	();/*data send   function*/
END_DEF_CLASS(cling_protocol_data)


bool massage_obtain_payload_from_revlist(struct massage_mac_layer_payload_rev **ppayload);
bool massage_mac_send_payload(char *ppayload, size_t lenth, uint8 type);
void set_massage_recieved_cmd_call_back(void (*callback)(char));


#endif

