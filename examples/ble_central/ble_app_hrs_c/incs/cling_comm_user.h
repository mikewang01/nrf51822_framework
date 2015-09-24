/***************************************************************
 * Name:      __CLING_COMM_CMD_H__
 * Purpose:   code for lenth sample  to store and send
 * Author:    mikewang(s)
 * Created:   2014-06-12
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */

#ifndef __CLING_COMM_USER_H__
#define __CLING_COMM_USER_H__
#include "oop_hal.h"
/* Exported constants --------------------------------------------------------*/

/**
 * Weather type for setting peripheral weather forecast information.
 */
typedef enum {
    PERIPHERAL_WEATHER_SUNNY=0,
    PERIPHERAL_WEATHER_CLOUDY,
    PERIPHERAL_WEATHER_RAINY,
    PERIPHERAL_WEATHER_SNOWY,
    PERIPHERAL_WEATHER_MAX,
}peripheral_weather_type_t;
    
/* Exported types ------------------------------------------------------------*/
// ----------------------------------
// Control register definition
DEF_CLASS(cling_comm_controller)
int (*init)(CLASS(cling_comm_controller)*);
int (*de_init)(CLASS(cling_comm_controller)*);

/*weather related function*/
int (*add_weather)(CLASS(cling_comm_controller)* arg, uint8_t month, uint8_t day, peripheral_weather_type_t weather_type, uint8_t low_temperature, uint8_t high_temperature);
int (*set_weather)(CLASS(cling_comm_controller)* arg);
int (*get_weather_buffer_left)(CLASS(cling_comm_controller)* arg, uint16_t *buffer_size);
/*time set related*/
int (*set_time)(CLASS(cling_comm_controller)* arg, uint32_t utc_time);

int (*register_normal_package_process)(CLASS(cling_comm_controller)* arg, int(*p_callback)(uint8_t *data, uint16_t lenth));
int (*register_stream_package_process)(CLASS(cling_comm_controller)* arg, int(*p_callback)(uint8_t *data, uint16_t lenth));
int (*register_error_handle_process)(CLASS(cling_comm_controller)* arg, int(*p_callback)(uint8_t error_code));
int (*register_task_id)(CLASS(cling_comm_controller)* arg, uint16_t task_id);
int (*reboot)(CLASS(cling_comm_controller)* arg);
int (*device_config)(CLASS(cling_comm_controller)* arg, uint8_t *data, uint16_t lenth);
int (*set_smart_notification)(CLASS(cling_comm_controller)* arg, char* data, uint16_t lenth );
int (*set_ancs)(CLASS(cling_comm_controller)* arg, char* data, uint16_t lenth );
int (*load_device_info)(CLASS(cling_comm_controller)* arg);
void* user_data;
END_DEF_CLASS(cling_comm_controller)

#endif // __VERSION_H__


