#include "MX25L4006E.h"

#include "nrf51.h"
#include "debug.h"

#include "app.h"

/* MX25 series command hex code definition */
#define    	FLASH_ID        		0xC22013
#define    	FLASH_ELECTRONICID     	0x12 
#define    	FLASH_RESID0           	0xC212

//ID comands
#define    	FLASH_CMD_RDID      	0x9F    // RDID (Read Identification)
#define    	FLASH_CMD_RES       	0xAB    // RES (Read Electronic ID)
#define    	FLASH_CMD_REMS      	0x90    // REMS (Read Electronic & Device ID)

//register comands
#define    	FLASH_CMD_WRSR      	0x01    // WRSR (Write Status register)
#define    	FLASH_CMD_RDSR      	0x05    // RDSR (Read Status register)

//READ comands
#define    	FLASH_CMD_READ        	0x03    // READ (1 x I/O)
#define    	FLASH_CMD_FASTREAD    	0x0B    // FAST READ (Fast read data)
#define    	FLASH_CMD_DREAD       	0x3B    // DREAD (1In/2 Out fast read)

//Program comands
#define    	FLASH_CMD_WREN     		0x06    // WREN (Write Enable)
#define    	FLASH_CMD_WRDI     		0x04    // WRDI (Write Disable)
#define    	FLASH_CMD_PP       		0x02    // PP (page program)

//Erase comands
#define    	FLASH_CMD_SE       		0x20    // SE (Sector Erase)
#define    	FLASH_CMD_BE       		0xD8    // BE (Block Erase)
#define    	FLASH_CMD_CE       		0x60    // CE (Chip Erase) hex code: 60 or C7

//Mode setting comands
#define    	FLASH_CMD_DP       		0xB9    // DP (Deep Power Down)
#define    	FLASH_CMD_RDP      		0xAB    // RDP (Release form Deep Power Down)

//Status register
#define		STATUS_FLAG_WIP			0x01 	// Write in progress bit
#define		STATUS_FLAG_WEL			0x02 	// Write enable latch bit

// SPI CS pin control
#define 	FLASH_SPI_ENBALE()		GPIO_ResetBit(BOARD_PIN_FLASH_CS)
#define 	FLASH_SPI_DISABLE()		GPIO_SetBit(BOARD_PIN_FLASH_CS)


static int checkID(void)
{
	uint32_t flashID = 0;
	
	uint8_t cmd[4] = {0};
	cmd[0] = FLASH_CMD_RDID;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, true))
	{
		// DEBUG_LOG("send command RDID failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();
	
	flashID = (cmd[1] << 16) | (cmd[2] << 8) | cmd[3];
	 
	// DEBUG_LOG("flashID is: 0x%04X", flashID);

	if (flashID != FLASH_ID)
	{
		// DEBUG_ERROR("MX25L4006E ID is wrong!");
		return -1;
	}

	return 0;
}

static int checkElectronicID(void)
{
	uint32_t electronicID = 0;
	
	uint8_t cmd[5] = {0};
	cmd[0] = FLASH_CMD_RES;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, true))
	{
		// DEBUG_ERROR("send command RES failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();
	

	electronicID = cmd[4];
	 
	// DEBUG_LOG("electronicID is: 0x%04X", electronicID);

	if (electronicID != FLASH_ELECTRONICID)
	{
		// DEBUG_ERROR("MX25L4006E electronicID is wrong!");
		return -1;
	}

	return 0;
}

static int checkElectronicAndDeviceID(void)
{
	uint32_t electronicAndDeviceID = 0;

	uint8_t cmd[6] = {0};
	cmd[0] = FLASH_CMD_REMS;


	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, true))
	{
		// DEBUG_ERROR("send command RDID failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();

	electronicAndDeviceID = (cmd[4] << 8) | cmd[5];

	// DEBUG_LOG("electronicAndDeviceID is: 0x%04X", electronicAndDeviceID);

	if (electronicAndDeviceID != FLASH_RESID0)
	{
		// DEBUG_ERROR("MX25L4006E electronic and device ID is wrong!");
		return -1;
	}

	return 0;
}

static int checkStatus(uint8_t flags)
{
	uint32_t status = 0;

	uint8_t cmd[2] = {0};
	cmd[0] = FLASH_CMD_RDSR;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, true))
	{
		// DEBUG_ERROR("send command RDID failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();

	status = cmd[1];

	// DEBUG_LOG("status is: 0x%04X", status);

	return status & flags;
}

static int commandWriteEnable(void)
{
	uint8_t cmd[1] = {0};

	cmd[0] = FLASH_CMD_WREN;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, false))
	{
		// DEBUG_ERROR("send command RDID failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();
	
	return 0;
}

