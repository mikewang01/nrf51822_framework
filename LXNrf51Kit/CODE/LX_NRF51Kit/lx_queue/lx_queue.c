#include "lx_queue.h"
#include "../lx_nrf51Kit.h"


typedef struct lx_queue_item
{
	struct lx_queue_item *pnext;

	void *pdata;

}lx_queue_item;

typedef struct
{	
	lx_queue_item 		*phead;
	uint16_t 			item_size;
	uint8_t 			count;

}lx_queue;


void *lx_queue_create(uint16_t item_size)
{
	if (item_size == 0)
	{
		return NULL;
	}

	LX_ENTER_CRITICAL();

	lx_queue *queue = (lx_queue *)LX_Malloc(sizeof(lx_queue));

	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return NULL;
	}

	queue->phead 		= NULL;
	queue->item_size 	= item_size;
	queue->count 		= 0;

	LX_EXIT_CRITICAL();

	return queue;
}

void lx_queue_destroy(void **q)
{
	LX_ENTER_CRITICAL();

	lx_queue *queue = *((lx_queue **)q);
	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return;
	}

	lx_queue_clear(queue);

	LX_Free(queue);

	*q = NULL;

	LX_EXIT_CRITICAL();
}

int lx_queue_append(void *q, void *item)
{
	LX_ENTER_CRITICAL();

	lx_queue *queue = q;
	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	lx_queue_item *new_item = (lx_queue_item *)LX_Malloc(sizeof(lx_queue_item));
	if (new_item == NULL)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}
	new_item->pnext = NULL;

	new_item->pdata = LX_Malloc(queue->item_size);
	if (new_item->pdata == NULL)
	{
		LX_Free(new_item);
		LX_EXIT_CRITICAL();
		return -1;
	}
	memcpy(new_item->pdata, item, queue->item_size);

	lx_queue_item **p = &queue->phead;
	while (*p != NULL)
	{
		p = &((*p)->pnext);
	} 		
	*p = new_item;

	queue->count++;

	LX_EXIT_CRITICAL();

	return 0;
}

int lx_queue_take_first(void *q, void *item)
{
	LX_ENTER_CRITICAL();

	lx_queue *queue = (lx_queue *)q;
	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	lx_queue_item *first_item = queue->phead;
	if (first_item == NULL)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	queue->phead = queue->phead->pnext;
	queue->count--;

	memcpy(item, first_item->pdata, queue->item_size);
	LX_Free(first_item->pdata);
	LX_Free(first_item);

	LX_EXIT_CRITICAL();

	return 0;
}

int lx_queue_at(void *q, int index, void *item)
{
	LX_ENTER_CRITICAL();

	lx_queue *queue = (lx_queue *)q;
	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	if (queue->count <= index)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	lx_queue_item *index_item = queue->phead;
	if (index_item == NULL)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	for (int i = 0; i < index; i++)
	{
		index_item = index_item->pnext;
		if (index_item == NULL)
		{
			LX_EXIT_CRITICAL();
			return -1;
		}
	}

	memcpy(item, index_item->pdata, queue->item_size);

	LX_EXIT_CRITICAL();

	return 0;
}

int lx_queue_remove(void *q, int index)
{
	LX_ENTER_CRITICAL();

	lx_queue *queue = (lx_queue *)q;
	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	if (queue->count <= index)
	{
		LX_EXIT_CRITICAL();
		return -1;
	}

	lx_queue_item *delete_item;
	if (index == 0)
	{
		delete_item = queue->phead;
		if (delete_item == NULL)
		{
			LX_EXIT_CRITICAL();
			return -1;
		}

		queue->phead = queue->phead->pnext;
		queue->count--;
	}
	else
	{
		lx_queue_item *index_item = queue->phead;
		if (index_item == NULL)
		{
			LX_EXIT_CRITICAL();
			return -1;
		}

		for (int i = 0; i < index - 1; i++)
		{
			index_item = index_item->pnext;
			if (index_item == NULL)
			{
				LX_EXIT_CRITICAL();
				return -1;
			}
		}

		delete_item = index_item->pnext;
		if (delete_item == NULL)
		{
			LX_EXIT_CRITICAL();
			return -1;
		}

		index_item->pnext = index_item->pnext->pnext;
		queue->count--;
	}

	LX_Free(delete_item->pdata);
	LX_Free(delete_item);
	

	LX_EXIT_CRITICAL();

	return 0;
}

uint32_t lx_queue_count(void *q)
{
	LX_ENTER_CRITICAL();

	lx_queue *queue = (lx_queue *)q;
	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return 0;
	}

	int count = queue->count;

	LX_EXIT_CRITICAL();

	return count;
}

void lx_queue_clear(void *q)
{
	LX_ENTER_CRITICAL();

	lx_queue *queue = (lx_queue *)q;
	if (queue == NULL)
	{
		LX_EXIT_CRITICAL();
		return;
	}

	lx_queue_item *first_item;
	while (queue->phead != NULL)
	{
		first_item = queue->phead;
		queue->phead = queue->phead->pnext;
		LX_Free(first_item->pdata);
		LX_Free(first_item);
	}

	queue->count = 0;

	LX_EXIT_CRITICAL();
}


