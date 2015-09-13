#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "lx_nrf51Kit.h"
/*
 * definitions of debug level
 */
#define DEBUG_LEVEL_NONE								0
#define DEBUG_LEVEL_ERROR								1
#define DEBUG_LEVEL_WARNING								2
#define DEBUG_LEVEL_LOG									3
#define DEBUG_LEVEL_ALL									10

/* 
 * debug level setting
 */

// #define DEBUG_LEVEL										DEBUG_LEVEL_NONE				/* Disable all log. */
// #define DEBUG_LEVEL										DEBUG_LEVEL_ERROR 				/* Enable errors only. */
#define DEBUG_LEVEL										DEBUG_LEVEL_WARNING 			/* Enable errors and warnings. */
// #define DEBUG_LEVEL										DEBUG_LEVEL_LOG 				/* Enable errors, warnings and logs. */
// #define DEBUG_LEVEL										DEBUG_LEVEL_ALL					/* Enable all. */

/*
 * definitions of debug directions
 */
#define DEBUG_MODE_UART									0
#define DEBUG_MODE_RADIO								0
#define DEBUG_MODE_FLASH								1


/*
 * definitions of error function
 */
#if (DEBUG_LEVEL >= DEBUG_LEVEL_ERROR)

extern int debug_status;
void Debug_Init(void);

#define DEBUG_ERROR(format, ...)	do{	\
										if (!debug_status)	\
										{	\
											Debug_Init();	\
											debug_status = 1;	\
										}	\
										LX_DateType logDate;	\
										LX_DateGet(&logDate);	\
										LX_ENTER_CRITICAL();	\
										printf("%04d-%02d-%02d %02d:%02d:%02d [ERROR] %s, %s, %d --->"format"\r\n", logDate.year, logDate.month, logDate.day, logDate.hour, logDate.minute, logDate.second, strrchr(__FILE__, '\\') + 1, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
										LX_EXIT_CRITICAL();	\
									}while(0)

#else

#define DEBUG_ERROR(format, ...)

#endif  /* DEBUG_LEVEL >= DEBUG_LEVEL_ERROR */

/*
 * definitions of warning function
 */
#if (DEBUG_LEVEL >= DEBUG_LEVEL_WARNING)

#define DEBUG_WARNING(format, ...)	do{	\
										if (!debug_status)	\
										{	\
											Debug_Init();	\
											debug_status = 1;	\
										}	\
										LX_DateType logDate;	\
										LX_DateGet(&logDate);	\
										LX_ENTER_CRITICAL();	\
										printf("%04d-%02d-%02d %02d:%02d:%02d [WARNING] %s, %s, %d --->"format"\r\n", logDate.year, logDate.month, logDate.day, logDate.hour, logDate.minute, logDate.second, strrchr(__FILE__, '\\') + 1, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
										LX_EXIT_CRITICAL();		\
									}while(0)

#else

#define DEBUG_WARNING(format, ...)

#endif	/* DEBUG_LEVEL >= DEBUG_LEVEL_WARNING */

/* 
 * defintions of log function
 */
#if (DEBUG_LEVEL >= DEBUG_LEVEL_LOG)

#define DEBUG_LOG(format, ...)		do{	\
										if (!debug_status)	\
										{	\
											Debug_Init();	\
											debug_status = 1;	\
										}	\
										LX_DateType logDate;	\
										LX_DateGet(&logDate);	\
										LX_ENTER_CRITICAL();	\
										printf("%04d-%02d-%02d %02d:%02d:%02d [LOG] %s, %s, %d --->"format"\r\n", logDate.year, logDate.month, logDate.day, logDate.hour, logDate.minute, logDate.second, strrchr(__FILE__, '\\') + 1, __FUNCTION__, __LINE__, ##__VA_ARGS__);	\
										LX_EXIT_CRITICAL();		\
									}while(0)
	
#else

#define DEBUG_LOG(format, ...)

#endif	/* DEBUG_LEVEL >= DEBUG_LEVEL_LOG */


#endif	/* _DEBUG_H_ */
