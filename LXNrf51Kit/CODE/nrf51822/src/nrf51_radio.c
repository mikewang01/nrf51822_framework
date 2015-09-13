#include "nrf51_radio.h"

#include "lx_nrf51Kit.h"
#include "debug.h"

#define PACKET0_S1_SIZE                  (0UL)  //!< S1 size in bits
#define PACKET0_S0_SIZE                  (0UL)  //!< S0 size in bits
#define PACKET0_PAYLOAD_SIZE             (0UL)  //!< payload size in bits
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
#define PACKET1_STATIC_LENGTH            (64UL)  //!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (64UL)  //!< payload size in bytes

#define RADIO_FIFO_SIZE                  (512UL)

typedef enum
{
    RADIO_STATUS_IDLE = 1,
    RADIO_STATUS_RX,
    RADIO_STATUS_TX,

}RADIO_Status;

static RADIO_Status status;
static uint8_t radio_packet[PACKET1_STATIC_LENGTH];

static LX_FIFOType fifoTX;
static LX_FIFOType fifoRX;

static void startRX(void);
static void startTX(void);

void RADIO_IRQHandler(void)
{
    if (NRF_RADIO->EVENTS_END != 0)
    {
        NRF_RADIO->EVENTS_END = 0;

        if (status == RADIO_STATUS_RX)
        {
            if (NRF_RADIO->CRCSTATUS == 1U)
            {
                // DEBUG_LOG("radio_buf is: %s", radio_packet);
                uint8_t len = radio_packet[0];

                if (0 != LX_FIFOWrite(fifoRX, radio_packet + 1, len))
                {
                    DEBUG_LOG("fifo is full!");
                }
            }  
        }
        else if (status == RADIO_STATUS_TX)
        {
            /* Do not have nothing to do. */
        } 

        status = RADIO_STATUS_IDLE;

        if (0 < LX_FIFOCount(fifoTX))
        {
            DelayMS(5);
            startTX();
        }
        else
        {
            startRX();
        }
    }
}

static void startRX(void)
{
    if (status == RADIO_STATUS_RX)
    {
        return;
    }
    status = RADIO_STATUS_RX;

    /* Disable radio */
    NRF_RADIO->EVENTS_DISABLED   = 0U;
    NRF_RADIO->TASKS_DISABLE     = 1U;
    while (NRF_RADIO->EVENTS_DISABLED == 0U)
    {
        /* wait for disable finish */
    }

    /* Enable radio RX and wait for ready */
    NRF_RADIO->EVENTS_READY      = 0U;
    NRF_RADIO->TASKS_RXEN        = 1U;
    while (NRF_RADIO->EVENTS_READY == 0U)
    {
        /* wait for RX enable finish */
    }

    /* Start RX */
    NRF_RADIO->TASKS_START = 1U;
}

static void startTX(void)
{
    if (status == RADIO_STATUS_TX)
    {
        return;
    }
    status = RADIO_STATUS_TX;

    int len;

    if (0 > (len = LX_FIFORead(fifoTX, radio_packet + 1, PACKET1_PAYLOAD_SIZE - 1)))
    {
        DEBUG_LOG("get TX data failed! len is: %d", len);
        return;
    }
    radio_packet[0] = len;

    /* Disable radio */
    NRF_RADIO->EVENTS_DISABLED  = 0U;
    NRF_RADIO->TASKS_DISABLE    = 1U;
    while (NRF_RADIO->EVENTS_DISABLED == 0U)
    {
        /* wait for disable finish */
    }

    /* Enable radio TX and wait for ready */
    NRF_RADIO->EVENTS_READY     = 0U;
    NRF_RADIO->TASKS_TXEN       = 1;
    while (NRF_RADIO->EVENTS_READY == 0U)
    {
        /* wait for TX enable finish */
    }
    
    /* Start TX */
    NRF_RADIO->TASKS_START      = 1U;
}