static int commandChipErase(int timeout)
{
	int retry = timeout / 1;

	while (checkStatus(STATUS_FLAG_WIP))
	{
		/* do nothing */
		if (retry-- == 0)
		{
			// DEBUG_ERROR("erase failed! timeout!");
			return -1;
		}

		DelayMS(1);
	}	

	if (0 != commandWriteEnable())
	{
		// DEBUG_ERROR("command write enable failed!");
		return -1;
	}

	uint8_t cmd[1] = {0};

	cmd[0] = FLASH_CMD_CE;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, false))
	{
		// DEBUG_ERROR("send command RDID failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();

	return 0;
}

static int commandSectorErase(uint32_t address, int timeout)
{
	if (address > MX25L4006E_ADDRESS_MAX)
	{
		// DEBUG_ERROR("invalid address!");
		return -1;
	}

	int retry = timeout / 1;

	while (checkStatus(STATUS_FLAG_WIP))
	{
		/* do nothing */
		if (retry-- == 0)
		{
			// DEBUG_ERROR("erase failed! timeout!");
			return -1;
		}

		DelayMS(1);
	}	

	if (0 != commandWriteEnable())
	{
		// DEBUG_ERROR("command write enable failed!");
		return -1;
	}

	uint8_t cmd[4] = {0};

	cmd[0] = FLASH_CMD_SE;
	cmd[1] = (address >> 16);
	cmd[2] = (address >> 8);
	cmd[3] = address;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, false))
	{
		// DEBUG_ERROR("send command RDID failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();

	return 0;
}

static int commandPageProgram(uint32_t address, const uint8_t *buf, int size, int timeout)
{
	if (address > MX25L4006E_ADDRESS_MAX)
	{
		// DEBUG_ERROR("error: invalid address!");
		return -1;
	}

	if ((address & 0xFF) + size > 256)
	{
		// DEBUG_ERROR("error: write operation will exceed one page! address is: %d", address);
		return -1;
	}

	int retry = timeout / 1;

	while (checkStatus(STATUS_FLAG_WIP))
	{
		/* do nothing */
		if (retry-- == 0)
		{
			// DEBUG_ERROR("erase failed! timeout!");
			return -1;
		}

		DelayMS(1);
	}	

	if (0 != commandWriteEnable())
	{
		// DEBUG_ERROR("command write enable failed!");
		return -1;
	}

	uint8_t cmd[4] = {0};

	cmd[0] = FLASH_CMD_PP;
	cmd[1] = (address >> 16);
	cmd[2] = (address >> 8);
	cmd[3] = address;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, false))
	{
		// DEBUG_ERROR("send command PP failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}

	if (size != SPI_Transmit(NRF_SPI0, (uint8_t *)buf, size, timeout, false))
	{
		// DEBUG_ERROR("Program page failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}

	FLASH_SPI_DISABLE();

	return 0;
}

static int commandRead(uint32_t address, uint8_t *buf, int size, int timeout)
{
	if (address > MX25L4006E_ADDRESS_MAX)
	{
		// DEBUG_ERROR("invalid address!");
		return -1;
	}

	int retry = timeout / 1;

	while (checkStatus(STATUS_FLAG_WIP))
	{
		/* do nothing */
		if (retry-- == 0)
		{
			// DEBUG_ERROR("erase failed! timeout!");
			return -1;
		}

		DelayMS(1);
	}	

	uint8_t cmd[4] = {0};

	cmd[0] = FLASH_CMD_READ;
	cmd[1] = address >> 16;
	cmd[2] = address >> 8;
	cmd[3] = address;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, false))
	{
		// DEBUG_ERROR("send command READ failed!");

		FLASH_SPI_DISABLE();
		
		return -1;
	}

	if (size != SPI_Transmit(NRF_SPI0, buf, size, timeout, true))
	{
		// DEBUG_ERROR("read data failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}

	FLASH_SPI_ENBALE();

	return size;
}

static int commandSleep(void)
{
	uint8_t cmd[1] = {0};

	cmd[0] = FLASH_CMD_DP;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, true))
	{
		// DEBUG_ERROR("send command DP failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();

	DelayUS(10);

	return 0;
}

static int commandWakeup(void)
{
	uint8_t cmd[1] = {0};

	cmd[0] = FLASH_CMD_RDP;

	FLASH_SPI_ENBALE();
	if (sizeof(cmd) != SPI_Transmit(NRF_SPI0, cmd, sizeof(cmd), 5, true))
	{
		// DEBUG_ERROR("send command RDP failed!");

		FLASH_SPI_DISABLE();

		return -1;
	}
	FLASH_SPI_DISABLE();

	DelayUS(10);
	
	return 0;
}

