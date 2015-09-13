
/************************************************************
Copyright (C), 2008-2014, Colorful Sea Tech. Co., Ltd.
FileName:       manufacture_task.c
Author:      MikeWang   Version : 0.0          Date: 2014/8/18
Description:      s task to process the order from the host sofware      
Version:  0.0        
Function List:   


1. -------History:         
<author>   <time>    <version >    <desc>
Mike      14/8/18      0.0       build this moudle  
***********************************************************/

/*********************************************************************
 * INCLUDES
 */ 

#include "Q_Shell.h"
#include "string.h"
#include "stdio.h"
#include "kernel-includes.h"
#include "manufacture_task.h"
/*********************************************************************
* MACROS
*/

#define MANUFACTURE_TASK_ID  1

//x taskid to recieve ,y specific message ,z lenth of data
#define calibration_send_message(y ,z) send_message(MANUFACTURE_TASK_ID , SYSTEM_EVENT , IPC_MSG , CALIBRATION_COMMAND ,( void*)(y) , z) 

/*********************************************************************
 * TYPEDEFS
 */



/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */





static  os_err_t process_order(data_packge package);

/*********************************************************************
 * @fn      calibration_rx_callback(char data)
 *
 * @brief   This function mainly used for calibration mode in factoies
 *
 * @param   void
 *
 * @return  none
 */
u8 buffer_cmd[sizeof(data_packge)+4]={0};
enum current_status{
	  frame_ideal,
	  frame_start1,
	  frame_start2,
    frame_started,
    frame_recieving,
    frame_preended,
    frame_ended,
};

u8 current_status=frame_ended;
u8 *buffer_ptr =buffer_cmd;
#define START_FLAG_1        0xff
#define START_FLAG_2        0x55
#define START_FLAG_3        0x7e

#define PRE_ENED_FLAG     0x55
#define  ENED_FLAG       (u8)(~0x55)


/*0XFF 0X55 0X7E START OF FRAME*/
os_err_t  calibration_rx_callback(u8 data)
{
    
    
    
    /*start of a frame*/
    //  printf("%.2x",data);
    if(data == START_FLAG_1 && current_status == frame_ended)
    {
        buffer_ptr =  buffer_cmd;
        (*buffer_ptr) = data;
        current_status = frame_start1;
        buffer_ptr++;
        return SUCCESS;
    }else if(data == START_FLAG_2 && current_status == frame_start1)
		{
        (*buffer_ptr) = data; 
			  current_status = frame_start2;
			  buffer_ptr++;
			  return SUCCESS;
      
    }else if(data == START_FLAG_3 && current_status == frame_start2)
		{
       (*buffer_ptr) = data; 
			  current_status = frame_started;
			  buffer_ptr++;
			  return SUCCESS;

    }
    if(current_status == frame_started)
    {
        if(data == PRE_ENED_FLAG)
        {
            current_status = frame_preended;
        }
        *buffer_ptr =  data;
        buffer_ptr++;
        
    }else if(current_status == frame_preended)
    {
        if(data == ENED_FLAG)
        {
            current_status = frame_ended;
            *buffer_ptr =  data;
            buffer_ptr++;
            
            /*send processing task the order obttained by uart1*/
            calibration_send_message( buffer_cmd ,sizeof(data_packge));
            
            // process_order(*((data_packge*)buffer_cmd));
            
            buffer_ptr =  buffer_cmd;
            
        }else if(data == PRE_ENED_FLAG)
        {
            current_status = frame_preended;
            *buffer_ptr =  data;
            buffer_ptr++;
        }else
        {
            current_status = frame_started; 
            *buffer_ptr =  data;
            buffer_ptr++;
        }
        
        
    }
    return SUCCESS;
}








