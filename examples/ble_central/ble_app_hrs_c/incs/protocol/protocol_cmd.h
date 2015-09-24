#ifndef __UART_PROTOCAL_CMD_H__
#define __UART_PROTOCAL_CMD_H__

#include "ets_sys.h"
#include "osapi.h"
#include "C_types.h"
#include "oop_hal.h"
#include "cling_healthdata_json.h"
/*INIDICATE DIFFRENT IN CMD LAYER*/
enum uart_cmd_package_type{
	CMD_LOCATION_TYPE,
	CMD_HEALTH_TYPE	
};

/*********************************************************************
* MACROS
*/
/*to be compatible with old uart protocal*/
#define  UART_OLD_PROTOCAL

#define  CLING_ID_LENTH 4
#define  CLING_MAC_LENTH 6
#define  CLING_RSSI_LENTH 1
#define  CLING_CMD_TYPE_LENTH 1

/*information uploaded*/
struct cling_inf_rev{
#ifndef UART_OLD_PROTOCAL
	char package_type;/*indicate current pakage type*/
#endif
	char cling_id[CLING_ID_LENTH]; /*cling device id*/
	char cling_mac[CLING_MAC_LENTH];/*cling mac adress*/
	char cling_rssi[CLING_RSSI_LENTH];/*cling rssi number*/
};

struct cling_inf_send{
#ifndef UART_OLD_PROTOCAL
	char package_type;/*indicate current pakage type*/
#endif
	char *ptr;
};

#pragma pack(push) 
#pragma pack(1)	
/*user health information related*/
struct cling_health_rev{
#ifndef UART_OLD_PROTOCAL
	char package_type;/*indicate current pakage type*/
#endif
	char cling_mac[CLING_MAC_LENTH];/*cling mac adress*/
	uint8 null_align; /*for align used*/
	uint8  date;   /*the time when cling sendout health data */  
	uint16 steps;
	uint16 distance;
	uint16 calories;
	uint16 sleep_time;
	uint8  heart_rate;
	uint8  skin_temp;

	//char health_data[sizeof(struct health_data_inf)];
};
#pragma pack(pop) 

struct cling_health_send{
	
};



/*uart protocal cmd layer package*/
struct cling_cmd_rev{
	char package_type;
	char ppackage[1];
};


/*object prototype declaration*/
DEF_CLASS(cling_protocol)
	bool (*init)    	(CLASS(cling_protocol) *arg); 			/*initiate uart object*/
	bool (*de_init) 	(CLASS(cling_protocol) *arg);				/*delete uart object*/
	bool (*send_data) 		(CLASS(cling_protocol) *arg, char *pinf, size_t lenth);/*data send   function*/
	bool (*send_cmd) 		(CLASS(cling_protocol) *arg, char *pinf, size_t lenth);/*data send   function*/
	bool (*enable_recieving) (CLASS(cling_protocol) *arg);
	bool (*disable_recieving) (CLASS(cling_protocol) *arg);
	bool (*task_register) 	(CLASS(cling_protocol) *arg, uint16 task_id);			/*register  object*/
	bool (*recv_callback_register) 	(CLASS(cling_protocol) *arg, void (*callback)(void *pbuffer));/*register  object*/
	void *user_data;/*point to user private data*/
END_DEF_CLASS(cling_protocol)

#endif

