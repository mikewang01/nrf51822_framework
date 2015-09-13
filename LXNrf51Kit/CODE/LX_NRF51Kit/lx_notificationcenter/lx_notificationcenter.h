#ifndef _LX_NOTIFICATION_CENTER_H_
#define _LX_NOTIFICATION_CENTER_H_

#include "stdint.h"
#include "stdbool.h"

typedef struct
{
	/* message type */
	uint32_t type;

	/* pointer to message data, if exist */
	void *data;
	
	/* message data length */
	uint16_t len;
	
}lx_notification_t;

int lx_notification_center_put(lx_notification_t *msg);

int lx_notification_center_get(lx_notification_t *msg);

bool lx_notification_center_contains(uint32_t type);

int lx_notification_center_remove(uint32_t type);

int lx_notification_center_count(void);

#endif	/* _LX_NOTIFICATION_CENTER_H_ */
