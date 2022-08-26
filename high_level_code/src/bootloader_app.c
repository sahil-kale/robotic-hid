#include "bootloader_app.h"
#include "hal_bootloader_app.h"

bootloader_state_E determine_bootloader_state_from_reset_vector(void)
{
    bootloader_state_E state = BOOTLOADER_STATE_RESET;

    application_info_flash_t info = read_application_info();
    if(info.dfu_request)
    {
        state = BOOTLOADER_STATE_DFU;
    }
    else
    {
        state = BOOTLOADER_STATE_LAUNCH_APP;
    }

    return state;
}