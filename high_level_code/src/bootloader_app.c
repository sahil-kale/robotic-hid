#include "bootloader_app.h"
#include "hal_bootloader_app.h"
#include "hal_dfu.h"
#include "common.h"

bootloader_state_E bootloader_current_state = BOOTLOADER_STATE_RESET;

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

void bootloader_app_run(void)
{
    switch(bootloader_current_state)
    {
        case BOOTLOADER_STATE_RESET:;
            bootloader_current_state = determine_bootloader_state_from_reset_vector();
            break;
        case BOOTLOADER_STATE_LAUNCH_APP:;
            application_info_flash_t info = read_application_info();
            if(info.flash_valid && hal_dfu_validate_crc(APP_START_ADDRESS, info.application_size, info.application_crc))
            {
                hal_jump_to_app();
            }
            else
            {
                bootloader_current_state = BOOTLOADER_STATE_HALT;
            }
            break;
        case BOOTLOADER_STATE_DFU:;
            break;
        case BOOTLOADER_STATE_HALT:;
            break;
        default:
            break;
    }
}