void MX25L4006E_Init(void)
{
	/* init SPI0 */
	SPI_Deinit(NRF_SPI0);
	
	SPI_InitType SPI_InitStruct;
	SPI_InitStruct.sck_pin_number 		= BOARD_PIN_SPI_SCK;
	SPI_InitStruct.mosi_pin_number 		= BOARD_PIN_SPI_MOSI;
	SPI_InitStruct.miso_pin_number 		= BOARD_PIN_SPI_MISO;
	SPI_InitStruct.freq			 		= SPI_FREQUENCY_FREQUENCY_M8;
	SPI_InitStruct.mode			 		= SPI_MODE0;
	SPI_InitStruct.lsb_first			= false;

	SPI_Init(NRF_SPI0, &SPI_InitStruct);

	/* init CS pin */
	GPIO_InitType GPIO_InitStruct;
	GPIO_InitStruct.dir 	= GPIO_PIN_CNF_DIR_Output;
	GPIO_InitStruct.pull 	= GPIO_PIN_CNF_PULL_Disabled;
	GPIO_InitStruct.drive 	= GPIO_PIN_CNF_DRIVE_S0S1;
	GPIO_InitStruct.sense	= GPIO_PIN_CNF_SENSE_Disabled;
	GPIO_InitStruct.input 	= GPIO_PIN_CNF_INPUT_Disconnect;

	GPIO_Init(BOARD_PIN_FLASH_CS, &GPIO_InitStruct);
	GPIO_SetBit(BOARD_PIN_FLASH_CS);

	/* check MX25L4006E IC */
	checkID();
	checkElectronicID();
	checkElectronicAndDeviceID();
}

int MX25L4006E_EraseChip(int timeout, bool waitUntil)
{
	if (0 != commandChipErase(timeout))
	{
		// DEBUG_ERROR("erase chip failed!");
		return -1;
	}

	if (waitUntil)
	{
		int retry = timeout / 1;

		while (checkStatus(STATUS_FLAG_WIP))
		{
			if (retry-- == 0)
			{
				// DEBUG_ERROR("erase chip timeout!");
				return -1;
			}

			DelayMS(1);
		}
	}

	return 0;
}

int MX25L4006E_EraseSector(uint32_t sectorNumber, int timeout, bool waitUntil)
{
	if (sectorNumber >= MX25L4006E_SECTOR_COUNT)
	{
		// DEBUG_ERROR("invalid sector number!");
		return -1;
	}

	if (0 != commandSectorErase(MX25L4006E_SECTOR_SIZE * sectorNumber, timeout))
	{
		// DEBUG_ERROR("erase sector%d failed!", sectorNumber);
		return -1;
	}

	if (waitUntil)
	{
		int retry = timeout / 1;

		while (checkStatus(STATUS_FLAG_WIP))
		{
			if (retry-- == 0)
			{
				// DEBUG_ERROR("erase sector timeout!");
				return -1;
			}

			DelayMS(1);
		}
	}

	return 0;
}

int MX25L4006E_Write(uint32_t address, const uint8_t *buf, int size, int timeout)
{
	return commandPageProgram(address, buf, size, timeout);
}

int MX25L4006E_Read(uint32_t address, uint8_t *buf, int size, int timeout)
{
	return commandRead(address, buf, size, timeout);
}

int MX25L4006E_Sleep(void)
{
	return commandSleep();
}

int MX25L4006E_WakeUp(void)
{
	return commandWakeup();
}

bool MX25L4006E_IsBusy(void)
{
	return (checkStatus(STATUS_FLAG_WIP) != 0);
}

void MX25L4006E_Test(void)
{
	uint8_t buf[128];
	
	MX25L4006E_EraseSector(0, 300, true);

	MX25L4006E_Write(0, "This is test of sector 0", 
						strlen("This is test of sector 0"), 10);
	
	MX25L4006E_Read(0, buf, sizeof(buf), 10);

	char *strbuf = LX_Malloc(1024); 
	memset(strbuf, 0, 1024);
	strcat(strbuf, "\n");
	for (int i = 0; i < sizeof(buf); i++)
	{
		char strbyte[16];
		sprintf(strbyte, "%c ", buf[i]);
		strcat(strbuf, strbyte);
		if (i % 16 == 0)
		{
			strcat(strbuf, "\r\n");
		}
	}

	// DEBUG_LOG("buf is: %s", strbuf);
	LX_Free(strbuf);

	MX25L4006E_EraseSector(4096, 300, true);

	MX25L4006E_Write(4096, "HelloWorld this is from me! Lalala", 
						strlen("HelloWorld this is from me! Lalala"), 10);
	
	MX25L4006E_Read(4096, buf, sizeof(buf), 10);

	strbuf = LX_Malloc(1024); 
	memset(strbuf, 0, 1024);
	strcat(strbuf, "\n");
	for (int i = 0; i < sizeof(buf); i++)
	{
		char strbyte[16];
		sprintf(strbyte, "%c ", buf[i]);
		strcat(strbuf, strbyte);
		if (i % 16 == 0)
		{
			strcat(strbuf, "\r\n");
		}
	}

	// DEBUG_LOG("buf is: %s", strbuf);
	LX_Free(strbuf);
}
