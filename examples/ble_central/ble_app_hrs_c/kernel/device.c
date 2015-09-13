/***************************************************************
 * Name:      device.c
 * Purpose:   code for device management
 * Author:    mikewang(s)
 * Created:   2014-05-15
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "kernel-includes.h"
#include "string.h"
/*********************************************************************
* MACROS
*/



/*********************************************************************
 * TYPEDEFS
 */



/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * LOCAL VARIABLES
 */
static struct os_object *object_dev_list;//header list for dev

/*********************************************************************
 * EXTERNAL VARIABLES
 */
/*********************************************************************
 * FUNCTIONS
 */



/*********************************************************************
 * @fn      OS_err_t OS_device_register
 *
 * @brief   This function registers a device driver with specified name
 *
 * 			    @param dev the pointer of device driver structure 
 *					@param name the device driver's name
 *					@param flags the flag of device
 * 
 *   @return the error code, OS_EOK on initialization successfully. 
 */
os_err_t os_device_register(os_device_t *dev,   
                            const char  *name,   
                            u16 flags)   
{   
    if (dev == NULL)   
        return ERROR;   
    
    if (os_object_find(object_dev_list,name) != NULL)//if this device has been registored return error  
        return ERROR;   
    dev->flag = flags;
    
    os_object_init(&object_dev_list, &(dev->parent), OS_Object_Class_Device , name);//???????,?????????????????????,??????   
    
    
    return SUCCESS;   
}


/*********************************************************************
 * @fn      OS_err_t rt_device_unregister
 *
 * @brief   This function unregisters a device driver with specified by dev structor pointer
 *
 * 			    @param dev the pointer of device driver structure 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */
os_err_t os_device_unregister(os_device_t *dev)  
{  
    //RT_ASSERT(dev != RT_NULL);  	  
    return os_object_detach(&object_dev_list , &(dev->parent));//  
} 




/*********************************************************************
 * @fn      os_err_t os_device_init_all
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */

os_err_t os_device_init_all(void)
{
    os_device_t *device=NULL;
    struct os_object *object=object_dev_list;
    //const typeof( ((os_device_t *)0)->parent )
    while(object!=NULL)
    {	
        //	container_of(object,os_device_t,parent);
        
        struct os_object *__mptr = (object);
        device=(os_device_t *)( (char *)__mptr - offsetof(os_device_t,parent));
        if(device->init!=NULL)
        {
            if((device->flag&OS_DEVICE_FLAG_ACTIVATED)!=OS_DEVICE_FLAG_ACTIVATED)
            {
                device->init(device);
                device->flag|=OS_DEVICE_FLAG_ACTIVATED;
            }
            else
            {
                device->flag|=OS_DEVICE_FLAG_ACTIVATED;
            }
            
        }
        object=object->next;
    }
    return SUCCESS;
} 	


/*********************************************************************
 * @fn      os_err_t rt_device_open
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */
os_err_t os_device_open(os_device_t *dev, u16 oflag)  
{  
    os_err_t result = SUCCESS;  
    
    if(dev==NULL)
    {
        return ERROR;
    }
    
    /* if device is not initialized, initialize it. */  
    if (!(dev->flag & OS_DEVICE_FLAG_ACTIVATED))//??????????   
    {  
        if (dev->init != NULL)  
        {  
            result = dev->init(dev);//?????????????   
            if (result != SUCCESS)  
            {  
                //printf("To initialize device:%s failed. The error code is %d\n",  
                //       dev->parent.name, result);  
                
                return result;  
            }  
        }  
        
        dev->flag |= OS_DEVICE_FLAG_ACTIVATED;//??????   
    }  
    
    /* device is a stand alone device and opened *///????????  
    if ((dev->flag & RT_DEVICE_FLAG_STANDALONE) &&  
            (dev->open_flag & RT_DEVICE_OFLAG_OPEN))  
    {  
        return BUSY;  
    }  
    
    /* call device open interface */  
    if (dev->open != NULL)  
    {  
        result = dev->open(dev, oflag);//????   
    }  
    
    /* set open flag */  
    if (result == SUCCESS || result == OS_ENOSYS)  
        dev->open_flag = oflag | RT_DEVICE_OFLAG_OPEN;//??????   
    
    return result;  
}  

/*********************************************************************
 * @fn      os_err_t rt_device_open
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */
os_err_t os_device_close(os_device_t* dev)  	
{  
    os_err_t result = SUCCESS;  
    
    if(dev==NULL)
    {
        return ERROR;
    }	  
    /* call device close interface */  
    if (dev->close != NULL)  
    {  
        result = dev->close(dev);//????   
    }  
    
    /* set open flag */  
    if (result == SUCCESS || result == OS_ENOSYS)  
        dev->open_flag = RT_DEVICE_OFLAG_CLOSE;//???????????   
    
    return result;  
}  


