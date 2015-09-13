/***************************************************************
 * Name:      LENTHMEASURE.H
 * Purpose:   define  macros and decalration of function
 * Author:    mikewang(s)
 * Created:   2014-05-15
 * Copyright: mikewang(mikewang01@hotmail.com)
 * License:
 **************************************************************/

#ifndef __DEVICE_H__
#define __DEVICE_H__
/*********************************************************************
 * INCLUDES
 */

#include "cpu.h"
#include "obj.h"
#include "config.h"

/*********************************************************************
* TYPEDEF
*/




/*********************************************************************
* MACROS
*/

/**
* container_of - cast a member of a structure out to the containing structure
* @ptr:     the pointer to the member.
* @type:     the type of the container struct this is embedded in.
* @member:     the name of the member within the struct.
*
*/
typedef u32 size_t;
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({           \
         const typeof( ((type *)0)->member ) *__mptr = (ptr);     \
					(type *)( (char *)__mptr - offsetof(type,member) );})

					

/*********************************************************************
 * ENUM
 */
 				 
/* device (I/O) class type 
	 */  
	enum os_device_class_type  
	{  
			OS_Device_Class_Char = 0,                           /**< character device */  
	    OS_Device_Class_Block,                              /**< block device */  
	    OS_Device_Class_NetIf,                              /**< net interface */  
	    OS_Device_Class_MTD,                                /**< memory device */  
	    OS_Device_Class_CAN,                                /**< CAN device */  
	    OS_Device_Class_RTC,                                /**< RTC device */  
	    OS_Device_Class_Sound,                              /**< Sound device */  
	    OS_Device_Class_Graphic,                            /**< Graphic device */  
	    OS_Device_Class_I2CBUS,                             /**< I2C bus device */  
	    OS_Device_Class_USBDevice,                          /**< USB slave device */  
	    OS_Device_Class_USBHost,                            /**< USB host bus */  
	    OS_Device_Class_SPIBUS,                             /**< SPI bus device */  
	    OS_Device_Class_SPIDevice,                          /**< SPI device */  
	    OS_Device_Class_SDIO,                               /**< SDIO bus device */  
	    OS_Device_Class_PM,                                 /**< PM pseudo device */ 
      OS_Device_Class_Pipe,																/**< pipe  device */ 
      OS_Device_Class_Portal,															/**< portal  device */ 
		  OS_Device_Class_Misc,																 /**< Misc pseudo device */ 
	    OS_Device_Class_Unknown                             /**< unknown device */  
	};  
	
	
enum{
	OS_DEVICE_FLAG_ACTIVATED  = 0X01,
	OS_DEVICE_FLAG_INACTIVATED= 0X02,
	RT_DEVICE_FLAG_STANDALONE = 0X04,
	RT_DEVICE_OFLAG_OPEN      = 0X08,
	RT_DEVICE_OFLAG_CLOSE     =	0X10
};

enum{
	OS_DEVICE_USART_ID=0x01,
	OS_DEVICE_KEY_ID,

};
/*********************************************************************
 * STRUCTS
 */
 

	/** 
	 * Device structure 
	 */  
	

	
	
typedef struct rt_device  os_device_t;
struct rt_device  
 	{  
  	    struct os_object          parent;                   /**< inherit from rt_object */ 
	  
  	    enum os_device_class_type type;                     /**< device type */
  	    u16               flag;                     /**< device flag */ 
  	    u16               open_flag;                /**< device open flag */
	  
  	    u8                device_id;                /**< 0 - 255 */ 
	  
  	    /* device call back */  
  	    os_err_t (*rx_indicate)(os_device_t* dev, os_size_t size);  
  	    os_err_t (*tx_complete)(os_device_t* dev, void *buffer);   
 	  
 	    /* common device interface */  
  	    os_err_t  (*init)   (os_device_t* dev);//???????   
  	    os_err_t  (*open)   (os_device_t* dev, u16 oflag);//??????   
  	    os_err_t  (*close)  (os_device_t* dev);//??????   
   	    os_size_t (*read)   (os_device_t* dev, os_off_t pos, void *buffer, os_size_t size);//read device ptr  
        os_size_t (*write)  (os_device_t* dev, os_off_t pos, const void *buffer, os_size_t size);//?????   
	      os_err_t  (*control)(os_device_t* dev, u8 cmd, void *args);//??????   
	  
  	#ifdef OS_USING_DEVICE_SUSPEND   
	    os_err_t (*suspend) (os_device_t* dev);//????   
	    os_err_t (*resumed) (os_device_t* dev);//????   
   	#endif   
	  
	    void                     *user_data;                /**< device private data *///????  
	};  




   
/*********************************************************************
 * CONSTANTS
 */
   

   


/*********************************************************************
 * FUNCTIONS
 */
os_err_t os_device_register(os_device_t *dev, const char  *name,  u16 flags); 
	
os_err_t os_device_unregister(os_device_t *dev);
	
os_err_t os_device_init_all(void);

os_err_t os_device_open(os_device_t *dev, u16 oflag);

os_err_t os_device_close(os_device_t* dev);

os_size_t os_device_read(os_device_t *dev,  
	                         os_off_t    pos,  
	                         void       *buffer,  
	                         os_size_t   size); 
													 
	os_size_t os_device_write(os_device_t *dev,  
	                          os_off_t    pos,  
	                          const void *buffer,  
	                          os_size_t   size);

os_err_t os_device_control(os_device_t *dev, u8 cmd, void *arg);

os_err_t os_device_set_rx_indicate(os_device_t *dev,   os_err_t (*rx_ind)(os_device_t *dev, os_size_t size));

os_err_t os_device_set_tx_complete(os_device_t *dev,  os_err_t (*tx_done)(os_device_t *dev, void *buffer)) ;	
os_device_t* os_device_get(const char * name) ;														
/*
 * Task Initialization for lenth measure
 */






#endif


