#ifndef _LX_FIFO_H_
#define _LX_FIFO_H_

#include <stdint.h>
#include <stdbool.h>

void *lx_fifo_create(uint16_t size);

void lx_fifo_destroy(void **fifo);

int lx_fifo_write(void *fifo, const uint8_t *buf, uint16_t len);

int lx_fifo_read(void *fifo, uint8_t *buf, uint16_t len);

int lx_fifo_count(void *fifo);

bool lx_fifo_is_empty(void *fifo);

bool lx_fifo_is_full(void *fifo);

void lx_fifo_flush(void *fifo);


#endif /* _LX_FIFO_H_ */