void RADIO_Init(void)
{
    // Radio power
    NRF_RADIO->POWER = (RADIO_POWER_POWER_Enabled << RADIO_POWER_POWER_Pos);

    // Radio config
    NRF_RADIO->TXPOWER    = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
    NRF_RADIO->FREQUENCY  = 15UL;                // Frequency bin 7, 2407MHz
    NRF_RADIO->MODE       = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);

    // Radio address config
    NRF_RADIO->PREFIX0        = 0xC4C3C2E7UL;  // Prefix byte of addresses 3 to 0
    NRF_RADIO->PREFIX1        = 0xC5C6C7C8UL;  // Prefix byte of addresses 7 to 4
    NRF_RADIO->BASE0          = 0xE7E7E7E7UL;  // Base address for prefix 0
    NRF_RADIO->BASE1          = 0x00C2C2C2UL;  // Base address for prefix 1-7
    NRF_RADIO->TXADDRESS      = 0x00UL;      // Set device address 0 to use when transmitting
    NRF_RADIO->RXADDRESSES    = 0x01UL;    // Enable device address 0 to use which receiving

    // Packet configuration
    NRF_RADIO->PCNF0 = (PACKET0_S1_SIZE << RADIO_PCNF0_S1LEN_Pos) |
                       (PACKET0_S0_SIZE << RADIO_PCNF0_S0LEN_Pos) |
                       (PACKET0_PAYLOAD_SIZE << RADIO_PCNF0_LFLEN_Pos);

    // Packet configuration
    NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
                       (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)           |
                       (PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)       |
                       (PACKET1_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos)           |
                       (PACKET1_PAYLOAD_SIZE << RADIO_PCNF1_MAXLEN_Pos);

    // CRC Config
    NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
    if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
    {
        NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value      
        NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
    }
    else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
    {
        NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
        NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
    }

    DelayMS(3);

    // Set payload pointer
    NRF_RADIO->PACKETPTR = (uint32_t)radio_packet;

    // Enable END interrupt
    NRF_RADIO->EVENTS_END   = 0;
    NRF_RADIO->INTENCLR     = 0xFFFFFFFFUL;
    NRF_RADIO->INTENSET     |= RADIO_INTENSET_END_Set << RADIO_INTENSET_END_Pos;

    NVIC_ClearPendingIRQ(RADIO_IRQn);
    NVIC_SetPriority(RADIO_IRQn, 3);
    NVIC_EnableIRQ(RADIO_IRQn);

    // Create RX fifo
    fifoRX = LX_FIFOCreate(RADIO_FIFO_SIZE);
    fifoTX = LX_FIFOCreate(RADIO_FIFO_SIZE);

    // start RX
    startRX();
}

void RADIO_Deinit(void)
{
    NRF_RADIO->POWER = (RADIO_POWER_POWER_Disabled << RADIO_POWER_POWER_Pos);

    LX_FIFODestroy(fifoRX);
    LX_FIFODestroy(fifoTX);
}

void RADIO_TX(const uint8_t *buf, int size)
{
    if (0 != LX_FIFOWrite(fifoTX, buf, size))
    {
        DEBUG_LOG("tx fifo is full!");
        return;
    }

    startTX();
}

int RADIO_RX(uint8_t *buf, int size, int timeout)
{
    /* retry every 1ms */
    int retry = timeout / 1; 
    
    for (;;)
    {
        if (LX_FIFOCount(fifoRX) < size)
        {
            /* return cause timeout */
            if (retry-- <= 0)
            {
                break;
            }

            DelayMS(1);
            continue;
        }

        break;
    }

    if (LX_FIFOCount(fifoRX) == 0)
    {
        return 0;
    }

    int readLen = LX_FIFORead(fifoRX, (uint8_t *)buf, size);

    return readLen;
}

void RADIO_Clear(void)
{
    LX_FIFOFlush(fifoRX);
    LX_FIFOFlush(fifoTX);
}
