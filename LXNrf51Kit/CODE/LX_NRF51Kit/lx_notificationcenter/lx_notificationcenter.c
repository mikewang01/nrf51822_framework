#include "lx_notificationcenter.h"

#include "lx_nrf51Kit.h"
#include "debug.h"

static LX_QueueType queue;

int lx_notification_center_put(lx_notification_t *msg)
{
	if (NULL == queue)
	{
		if (NULL == (queue = LX_QueueCreate(sizeof(lx_notification_t))))
		{
			DEBUG_ERROR("create queue failed!");
			return -1;
		}

		// DEBUG_LOG("create queue succeed!");
	}

	if (msg == NULL)
	{
		DEBUG_ERROR("msg is NULL!");
		return -1;
	}

	lx_notification_t new_msg;
	
	new_msg.type = msg->type;
	new_msg.len  = msg->len;

	// DEBUG_LOG("msg->type is: %d, msg->len is: %d", msg->type, msg->len);

	if (msg->len > 0)
	{
		if (NULL == (new_msg.data = LX_Malloc(msg->len)))
		{
			DEBUG_ERROR("no memory!");
			return -1;
		}
		memcpy(new_msg.data, msg->data, msg->len);
	}

	if (0 != LX_QueueAppend(queue, (void *)&new_msg))
	{
		DEBUG_ERROR("put msg failed! msg->type is: %d, memory usage is: %d", 
			msg->type, LX_Memusage());
		return -1;
	}
	
	// DEBUG_LOG("put msg succeed!");

	return 0;
}

int lx_notification_center_get(lx_notification_t *msg)
{
	if (NULL == queue)
	{
		return -1;
	}

	if (0 != LX_QueueTakeFirst(queue, (void *)msg))
	{
		DEBUG_ERROR("get msg failed!");
		return -1;
	}

	return 0;
}

bool lx_notification_center_contains(uint32_t type)
{
	if (NULL == queue)
	{
		return false;
	}

	LX_ENTER_CRITICAL();

	for (int i = 0; i < LX_QueueCount(queue); i++)
	{
		lx_notification_t msg;
		
		LX_QueueAt(queue, i, &msg);
		
		if (msg.type == type)
		{
			LX_EXIT_CRITICAL();
			
			return true;
		}
	}

	LX_EXIT_CRITICAL();

	return false;
}

int lx_notification_center_remove(uint32_t type)
{
	if (NULL == queue)
	{
		return -1;
	}

	LX_ENTER_CRITICAL();

	for (int i = 0; i < LX_QueueCount(queue); i++)
	{
		lx_notification_t msg;
		
		LX_QueueAt(queue, i, &msg);
		
		if (msg.type == type)
		{
			if (0 != LX_QueueRemove(queue, i))
			{
				DEBUG_ERROR("remove msg failed!");

				LX_EXIT_CRITICAL();

				return -1;
			}

			LX_EXIT_CRITICAL();

			return 0;
		}
	}

	LX_EXIT_CRITICAL();

	return -1;
}

int lx_notification_center_count(void)
{
	if (queue == NULL)
	{
		return 0;
	}

	return LX_QueueCount(queue);
}

