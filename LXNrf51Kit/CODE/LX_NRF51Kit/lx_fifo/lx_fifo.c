#include "lx_fifo.h"
#include "../lx_nrf51Kit.h"


typedef struct
{
    uint8_t           *pbuf;
    uint16_t           size;
    volatile uint16_t  read_pos;
    volatile uint16_t  write_pos;
    volatile uint8_t   carry;

}lx_fifo;

#define FIFO_COUNT(fifo)   ((fifo->carry ? fifo->size : 0) + fifo->write_pos - fifo->read_pos)

void *lx_fifo_create(uint16_t size)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = (lx_fifo *)LX_Malloc(sizeof(lx_fifo));

    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return NULL;
    }

    fifo->pbuf = (uint8_t *)LX_Malloc(size);
    if (fifo->pbuf == NULL)
    {
        LX_Free(fifo);
        LX_EXIT_CRITICAL();
        return NULL;
    }

    fifo->size          = size;
    fifo->read_pos      = 0;
    fifo->write_pos     = 0;
    fifo->carry         = 0;

    LX_EXIT_CRITICAL();
    return fifo;
}

void lx_fifo_destroy(void **f)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = *((lx_fifo **)f);

    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return;
    }

    LX_Free(fifo->pbuf);
    LX_Free(fifo);

    *f = NULL;

    LX_EXIT_CRITICAL();
}


int lx_fifo_write(void *f, const uint8_t *buf, uint16_t len)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = (lx_fifo *)f;

    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return -1;
    }

    if (FIFO_COUNT(fifo) + len > fifo->size)
    {
        LX_EXIT_CRITICAL();
        return -1;
    }

    for (int i = 0; i < len; i++)
    {
        fifo->pbuf[fifo->write_pos] = buf[i];
        fifo->write_pos++;
        if (fifo->write_pos == fifo->size)
        {
            fifo->write_pos = 0;
            fifo->carry = 1;
        }
    }
    
    LX_EXIT_CRITICAL();

    return 0;
}


int lx_fifo_read(void *f, uint8_t *buf, uint16_t len)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = (lx_fifo *)f;
    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return -1;
    }

    if (FIFO_COUNT(fifo) <= 0)
    {
        LX_EXIT_CRITICAL();
        return 0;
    }

    uint16_t readLen = len < FIFO_COUNT(fifo) ? len : FIFO_COUNT(fifo); 

    for (int i = 0; i < readLen; i++)
    {
        buf[i] = fifo->pbuf[fifo->read_pos];
        fifo->read_pos++;
        if (fifo->read_pos == fifo->size)
        {
            fifo->read_pos = 0;
            fifo->carry = 0;
        }
    }
        
    LX_EXIT_CRITICAL();
    
    return readLen;    
}

int lx_fifo_count(void *f)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = (lx_fifo *)f;

    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return -1;
    }

    uint16_t count = FIFO_COUNT(fifo);

    LX_EXIT_CRITICAL();

    return count;
}

bool lx_fifo_is_empty(void *f)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = (lx_fifo *)f;

    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return true;
    }

    uint16_t count = FIFO_COUNT(fifo);

    LX_EXIT_CRITICAL();

    return count == 0;
}

bool lx_fifo_is_full(void *f)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = (lx_fifo *)f;

    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return false;
    }

    uint16_t count = FIFO_COUNT(fifo);

    LX_EXIT_CRITICAL();

    return count == fifo->size;
}


void lx_fifo_flush(void *f)
{
    LX_ENTER_CRITICAL();

    lx_fifo *fifo = (lx_fifo *)f;

    if (fifo == NULL)
    {
        LX_EXIT_CRITICAL();
        return;
    }

    fifo->read_pos = fifo->write_pos;
    fifo->carry = 0;

    LX_EXIT_CRITICAL();
}