/*********************************************************************
 * @fn      os_err_t os_device_read
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */
os_size_t os_device_read(os_device_t *dev,  
                         os_off_t    pos,  
                         void       *buffer,  
                         os_size_t   size)  
{  
    if(dev==NULL)
    {
        return ERROR;
    }	  
    
    /* call device read interface */  
    if (dev->read != NULL) 
    {  
        return dev->read(dev, pos, buffer, size);
    }  
    
    
    return 0;  
}  

/*********************************************************************
 * @fn      os_err_t os_device_write
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */
os_size_t os_device_write(os_device_t *dev,  
                          os_off_t    pos,  
                          const void *buffer,  
                          os_size_t   size)  
{  
    
    if(dev==NULL)
    {
        return ERROR;
    }	  	  
    /* call device write interface */  
    if (dev->write != NULL)//if wrtite pointer is not empty
    {  
        return dev->write(dev, pos, buffer, size);//call driver layer api  
    }  
    
    
    return 0;  
}  

/*********************************************************************
 * @fn      os_err_t os_device_control
 *
 * @brief   This function used to control specific device
 *
 * 			    @param os_device_t *dev  :device pointer
 *                 u8 cmd :controlled cmd for this moudule
 *                 void *arg :arg[0] numbers of parameter
 *                 void *arg :arg[1] details of data
 * 
 *   @return the error code, success on  successfully. 
 */
os_err_t os_device_control(os_device_t *dev, u8 cmd, void *arg)  
{  
    if(dev==NULL)
    {
        return ERROR;
    }		  
    /* call device write interface */  
    if (dev->control != NULL)  
    {  
        return dev->control(dev, cmd, arg); 
    }  
    
    return SUCCESS;  
}  

/*********************************************************************
 * @fn      os_err_t rt_device_set_rx_indicate
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */	
os_err_t os_device_set_rx_indicate(os_device_t *dev,   os_err_t (*rx_ind)(os_device_t *dev, os_size_t size))  
{  
    if(dev==NULL)
    {
        return ERROR;
    }	
    
    dev->rx_indicate= rx_ind;  
    
    return SUCCESS;  
} 

/*********************************************************************
 * @fn      os_err_t rt_device_set_tx_complete
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */	
os_err_t os_device_set_tx_complete(os_device_t *dev,  os_err_t (*tx_done)(os_device_t *dev, void *buffer))  
{  
    if(dev==NULL)
    {
        return ERROR;
    }	
    
    dev->tx_complete = tx_done;  
    
    return SUCCESS;  
}  

/*********************************************************************
 * @fn      os_err_t rt_device_set_tx_complete
 *
 * @brief   This function initiate all the device registered
 *
 * 			    @param none 
 *			
 * 
 *   @return the error code, success on initialization successfully. 
 */	
os_device_t* os_device_get(const char * name)  
{  
    if(object_dev_list==NULL)
    {
        return NULL;
    }	else
    {
        struct os_object *obj=os_object_find(object_dev_list,name);
        if(obj!=NULL)
        {
            struct os_object *__mptr = (obj);
            return ((os_device_t *)( (char *)__mptr - offsetof(os_device_t,parent)));
        }
    }
    return NULL;
} 	




os_err_t _list_device()
{
    
    struct os_object *__mptr = object_dev_list;
    
    
    char * const device_type_str[] =
    {
        "Character Device",
        "Block Device",
        "Network Interface",
        "MTD Device",
        "CAN Device",
        "RTC",
        "Sound Device",
        "Graphic Device",
        "I2C Bus",
        "USB Slave Device",
        "USB Host Bus",
        "SPI Bus",
        "SPI Device",
        "SDIO Bus",
        "PM Pseudo Device",
        "Pipe",
        "Portal Device",
        "Miscellaneous Device",
        "Unknown"
    };
    
    if(__mptr==NULL)
        return ERROR;
    
    printf("device    type      \r\n");
    printf("-------- ---------- \r\n");
    for (; __mptr!=NULL; __mptr = __mptr->next)
    {
        struct rt_device *device=(os_device_t *)( (char *)__mptr - offsetof(os_device_t,parent));
        printf("%-8.*s %-8s \r\n",
               OS_NAME_MAX,
               device->parent.name,
               device_type_str[device->type] 
                );
    }
    
    return SUCCESS;
}


