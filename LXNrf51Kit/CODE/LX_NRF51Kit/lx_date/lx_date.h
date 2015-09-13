#ifndef _LX_DATE_H_
#define _LX_DATE_H_

#include <stdint.h>

typedef struct
{
    uint16_t	year;
    uint8_t 	month;
    uint8_t 	day;
    uint8_t 	hour;   
    uint8_t 	minute;
    uint8_t 	second;

}lx_date_t;

int lx_date_get(lx_date_t *);

int lx_date_calibrate(lx_date_t *);

int lx_date_add_one_second(void);

#endif	/* _LX_DATE_H_ */
