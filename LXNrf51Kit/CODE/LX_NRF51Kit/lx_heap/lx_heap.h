#ifndef _LX_HEAP_H_
#define _LX_HEAP_H_

#include <stdint.h>

void *lx_malloc(uint32_t size);

void lx_free(void *p);

uint32_t lx_memusage(void);

#endif	/* _LX_HEAP_H_ */
