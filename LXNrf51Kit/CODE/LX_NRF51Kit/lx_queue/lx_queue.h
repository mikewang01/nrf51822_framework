#ifndef _lx_queue_H_
#define _lx_queue_H_

#include <stdint.h>

void *lx_queue_create(uint16_t item_size);

void lx_queue_destroy(void **queue);

int lx_queue_append(void *queue, void *item);

int lx_queue_take_first(void *queue, void *item);

int lx_queue_at(void *queue, int index, void *item);

int lx_queue_remove(void *queue, int index);

uint32_t lx_queue_count(void *queue);

void lx_queue_clear(void *queue);


#endif	/* _lx_queue_H_ */
