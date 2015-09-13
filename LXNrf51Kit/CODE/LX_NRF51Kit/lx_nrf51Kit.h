/*
 * Created by lxian1988@gmail.com.
 * 2015/2/9
 */

#ifndef _LX_NRF51_KIT_H_
#define _LX_NRF51_KIT_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef NULL
#define NULL                                    (void *)0
#endif

/* ================================================================================ */
/* ================               Error definitions                ================ */
/* ================================================================================ */

#define LX_OK                                   0
#define LX_ERROR                                -1
#define LX_ERROR_PARAMETER                      -2
#define LX_ERROR_MEMORY                         -3
#define LX_ERROR_TIMEOUT                        -4


/* ================================================================================ */
/* ================              Thread Safe Support     	   ================ */
/* ================================================================================ */

/* 
 * Macros can be changed for different kind of thread safe method. 
 * In a RTOS environment, re-define these critical related functions
 * as those provided by the RTOS. For example, in code running in FreeRTOS,
 * define LX_ENTER_CRITICAL() as taskENTER_CRITICAL().
 * PS: Support interrupt handler as well.
 */

/* WARNING: DO NOT call any SVC functions between critical region */
#include "nrf_soc.h"
#include "softdevice_manager.h"

#define LX_ENTER_CRITICAL()                                                             \
        uint8_t is_nested_critical_region;                                              \
        uint32_t cpusr = 0;	                                                        \
        bool is_softdevice_enabled = Softdevice_IsEnabled();				\
        if (is_softdevice_enabled)							\
        {										\
        	sd_nvic_critical_region_enter(&is_nested_critical_region);		\
        } 										\
        else										\
        {										\
        	cpusr = __get_PRIMASK();					        \
        	__set_PRIMASK(1);							\
        }

#define LX_EXIT_CRITICAL()								\
        if (is_softdevice_enabled)						        \
        {										\
        	sd_nvic_critical_region_exit(is_nested_critical_region);		\
        }										\
        else										\
        {									        \
        	__set_PRIMASK(cpusr);							\
        }										


/* ================================================================================ */
/* ================      	    HEAP Function     	           ================ */
/* ================================================================================ */

/* Memory size in heap */
#define HEAP_SIZE                               (4 * 1024)

/* Memory block size of heap */
#define HEAP_BLOCK_SIZE                         (32)  	  						

/* Memory block table size */
#define HEAP_ALLOC_TABLE_SIZE                   (HEAP_SIZE / HEAP_BLOCK_SIZE) 	
 	

#include "lx_heap/lx_heap.h"

/*
 * Macros can be changed for portable request. For example, in a STM32F10x + FreeRTOS project,
 * LX_Malloc can be defined as function pvPortMalloc in heap_4.c. For more information see 
 * FreeRTOS demos.
 */
#define	LX_Malloc                               lx_malloc
#define LX_Free                                 lx_free
#define LX_Memusage                             lx_memusage
#define LX_Memcpy                               memcpy 
#define LX_Memset                               memset 
#define LX_Memcmp                               memcmp

/* ================================================================================ */
/* ================      	    FIFO Function     	           ================ */
/* ================================================================================ */

#include "lx_fifo/lx_fifo.h"

/*
 * Macros can be changed for portable request.
 */	
#define LX_FIFOType		                void *

#define LX_FIFOCreate                           lx_fifo_create
#define LX_FIFODestroy                          lx_fifo_destroy
#define LX_FIFOWrite                            lx_fifo_write
#define LX_FIFORead                             lx_fifo_read
#define LX_FIFOCount                            lx_fifo_count
#define LX_FIFOIsEmpty                          lx_fifo_is_empty
#define LX_FIFOIsFull                           lx_fifo_is_full
#define LX_FIFOFlush                            lx_fifo_flush


/* ================================================================================ */
/* ================      	    QUEUE Function     	           ================ */
/* ================================================================================ */

#include "lx_queue/lx_queue.h"

/*
 * Macros can be changed for portable request.
 */
#define LX_QueueType                            void *

#define LX_QueueCreate                          lx_queue_create
#define LX_QueueDestroy                         lx_queue_destroy
#define LX_QueueAppend                          lx_queue_append
#define LX_QueueTakeFirst                       lx_queue_take_first
#define LX_QueueAt                              lx_queue_at
#define LX_QueueRemove                          lx_queue_remove
#define LX_QueueCount                           lx_queue_count
#define LX_QueueClear                           lx_queue_clear

/* ================================================================================ */
/* ================                 Date Function                  ================ */
/* ================================================================================ */

#include "lx_date/lx_date.h"

#define LX_DateType                             lx_date_t

#define LX_DateGet                              lx_date_get
#define LX_DateCalibrate                        lx_date_calibrate
#define LX_DateAddOneSecond                     lx_date_add_one_second

/* ================================================================================ */
/* ================          NotificationCenter Function     	   ================ */
/* ================================================================================ */

#include "lx_notificationcenter/lx_notificationcenter.h"

#define LX_NotificationType                     lx_notification_t

#define LX_NotificationCenterPut                lx_notification_center_put
#define LX_NotificationCenterGet                lx_notification_center_get
#define LX_NotificationCenterContains           lx_notification_center_contains
#define LX_NotificationCenterRemove             lx_notification_center_remove
#define LX_NotificationCenterCount              lx_notification_center_count


#endif	/* _LX_NRF51_KIT_H_ */
