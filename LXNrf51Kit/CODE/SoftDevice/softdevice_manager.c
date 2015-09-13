#include "softdevice_manager.h"

#include "nrf_soc.h"
#include "ble.h"

#include "lx_nrf51Kit.h"
#include "app.h"

volatile static bool is_softdevice_enabled = false;

void softdevice_assertion_handler(uint32_t pc, uint16_t line_num, const uint8_t *file_name)
{
    DEBUG_LOG("SoftDevice failed!");

    NVIC_SystemReset();
}

/**@brief   Function for handling the Application's BLE Stack events interrupt.
 *
 * @details This function is called whenever an event is ready to be pulled.
 */
void SWI2_IRQHandler(void)
{
    /* put this event into NotificationCenter */
    LX_NotificationType msg = {0};
    msg.type = APP_MSG_SOFTDEVICE_EVENT;
    LX_NotificationCenterPut(&msg);
}

int Softdevice_Init(nrf_clock_lfclksrc_t clock_source)
{   
    // Initialize SoftDevice.
    if (NRF_SUCCESS != sd_softdevice_enable(clock_source, softdevice_assertion_handler))
    {
        return -1;
    }
    
    // Enable BLE event interrupt (interrupt priority has already been set by the stack).
    if (NRF_SUCCESS != sd_nvic_EnableIRQ(SWI2_IRQn))
    {
        return -1;
    }

    is_softdevice_enabled = true;

    if (NRF_SUCCESS != sd_power_mode_set(NRF_POWER_MODE_LOWPWR))
    {
        DEBUG_LOG("sd_power_mode_set failed!");
    }

    return 0;
}

void Softdevice_Deinit(void)
{
    sd_softdevice_disable();

    is_softdevice_enabled = false;
}

bool Softdevice_IsEnabled(void)
{
    return is_softdevice_enabled;
}

