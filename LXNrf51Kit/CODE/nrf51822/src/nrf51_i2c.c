#include "nrf51_i2c.h"



#define TWI_SDA_STANDARD0_NODRIVE1(pin) do { \
        NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
        |(GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)    \
        |(GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)  \
        |(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) \
        |(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);  \
} while (0) /*!< Configures SDA pin to Standard-0, No-drive 1 */


#define TWI_SCL_STANDARD0_NODRIVE1(pin) do { \
        NRF_GPIO->PIN_CNF[pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
        |(GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos)    \
        |(GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)  \
        |(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) \
        |(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);  \
} while (0) /*!< Configures SCL pin to Standard-0, No-drive 1 */


#define TWI_SDA_INPUT(pin)  do { NRF_GPIO->DIRCLR = (1UL << pin);  } while(0)   /*!< Configures SDA pin as input  */
#define TWI_SDA_OUTPUT(pin) do { NRF_GPIO->DIRSET = (1UL << pin);  } while(0)   /*!< Configures SDA pin as output */
#define TWI_SCL_OUTPUT(pin) do { NRF_GPIO->DIRSET = (1UL << pin); } while(0)   /*!< Configures SCL pin as output */

#define TWI_SDA_READ(pin) ((NRF_GPIO->IN >> pin) & 0x1UL)                     /*!< Reads current state of SDA */
#define TWI_SCL_READ(pin) ((NRF_GPIO->IN >> pin) & 0x1UL)                    /*!< Reads current state of SCL */

#define TWI_SCL_HIGH(pin)   do { NRF_GPIO->OUTSET = (1UL << pin); } while(0)   /*!< Pulls SCL line high */
#define TWI_SCL_LOW(pin)    do { NRF_GPIO->OUTCLR = (1UL << pin); } while(0)   /*!< Pulls SCL line low  */
#define TWI_SDA_HIGH(pin)   do { NRF_GPIO->OUTSET = (1UL << pin);  } while(0)   /*!< Pulls SDA line high */
#define TWI_SDA_LOW(pin)    do { NRF_GPIO->OUTCLR = (1UL << pin);  } while(0)   /*!< Pulls SDA line low  */

#define TWI_DELAY() DelayUS(20) 

static uint32_t sda_pin,sck_pin;

static void i2c_master_clear_bus(void)
{
		TWI_SDA_OUTPUT(sda_pin);
		TWI_SCL_OUTPUT(sck_pin);
    TWI_SDA_HIGH(sda_pin);
    TWI_SCL_HIGH(sck_pin);
    TWI_DELAY();
}

static void i2c_start(void)
{
	TWI_SDA_OUTPUT(sda_pin);
	TWI_SCL_OUTPUT(sck_pin);
	TWI_SCL_LOW(sck_pin);
	TWI_DELAY();
	TWI_SCL_HIGH(sck_pin);
	TWI_DELAY();
	TWI_SDA_HIGH(sda_pin);
	TWI_DELAY();
	TWI_SDA_LOW(sda_pin);
	TWI_DELAY();
	TWI_SCL_LOW(sck_pin);
}
static void i2c_stop(void)
{
	TWI_SDA_OUTPUT(sda_pin);
	TWI_SCL_OUTPUT(sck_pin);
	TWI_SCL_LOW(sck_pin);
	TWI_DELAY();
	TWI_SCL_HIGH(sck_pin);
	TWI_DELAY();
	TWI_SDA_LOW(sda_pin);
	TWI_DELAY();
	TWI_SDA_HIGH(sda_pin);
	TWI_DELAY();
	TWI_SCL_HIGH(sck_pin);

}
/*****************??????? ???????*****/
/*      Return:		acknowledgement from slave:         		      */
/*		        0 = no acknowledge is received			      */
/*	       	 	1 = acknowledge is received			      */ 
static uint8_t i2c_receive_ack(void)
{
	uint8_t ack=0;
	uint16_t timeout=0;
	TWI_SDA_HIGH(sda_pin);		
	TWI_DELAY();
	TWI_SDA_INPUT(sda_pin);	
	TWI_DELAY();
	TWI_SCL_HIGH(sck_pin);
	TWI_DELAY();
	
	while(TWI_SDA_READ(sda_pin))
	{
			if(timeout++>22440)
			{
				break;
			}
	}
	
	if(timeout>=22400)
	{
		ack=0;
	}
	else
	{
		ack=1;		//ack
	}
	TWI_SCL_LOW(sck_pin);
  TWI_DELAY();	

return ack;
}
static void i2c_send_ack(uint8_t ack)
{
	TWI_SDA_OUTPUT(sda_pin);
	TWI_SCL_LOW(sck_pin);
	TWI_DELAY();
	if(ack)
	{
	TWI_SDA_HIGH(sda_pin);	//nack
	}
	else
	{
	TWI_SDA_LOW(sda_pin);		//ack
	}
	TWI_SCL_HIGH(sck_pin);
	TWI_DELAY();
	TWI_SCL_LOW(sck_pin);
		
	TWI_DELAY();
	TWI_SCL_HIGH(sck_pin);
}
/*****************??????? ???????*****/
/*      Return:		acknowledgement from slave:         		      */
/*		        0 = no acknowledge is received			      */
/*	       	 	1 = acknowledge is received			      */   
static int  i2c_master_clock_byte_ledo(uint8_t byte)
{
	TWI_SDA_OUTPUT(sda_pin);
   // MSB first
    for(uint8_t i=0;i<8;i++)
    {
        TWI_SCL_LOW(sck_pin);     
        if (byte & 0x80)
        {
            TWI_SDA_HIGH(sda_pin);
        }
        else
        {
            TWI_SDA_LOW(sda_pin);
        }	
				TWI_DELAY();				
				TWI_SCL_HIGH(sck_pin);
				TWI_DELAY();
				//TWI_SCL_LOW();
				byte=byte<<1;
		}
		TWI_SCL_LOW(sck_pin);
		TWI_SDA_HIGH(sda_pin);
		TWI_DELAY();

		return i2c_receive_ack();
}
 

