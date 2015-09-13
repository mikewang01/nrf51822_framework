#ifndef _MX25L4006E_H_
#define _MX25L4006E_H_

#include "lx_nrf51Kit.h"


//Byte orgnization
#define		MX25L4006E_PAGE_SIZE				256
#define 	MX25L4006E_PAGE_COUNT				2048

#define    	MX25L4006E_SECTOR_SIZE				4096UL
#define     MX25L4006E_SECTOR_COUNT				128

#define		MX25L4006E_BLOCK_SIZE				MX25L4006E_SECTOR_SIZE * 16
#define 	MX25L4006E_BLOCK_COUNT				8

#define    	MX25L4006E_ADDRESS_MAX 				(MX25L4006E_SECTOR_SIZE * MX25L4006E_SECTOR_COUNT)

/*
 * Initialize MX25L4006E
 */
void MX25L4006E_Init(void);

/*
 * Erase whole chip
 */
int MX25L4006E_EraseChip(int timeout, bool waitUntil);

/*
 * Erase a sector
 * 1 sector = 16 page = 4096 Byte
 */
int MX25L4006E_EraseSector(uint32_t blockNumber, int timeout, bool waitUntil);

/*
 * Write bytes to a specific address.
 * Note: address + size should not exceed one page. That is: address & 0xFF + size > 256
 * Timeout is in millisecond
 */
int MX25L4006E_Write(uint32_t address, const uint8_t *buf, int size, int timeout);

/*
 * Read bytes from a specific address.
 * Timeout is in millisecond
 */
int MX25L4006E_Read(uint32_t address, uint8_t *buf, int size, int timeout);

/*
 * Make MX25L4006E enter sleep mode
 */
int MX25L4006E_Sleep(void);

/*
 * Wake up MX25L4006E from sleep mode
 */
int MX25L4006E_WakeUp(void);

/*
 * Check is busy
 */
bool MX25L4006E_IsBusy(void);


void MX25L4006E_Test(void);

#endif	/* _MX25L4006E_H_ */
