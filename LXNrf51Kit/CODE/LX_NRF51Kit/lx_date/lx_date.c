#include "lx_date.h"

#include "lx_nrf51Kit.h"

static lx_date_t *current_date = NULL;

int lx_date_get(lx_date_t *date)
{
	if (NULL == current_date)
	{
		if (NULL ==	(current_date = (lx_date_t *)LX_Malloc(sizeof(lx_date_t))))
		{
			return -1;
		}

		current_date->year 			= 1970;
		current_date->month			= 1;
		current_date->day 			= 1;
		current_date->hour 			= 0;
		current_date->minute 		= 0;
		current_date->second 		= 0;
	}

	memcpy(date, current_date, sizeof(lx_date_t));

	return 0;
}

int lx_date_calibrate(lx_date_t *date)
{
	if (NULL == current_date)
	{
		if (NULL ==	(current_date = (lx_date_t *)LX_Malloc(sizeof(lx_date_t))))
		{
			return -1;
		}
	}

	memcpy(current_date, date, sizeof(lx_date_t));

	return 0;
}

int lx_date_add_one_second(void)
{
	if (NULL == current_date)
	{
		if (NULL ==	(current_date = (lx_date_t *)LX_Malloc(sizeof(lx_date_t))))
		{
			return -1;
		}

		current_date->year 			= 1970;
		current_date->month			= 1;
		current_date->day 			= 1;
		current_date->hour 			= 0;
		current_date->minute 		= 0;
		current_date->second 		= 0;
	}

	if (++current_date->second == 60)
	{
		current_date->second = 0;
		if (++current_date->minute == 60)
		{
			current_date->minute = 0;
			if (++current_date->hour == 24)
			{
				current_date->hour = 0;

				++current_date->day;

				if (current_date->day == 32)
				{
					current_date->day = 1;

					if (++current_date->month == 13)
					{
						current_date->month = 1;
						++current_date->year;
					}
				}
				else if (current_date->day == 31)
				{
					if (current_date->month != 1 
						&& current_date->month != 3
						&& current_date->month != 5
						&& current_date->month != 7
						&& current_date->month != 8
						&& current_date->month != 10
						&& current_date->month != 12)
					{
						current_date->day = 1;

						if (++current_date->month == 13)
						{
							current_date->month = 1;
							++current_date->year;
						}
					}
				}
				else if (current_date->day == 30)
				{
					if (current_date->month == 2)
					{
						current_date->day = 1;

						++current_date->month;
					}
				}
				else if (current_date->day == 29)
				{
					if (current_date->month == 2)
					{
						if (!(((current_date->year / 4 == 0) && (current_date->year / 100 != 0))
							|| ( current_date->year / 400 == 0))
							)
						{
							current_date->day = 1;

							++current_date->month;
						} 
					}
				}
			}
		}
	}

	return 0;
}