static uint8_t i2c_read_byte(uint8_t ack)
{
	uint8_t i=0,BufferData=0;
	TWI_SDA_HIGH(sda_pin);
	TWI_SDA_INPUT(sda_pin);
	for (i=8; i>0; i--) 
	{
		TWI_SCL_LOW(sck_pin);
		TWI_DELAY();
		TWI_SCL_HIGH(sck_pin);
		TWI_DELAY();
		if(TWI_SDA_READ(sda_pin))
		{
			BufferData=BufferData|(1<<(i-1));
		}
	}

	i2c_send_ack(ack);	//nack
	return BufferData;
}


void I2C_Init(I2C_InitType *I2C_InitStruct)
{
	GPIO_InitType GPIO_InitStruct;

	GPIO_InitStruct.dir 	= GPIO_PIN_CNF_DIR_Output;
	GPIO_InitStruct.pull 	= GPIO_PIN_CNF_PULL_Pullup;
	GPIO_InitStruct.drive 	= GPIO_PIN_CNF_DRIVE_S0H1;
	GPIO_InitStruct.sense	= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input 	= GPIO_PIN_CNF_INPUT_Disconnect;

	GPIO_Init(I2C_InitStruct->sda_pin, &GPIO_InitStruct);
	GPIO_Init(I2C_InitStruct->sck_pin, &GPIO_InitStruct);
	sda_pin=I2C_InitStruct->sda_pin;
	sck_pin=I2C_InitStruct->sck_pin;
    // Configure both pins to output Standard 0, No-drive (open-drain) 1
    TWI_SDA_STANDARD0_NODRIVE1(sda_pin); /*lint !e416 "Creation of out of bounds pointer" */
    TWI_SCL_STANDARD0_NODRIVE1(sck_pin); /*lint !e416 "Creation of out of bounds pointer" */

    // Configure SCL as output
    TWI_SCL_HIGH(sck_pin);
    TWI_SCL_OUTPUT(sck_pin);

    // Configure SDA as output
    TWI_SDA_HIGH(sda_pin);
    TWI_SDA_OUTPUT(sda_pin);

    i2c_master_clear_bus();
}

/*      Return:		acknowledgement from slave:         		      */
/*		        0 = no acknowledge is received			      */
/*	       	 	1 =  acknowledge is received			      */   

int I2C_ReadCommand(uint8_t slave_address ,uint8_t *value,uint16_t len)
{
	i2c_start();
	if(!i2c_master_clock_byte_ledo(slave_address))
	{
		i2c_stop();		
		return -1;
	}	
	for(uint8_t i=0;i<len-1;i++)
	{
		*value++=i2c_read_byte(0);	//ack  	
		//*value++=0x45;
	}	
	*value++=i2c_read_byte(1);	//ack 
	
	i2c_stop();			  //light_led(LD1_3_5);
	i2c_master_clear_bus(); 
	return 0;
}

int I2C_WriteCommand(uint8_t slave_address,uint8_t command )
{
  i2c_start();
	if(!i2c_master_clock_byte_ledo(slave_address))
	{
		i2c_stop();
		return -1;
	}
DelayMS(1);	
	if(!i2c_master_clock_byte_ledo(command))
	{
		i2c_stop();
		return -1;
	}

	i2c_stop();
	i2c_master_clear_bus();
	return 0;
}


int I2C_Write(uint8_t slave_address,uint8_t reg,uint8_t *value,uint16_t len)
{
  i2c_start();
	if(!i2c_master_clock_byte_ledo(slave_address))
	{
		i2c_stop();
		return -1;
	}
	if(!i2c_master_clock_byte_ledo(reg))
	{
		i2c_stop();
		return -1;
	}
	for(uint8_t i=0;i<len;i++)
	{
		if(!i2c_master_clock_byte_ledo(*value++))
		{
			i2c_stop();
			return -1;
		}
	}
	i2c_stop();
	i2c_master_clear_bus();
	return 0;
}

int I2C_Read(uint8_t slave_address ,uint8_t reg,uint8_t *value,uint16_t len)
{
//	uint8_t regdata;

  i2c_start();
	if(!i2c_master_clock_byte_ledo(slave_address))
	{
		i2c_stop();		
		return -1;
	}	
	if(!i2c_master_clock_byte_ledo(reg))
	{
		i2c_stop();
		return -1;
	}	
	i2c_start();
	if(!i2c_master_clock_byte_ledo(slave_address+1))
	{
		i2c_stop();
		return -1;
	}
	
	for(uint8_t i=0;i<len-1;i++)
	{
		*value++=i2c_read_byte(0);	//ACk  	
		//*value++=0x45;
	}	
	*value++=i2c_read_byte(1);	//ack 
  i2c_stop();			  
	i2c_master_clear_bus(); 
	return 0;
}


